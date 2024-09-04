#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif

#include <vector>

#include "BackFrame.h"
#include "MenuFrame.h"
#include "EmulationFrame.h"

#include "CPCCore/CPCCoreEmu/Screen.h"


#define FRAME_BUFFER_SIZE 3


class DisplayPi : public IDisplay
{
public:

   enum ScreenType
   {
      EmulationWindow,
      BackWindow,
      TitleScreen,
      OptionMenu
   };

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
   virtual int GetWidth();
   virtual int GetHeight();
   virtual void SetSize(SizeEnum size);
   virtual SizeEnum  GetSize();
   virtual void VSync(bool dbg = false);

   // Start of sync
   virtual void StartSync();
   // Wait VBL
   virtual void WaitVbl();

   // Services
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


   virtual int* GetVideoBuffer(int y);
   virtual int* GetVideoBuffer(ScreenType screen, int y);


   virtual int GetStride();
   virtual void ClearBuffer(int frame_index);

   virtual void Draw();
   //virtual void CopyMemoryToRessources();

   virtual void BeginDraw() = 0;
   virtual void EndDraw() = 0;
   virtual bool ChangeNeeded(int change) = 0;

   BasicFrame *GetBackgroundFrame() { return &back_frame_; }
   BasicFrame *GetMenuFrame() { return &menu_frame_; }
   BasicFrame *GetEmulationFrame() { return &emu_frame_; }

protected:
   class Frame
   {
   public:
      BasicFrame* frame_;
   };

   virtual void CopyMemoryToRessources(DisplayPi::Frame* frame_) = 0;
   virtual void ChangeAttribute(Frame*, int src_x, int src_y, int src_w, int src_h,
      int dest_x, int dest_y, int dest_w, int dest_h) = 0;

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
   volatile FrameState frame_used_[FRAME_BUFFER_SIZE];
   volatile unsigned int current_buffer_;
   unsigned int frame_queue_[FRAME_BUFFER_SIZE];
   unsigned int nb_frame_in_queue_;

   BackFrame back_frame_;
   MenuFrame menu_frame_;
   EmulationFrame emu_frame_;


   std::vector<DisplayPi::Frame*> windows_list_;

   bool sync_on_frame_;
};