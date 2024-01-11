#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif

#include "CPCCore/CPCCoreEmu/Screen.h"


class CoolspotFont;
#define FRAME_BUFFER_SIZE 3


class DisplayPi : public IDisplay
{
public:
   DisplayPi(CLogger* logger);
   virtual ~DisplayPi();

   virtual bool Initialization();
   virtual void SyncWithFrame (bool set){sync_on_frame_ = set;}
   virtual bool IsSyncOnFrame(){return sync_on_frame_;}
   
   virtual void SetFullResolution (bool set){full_resolution_ = set;};

   virtual void SetScanlines(int scan);
   virtual bool AFrameIsReady();
   virtual void Display();

   virtual void Config();
   virtual const char* GetInformations();
   virtual int GetWidth() = 0;
   virtual int GetHeight() = 0;
   virtual void SetSize(SizeEnum size);
   virtual SizeEnum  GetSize();
   virtual void VSync(bool dbg = false);

   // Start of sync
   virtual void StartSync();
   // Wait VBL
   virtual void WaitVbl();

   // Services
   virtual void DisplayText(const char* txt, int x, int y, bool selected = false);

   virtual void Reset();
   virtual void FullScreenToggle();
   virtual void ForceFullScreen(bool fullscreen);
   virtual void Screenshot();
   virtual void Screenshot(const char* scr_path);
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
   virtual void Loop();
   virtual void StopLoop();

   virtual void Lock() = 0;
   virtual void Unlock() = 0;

   virtual int* GetVideoBuffer(int y) = 0;
   virtual int GetStride() = 0;

   virtual void SetFrame(int frame_index) = 0;
   virtual void Draw() = 0;
   virtual void ClearBuffer(int frame_index) = 0;
protected:
   //CScreenDevice*		screen_;
   CLogger* logger_;
   bool full_resolution_;
   bool full_resolution_cached_;

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
   bool loop_run;
   CoolspotFont *font_;
};