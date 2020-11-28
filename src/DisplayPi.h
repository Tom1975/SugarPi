#pragma once

//
#include <circle/screen.h>
#include <circle/logger.h>
#include <circle/bcmframebuffer.h>

#include "CPCCore/CPCCoreEmu/Screen.h"

#define FRAME_BUFFER_SIZE 2

class DisplayPi : public IDisplay
{
public:
   DisplayPi(CLogger* logger, CTimer* timer);
   virtual ~DisplayPi();

   bool Initialization();
   void SyncWithFrame (bool set){sync_on_frame_ = set;}
   bool IsSyncOnFrame(){return sync_on_frame_;}
   
   void SetFullResolution (bool set){full_resolution_ = set;};

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

   CBcmFrameBuffer* GetFrameBuffer() {
      return &frame_buffer_;   }

   void Lock() { mutex_.Acquire(); }
   void Unlock() { mutex_.Release(); }

   void Loop();

protected:
   //CScreenDevice*		screen_;
   CLogger* logger_;
   CTimer* timer_;
   CBcmFrameBuffer   frame_buffer_;
   bool full_resolution_;
   bool full_resolution_cached_;

   CSpinLock   mutex_;

   unsigned int added_line_;
   unsigned int last_tick_frame_;

   // Frame buffer availability
   typedef enum
   {
      FR_FREE,
      FR_USED,
      FR_READY
   } FrameState;
   FrameState frame_used_[FRAME_BUFFER_SIZE];
   unsigned int buffer_used_;

   unsigned int frame_queue_[FRAME_BUFFER_SIZE];
   unsigned int nb_frame_in_queue_;

   bool sync_on_frame_;
};