#pragma once


#include <circle/screen.h>
#include <circle/logger.h>
#include <circle/bcmframebuffer.h>

#include "CPCCore/CPCCoreEmu/simple_vector.hpp"

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
   void CopyMemoryToRessources();

   CTimer* timer_;
   CSpinLock   mutex_;

   // Dispmanx elements
   DISPMANX_DISPLAY_HANDLE_T  display_;
   DISPMANX_MODEINFO_T        info_;

   class DispmanxWindow
   {
   public:
         BasicFrame* frame_;
         DISPMANX_RESOURCE_HANDLE_T resource_;
         long unsigned int ptr_;
         DISPMANX_ELEMENT_HANDLE_T element_;     
         VC_IMAGE_TYPE_T type_of_image_; 
         unsigned int priority_;
         VC_DISPMANX_ALPHA_T alpha_;
   };

   std::vector<DispmanxWindow> windows_list_;
   
   DispmanxWindow emu_wnd_;
   DispmanxWindow menu_wnd_;
   DispmanxWindow back_wnd_;

   // Display informations
   typedef struct
   {
      DISPMANX_DISPLAY_HANDLE_T   display;
      DISPMANX_MODEINFO_T         info;
      void                       *image;
      DISPMANX_UPDATE_HANDLE_T    update;
      DISPMANX_RESOURCE_HANDLE_T  resource;
      DISPMANX_ELEMENT_HANDLE_T   element;
      uint32_t                    vc_image_ptr;

   } RECT_VARS_T;

};