#pragma once


#include <circle/screen.h>
#include <circle/logger.h>
#include <circle/bcmframebuffer.h>

#include <bcm_host.h>

#include "DisplayPi.h"

class DisplayPiImp : public DisplayPi
{
public:
   DisplayPiImp(CLogger* logger, CTimer* timer);
   virtual ~DisplayPiImp();

   bool ListEDID();
   
   bool Initialization();
   void SyncWithFrame (bool set);
   bool IsSyncOnFrame(){return sync_on_frame_;}
   
   void SetFullResolution (bool set){full_resolution_ = set;};

   virtual void SetScanlines(int scan);

   virtual const char* GetInformations();

   virtual void WaitVbl();
   virtual int* GetVideoBuffer(int y);
   virtual int GetStride();

   virtual int GetWidth();
   virtual int GetHeight();

   // Capability of device
   virtual bool CanVSync() { return true; }
   virtual bool CanInsertBlackFrame() { return false; }
   virtual void Activate(bool on) {};

   void Lock() { mutex_.Acquire(); }
   void Unlock() { mutex_.Release(); }

   virtual void SetFrame(int frame_index);
   virtual void Draw();
   virtual void ClearBuffer(int frame_index);

protected:
   CTimer* timer_;
   CSpinLock   mutex_;

   DISPMANX_RESOURCE_HANDLE_T main_resource_[FRAME_BUFFER_SIZE];
   long unsigned int main_ptr_;

   DISPMANX_RESOURCE_HANDLE_T back_resource_;
   long unsigned int back_ptr_;

   DISPMANX_RESOURCE_HANDLE_T menu_resource_;
   long unsigned int menu_ptr_;

   DISPMANX_ELEMENT_HANDLE_T element_;
};