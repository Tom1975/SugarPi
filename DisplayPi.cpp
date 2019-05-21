//
#include "DisplayPi.h"


DisplayPi::DisplayPi(CLogger* logger) : 
   logger_(logger),
   frame_buffer_(1024, 768, 32, 640, 480)
{
   screen_ = new CScreenDevice(1024, 768);
}

DisplayPi::~DisplayPi()
{
   
}

bool DisplayPi::Initialization()
{
   bool ret = screen_->Initialize();

   // draw rectangle on screen
   for (unsigned nPosX = 0; nPosX < screen_->GetWidth(); nPosX++)
   {
      screen_->SetPixel(nPosX, 0, NORMAL_COLOR);
      screen_->SetPixel(nPosX, screen_->GetHeight() - 1, NORMAL_COLOR);
   }
   for (unsigned nPosY = 0; nPosY < screen_->GetHeight(); nPosY++)
   {
      screen_->SetPixel(0, nPosY, NORMAL_COLOR);
      screen_->SetPixel(screen_->GetWidth() - 1, nPosY, NORMAL_COLOR);
   }

   // draw cross on screen
   for (unsigned nPosX = 0; nPosX < screen_->GetWidth(); nPosX++)
   {
      unsigned nPosY = nPosX * screen_->GetHeight() / screen_->GetWidth();

      screen_->SetPixel(nPosX, nPosY, NORMAL_COLOR);
      screen_->SetPixel(screen_->GetWidth() - nPosX - 1, nPosY, NORMAL_COLOR);
   }

   frame_buffer_.Initialize();
   return ret;
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
   return "Display for Raspberry PI3 - Bare metal";
}

int DisplayPi::GetWidth()
{
   logger_->Write("Display", LogNotice, "GetWidth - NOT IMPLEMENTED ");
   return 1024;
}

int DisplayPi::GetHeight()
{
   logger_->Write("Display", LogNotice, "GetHeight - NOT IMPLEMENTED ");
   return 768;
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

void DisplayPi::VSync(bool dbg )
{
   
}

// Start of sync
void DisplayPi::StartSync()
{
   logger_->Write("Display", LogNotice, "StartSync - NOT IMPLEMENTED ");
}

// Wait VBL
void DisplayPi::WaitVbl()
{
   logger_->Write("Display", LogNotice, "WaitVbl - NOT IMPLEMENTED ");
}

int* DisplayPi::GetVideoBuffer(int y)
{
   return (int*)(frame_buffer_.GetBuffer() + y* frame_buffer_.GetPitch());
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
