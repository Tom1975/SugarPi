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


DisplayPi::DisplayPi(CLogger* logger) :
   logger_(logger),
   full_resolution_(false),
   full_resolution_cached_(false),
   added_line_(1),
   buffer_used_(0),
   nb_frame_in_queue_(0),
   sync_on_frame_(false)
{
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
      frame_used_[i] = FR_FREE;
   }
   frame_used_[buffer_used_] = FR_USED;

   
}

DisplayPi::~DisplayPi()
{
   delete font_;
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
   Draw();
}

void DisplayPi::Reset()
{
   logger_->Write("Display", LogNotice, "Reset - NOT IMPLEMENTED ");
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
   loop_run = false;
}

void DisplayPi::Loop()
{
   loop_run = true;
   logger_->Write("DIS", LogNotice, "Starting loop");

   while (loop_run)
   {
      // Display available frame
      int frame_index = -1;
      Lock();
      if (!sync_on_frame_ && nb_frame_in_queue_ > 0)
      {
         
         frame_index = frame_queue_[0];
         logger_->Write("DIS", LogNotice, "A frame is present. nb_frame_in_queue_ = %i; frame_index = %i", nb_frame_in_queue_, frame_index);
         nb_frame_in_queue_--;

         memmove(frame_queue_, &frame_queue_[1], nb_frame_in_queue_ * sizeof(unsigned int));

         Unlock();
         SetFrame(frame_index);
         //logger_->Write("DIS", LogNotice, "frame_index : %i", frame_index);
         Draw();

         // Set it as available
         frame_used_[frame_index] = FR_FREE;
      }
      else
      {
         Unlock();
         WAIT(1);
      }
      // sleep ?

   }
}

void DisplayPi::VSync(bool dbg)
{
   bool clear_framebuffer = false;
   if (full_resolution_cached_ != full_resolution_)
   {
      clear_framebuffer = true;
      full_resolution_cached_ = full_resolution_;
   }

   if (sync_on_frame_) // To turn on : Use the display core !
   {
      Lock();
      nb_frame_in_queue_ = 0;
      Unlock();
      logger_->Write("DIS", LogNotice, "A frame is present. sync_on_frame_; frame_index = %i", buffer_used_);
      SetFrame(buffer_used_);
      Draw();

      if (clear_framebuffer)
      {
         ClearBuffer(buffer_used_);
      }
   }
   else
   {
      Lock();

      // The frame is ready : Add it to the queue
      //logger_->Write("DIS", LogNotice, "VSync : nb_frame_in_queue_ = %i", nb_frame_in_queue_);
      bool found = false;
      for (int i = 0; i < FRAME_BUFFER_SIZE && !found; i++)
      {
         if (frame_used_[i] == FR_FREE)
         {
            frame_queue_[nb_frame_in_queue_++] = buffer_used_;
            frame_used_[buffer_used_] = FR_READY;
            //logger_->Write("DIS", LogNotice, "VSync : a frame is free : %i", i);
            frame_used_[i] = FR_USED;
            buffer_used_ = i;
            found = true;
            break;
         }
      }
      if (!found)
      {
         // No more buffer ready...so reuse the one currently added !
         frame_used_[buffer_used_] = FR_USED;
      }
         

      Unlock();
   }
   added_line_ = 1;
}

void DisplayPi::DisplayText(const char* txt, int x, int y, bool selected)
{
   // Display text
   int i = 0;

   char buff[16];
   memset(buff, 0, sizeof buff);
   strncpy(buff, txt, 15);
   
   unsigned int x_offset_output = 0;

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
         font_->Write(c, &line[x + x_offset_output]);
         //font_->CopyLetter(c, &line[x + x_offset_output], GetStride());

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