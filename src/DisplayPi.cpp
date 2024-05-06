//
#include "DisplayPi.h"

#include <memory.h>

#include "res/button_1.h"
#include "res/coolspot.h"

#ifdef  __circle__
#define WAIT(x) CTimer::Get ()->MsDelay(x)
#else
#include <chrono>
#include <thread>

#define WAIT(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));
#endif

#define REAL_DISP_X  1024 //832 //1024 // 768
#define REAL_DISP_Y  (288*2) // 624 //-16 //624 //576
//HEIGHT_VIRTUAL_SCREEN (288*2)

DisplayPi::DisplayPi(CLogger* logger) :
   logger_(logger),
   full_resolution_(false),
   full_resolution_cached_(false),
   added_line_(1),
   current_buffer_(0),
   nb_frame_in_queue_(0),
   sync_on_frame_(false)
{
   // Create backbuffers
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
      frame_used_[i] = FR_FREE;
      display_buffer_[i] = new int [REAL_DISP_X * REAL_DISP_Y];
   }
   frame_used_[current_buffer_] = FR_USED;

   
}

DisplayPi::~DisplayPi()
{
   delete font_;
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
      delete []display_buffer_[i];
   }
}

bool DisplayPi::Initialization()
{
   font_ = new CoolspotFont(GetStride());
   return true;
}

void DisplayPi::SetScanlines(int scan)
{
   
}

bool DisplayPi::AFrameIsReady()
{
   logger_->Write("Display", LogNotice, "A Frame is ready - NOT IMPLEMENTED ");
   return false;
}

void DisplayPi::Display()
{
   logger_->Write("Display", LogNotice, "Display - NOT IMPLEMENTED ");
}

void DisplayPi::Config()
{
   logger_->Write("Display", LogNotice, "Config - NOT IMPLEMENTED ");
}

const char* DisplayPi::GetInformations()
{
   logger_->Write("Display", LogNotice, "Get Information ");
   return "Display for Raspberry PI - Bare metal";
}

void DisplayPi::SetSize(IDisplay::SizeEnum size)
{
   logger_->Write("Display", LogNotice, "SetSize - NOT IMPLEMENTED ");
}

IDisplay::SizeEnum  DisplayPi::GetSize()
{
   logger_->Write("Display", LogNotice, "SetSize - NOT IMPLEMENTED ");
   return IDisplay::S_STANDARD;
}

// Start of sync
void DisplayPi::StartSync()
{
   logger_->Write("Display", LogNotice, "StartSync - NOT IMPLEMENTED ");
}

// Wait VBL
void DisplayPi::WaitVbl()
{
   logger_->Write("Display", LogNotice, "WaitVbl");
   Draw();
}

void DisplayPi::FullScreenToggle()
{
   logger_->Write("Display", LogNotice, "FullScreenToggle - NOT IMPLEMENTED ");
}

void DisplayPi::ForceFullScreen(bool fullscreen)
{
   logger_->Write("Display", LogNotice, "ForceFullScreen - NOT IMPLEMENTED ");
}

void DisplayPi::Screenshot()
{
   logger_->Write("Display", LogNotice, "Screenshot - NOT IMPLEMENTED ");

}

void DisplayPi::Screenshot(const char* scr_path)
{
   logger_->Write("Display", LogNotice, "Screenshot - NOT IMPLEMENTED ");
}

void DisplayPi::ScreenshotEveryFrame(int on)
{
   logger_->Write("Display", LogNotice, "ScreenshotEveryFrame - NOT IMPLEMENTED ");
}

bool DisplayPi::IsEveryFrameScreened()
{
   logger_->Write("Display", LogNotice, "IsEveryFrameScreened - NOT IMPLEMENTED ");
   return false;
}

bool DisplayPi::SetSyncWithVbl(int speed)
{
   logger_->Write("Display", LogNotice, "SetSyncWithVbl - NOT IMPLEMENTED ");
   return true;
}

bool DisplayPi::IsWaitHandled()
{
   logger_->Write("Display", LogNotice, "IsWaitHandled - NOT IMPLEMENTED ");
   return true;
}

bool DisplayPi::IsDisplayed()
{
   return true;
}

bool DisplayPi::GetBlackScreenInterval()
{
   logger_->Write("Display", LogNotice, "GetBlackScreenInterval - NOT IMPLEMENTED ");
   return false;

}

void DisplayPi::SetBlackScreenInterval(bool on)
{
   logger_->Write("Display", LogNotice, "SetBlackScreenInterval - NOT IMPLEMENTED ");
}


void DisplayPi::WindowChanged(int x_in, int y_in, int wnd_width, int wnd_height)
{
   logger_->Write("Display", LogNotice, "WindowChanged - NOT IMPLEMENTED ");
}

void DisplayPi::ResetLoadingMedia()
{
   logger_->Write("Display", LogNotice, "ResetLoadingMedia - NOT IMPLEMENTED ");
}

void DisplayPi::SetLoadingMedia()
{
   logger_->Write("Display", LogNotice, "SetLoadingMedia - NOT IMPLEMENTED ");
}

void DisplayPi::ResetDragnDropDisplay()
{
   logger_->Write("Display", LogNotice, "ResetDragnDropDisplay - NOT IMPLEMENTED ");

}

void DisplayPi::SetDragnDropDisplay(int type)
{
   logger_->Write("Display", LogNotice, "SetDragnDropDisplay - NOT IMPLEMENTED ");
}

