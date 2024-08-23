//
#include "DisplayPi.h"

#include <memory.h>

#include "res/button_1.h"

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
   sync_on_frame_(false)

{
   // Create backbuffers
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
      frame_queue_[i] = 0;
      frame_used_[i] = FR_FREE;
   }

   frame_used_[current_buffer_] = FR_USED;

   
}

DisplayPi::~DisplayPi()
{
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
   }
}

bool DisplayPi::Initialization()
{
   // Get display size, to have a proportional width/height for everybody
   int h = GetHeight();
   int w = GetWidth();
   double ratio = (double)h / (double)w;
   // Backframe : 400 x ???
   unsigned int frame_w = 400;
   unsigned int frame_h = 400 * ratio;
   back_frame_.Init(frame_w, frame_h, FRAME_BUFFER_SIZE);

   // Compute
   frame_w = 1920;
   frame_h = 1920 * ratio;
   menu_frame_.Init(frame_w, frame_h, FRAME_BUFFER_SIZE);
   //menu_frame_.Init(w, h, FRAME_BUFFER_SIZE);

   // Emulation : try to keep those niiiice square pixels
   // todo
   emu_frame_.Init(REAL_DISP_X, REAL_DISP_Y, FRAME_BUFFER_SIZE);

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
   Draw();
}

void DisplayPi::VSync(bool dbg)
{
   Lock();

   // Wait to sync ?
   if (true)
   {
      while (!emu_frame_.CanDrawNewFrame())
      {
         Unlock();
         WAIT(1);
         Lock();
      }
   }
   emu_frame_.FrameIsReady();

   Unlock();
}

int* DisplayPi::GetVideoBuffer(int y)
{
   if ( y < emu_frame_.GetFullHeight())
   {
      return  (int*)&(emu_frame_.GetBuffer()[y * emu_frame_.GetPitch()]);
   }
   else
   {
      
      return  (int*)&(emu_frame_.GetBuffer()[y * emu_frame_.GetPitch()]);
   }
}

 int* DisplayPi::GetVideoBuffer(ScreenType screen, int y)
 {
   switch (screen)
   {
      case EmulationWindow: //return (int*)&display_buffer_[current_buffer_][(y)*REAL_DISP_X ];
      return (int*)&(emu_frame_.GetBuffer()[y * emu_frame_.GetPitch()]);break;
      case OptionMenu: return (int*)&(menu_frame_.GetBuffer()[y * emu_frame_.GetPitch()]);break;
   }
 }

int DisplayPi::GetStride()
{
   return REAL_DISP_X;
}

void DisplayPi::Reset()
{
   emu_frame_.Reset();
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
   {
      frame_used_[i] = FR_FREE;
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
   emu_frame_.Reset(frame_index);   
}

void DisplayPi::Draw()
{
   // Start Drawing
   BeginDraw();

   for (auto it : windows_list_)
   {
      int changed = it->frame_->AttributesHasChanged();

      // Copy memory to ressource
      CopyMemoryToRessources(it);

      // To be use if problems occurs 
      if (ChangeNeeded(changed ))
      {
         ChangeAttribute(it, it->frame_->GetOffsetX(), it->frame_->GetOffsetY(), it->frame_->GetWidth(), it->frame_->GetHeight(),
            it->frame_->GetDisplayX(), it->frame_->GetDisplayY(), it->frame_->GetDisplayWidth(), it->frame_->GetDisplayHeight() );

         it->frame_->AttributesChanged();
      }
   }
   EndDraw();
}

