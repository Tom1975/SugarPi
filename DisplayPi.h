#pragma once

//
#include <circle/screen.h>
#include <circle/logger.h>
#include <circle/bcmframebuffer.h>

#include "CPCCore/CPCCoreEmu/Screen.h"


class DisplayPi : public IDisplay
{
public:
   DisplayPi(CLogger* logger);
   virtual ~DisplayPi();

   bool Initialization();
   //CScreenDevice* GetScreenDevice() { return screen_; }

   virtual void SetScanlines(int scan);
   virtual bool AFrameIsReady();
   virtual void Display();

   virtual void Config();
   virtual const char* GetInformations();
   virtual int GetWidth();
   virtual int GetHeight();
   virtual void SetSize(SizeEnum size);
   virtual SizeEnum  GetSize();
   virtual void VSync(bool dbg = false);

   // Start of sync
   virtual void StartSync();
   // Wait VBL
   virtual void WaitVbl();


   virtual int* GetVideoBuffer(int y);
   virtual void Reset();
   virtual void FullScreenToggle();
   virtual void ForceFullScreen(bool fullscreen);
   virtual void Screenshot();
   virtual void ScreenshotEveryFrame(int on);
   virtual bool IsEveryFrameScreened();

   virtual bool SetSyncWithVbl(int speed);
   virtual bool IsWaitHandled();
   virtual bool IsDisplayed();
   virtual bool GetBlackScreenInterval();
   virtual void SetBlackScreenInterval(bool on);

   virtual void WindowChanged(int x_in, int y_in, int wnd_width, int wnd_height);

   virtual void ResetLoadingMedia();
   virtual void SetLoadingMedia();

   virtual void ResetDragnDropDisplay();
   virtual void SetDragnDropDisplay(int type);
   virtual void SetCurrentPart(int x, int y);
   virtual int GetDnDPart();

   // Capability of device
   virtual bool CanVSync() { return true; }
   virtual bool CanInsertBlackFrame() { return false; }
   virtual void Activate(bool on) {};

protected:
   //CScreenDevice*		screen_;
   CLogger*          logger_;

   CBcmFrameBuffer   frame_buffer_;

   unsigned int added_line_;
};