void DisplayPi::SetCurrentPart(int x, int y)
{
   logger_->Write("Display", LogNotice, "SetCurrentPart - NOT IMPLEMENTED ");
}

int DisplayPi::GetDnDPart()
{
   logger_->Write("Display", LogNotice, "GetDnDPart - NOT IMPLEMENTED ");
   return 0;
}


void DisplayPi::StopLoop()
{
}

void DisplayPi::Loop()
{
   // Display available frame
   int frame_index = -1;
   Lock();
   if (nb_frame_in_queue_ > 0)
   {
         
      frame_index = frame_queue_[0];
      //logger_->Write("DIS", LogNotice, "A frame is present. nb_frame_in_queue_ = %i; frame_index = %i", nb_frame_in_queue_, frame_index);
      nb_frame_in_queue_--;

      memmove(frame_queue_, &frame_queue_[1], nb_frame_in_queue_ * sizeof(unsigned int));
      SetFrame(frame_index);

      //logger_->Write("DIS", LogNotice, "frame_index : %i", frame_index);
      Draw();
      // Set it as available
      //logger_->Write("DIS", LogNotice, "Set current slot as free ! : %i", frame_index);
      frame_used_[frame_index] = FR_FREE;
      Unlock();
   }
   else
   {
      Unlock();
      WAIT(1);
   }
   // sleep ?

}

void DisplayPi::VSync(bool dbg)
{
   bool clear_framebuffer = false;
   if (full_resolution_cached_ != full_resolution_)
   {
      clear_framebuffer = true;
      full_resolution_cached_ = full_resolution_;
   }

   Lock();

   // The frame is ready : Add it to the queue
   //logger_->Write("DIS", LogNotice, "VSync : nb_frame_in_queue_ = %i", nb_frame_in_queue_);
   bool found = false;

#if 0
   for (int i = 0; i < FRAME_BUFFER_SIZE && !found; i++)
   {
      if (frame_used_[i] == FR_FREE)
      {
         logger_->Write("VSync", LogNotice, "Add a frame in the buffer : %i", i);
         frame_queue_[nb_frame_in_queue_++] = current_buffer_;
         frame_used_[current_buffer_] = FR_READY;

         // Wait for it to be free again !
         /*if (sync_on_frame_)
         {
            Unlock();
            while (frame_used_[current_buffer_] != FR_FREE)
            {
               WAIT(1);
            }
            Lock();
         }*/

         //logger_->Write("DIS", LogNotice, "VSync : a frame is free : %i", i);
         frame_used_[i] = FR_USED;
         current_buffer_ = current_buffer_ = i;
         found = true;
         break;
      }
   }
   if (!found)
   {
      // No more buffer ready...so reuse the one currently added !
      frame_used_[current_buffer_] = FR_USED;
   }
   Unlock();
   #else
   nb_frame_in_queue_ = 1;
   // wait until draw is done
   Unlock();
   while ( nb_frame_in_queue_ == 1)
   {
      WAIT(10);
   }
   #endif
         
   

}

void DisplayPi::DisplayText(const char* txt, int x, int y, bool selected)
{
   // Display text
   int i = 0;

   char buff[16];
   memset(buff, 0, sizeof buff);
   strncpy(buff, txt, 15);
   
   unsigned int x_offset_output = 0;
   //logger_->Write("DisplayText", LogNotice, "DisplayText : %s - Font = %X", txt, font_);
   while (txt[i] != '\0' )
   {

      // Display character
      unsigned char c = txt[i];

      if ( c == ' ' || c >= 0x80)
      {
         x_offset_output += 10;
      }
      else
      {
         
         int* line = GetVideoBuffer(y);
         //font_->Write(c, line + x + x_offset_output);
         font_->CopyLetter(c, &line[x + x_offset_output], GetStride());

         // Look for proper bitmap position (on first line only)
         /*for (int display_y = 0; display_y < font_->GetLetterHeight(c) && GetHeight()>display_y + y; display_y++)
         {
            int* line = GetVideoBuffer(display_y + y);
            font_->CopyLetter(c, display_y, &line[x + x_offset_output]);
         }*/
         x_offset_output += font_->GetLetterLength(c);
      }
      i++;
      
   }
}

int* DisplayPi::GetVideoBuffer(int y)
{
   return (int*)&display_buffer_[current_buffer_][(y)*REAL_DISP_X ];
}

int DisplayPi::GetStride()
{
   return REAL_DISP_X;
}

void DisplayPi::Reset()
{
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
      frame_used_[i] = FR_FREE;
      memset(&display_buffer_[i], 0, REAL_DISP_X * REAL_DISP_Y * FRAME_BUFFER_SIZE);
   }
   
}

int DisplayPi::GetHeight()
{
   return REAL_DISP_Y;
}

int DisplayPi::GetWidth()
{
   return REAL_DISP_X;
}

void DisplayPi::ClearBuffer(int frame_index)
{
   unsigned char* line = reinterpret_cast<unsigned char*>(&display_buffer_[frame_index][REAL_DISP_X * REAL_DISP_Y]);
   for (unsigned int count = 0; count < REAL_DISP_X; count++)
   {
      memset(line, 0x0, REAL_DISP_X * 4);
      line += REAL_DISP_Y * sizeof(int);
   }

}
