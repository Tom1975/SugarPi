#pragma once

#include <circle/screen.h>
#include <circle/logger.h>
#include <circle/bcmframebuffer.h>

#include "DisplayPi.h"

class DisplayPiImp : public DisplayPi
{
public:
   DisplayPiImp(CLogger* logger, CTimer* timer);
   virtual ~DisplayPiImp();

   bool ListEDID();
   
   bool Initialization();
   bool InitInterrupt(CInterruptSystem* interrupt);

   static void InterruptStub (void *pParam);

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

   CBcmFrameBuffer* GetFrameBuffer() {
      return frame_buffer_;   }

   void Lock() { mutex_.Acquire(); }
   void Unlock() { mutex_.Release(); }

   virtual void SetFrame(int frame_index);
   virtual void Draw();
   virtual void ClearBuffer(int frame_index);

   virtual void SetWindowsConfiguration(WindowStructure* window_structure, int nb_win);
   void UpdateWindowsConfiguration();

   void SetSetup(WindowsType setup);
   void GetFrameBuffer(int w, int h, int p, int color);

protected:
   void InterruptionHandler();

   CTimer* timer_;
   CBcmFrameBuffer*  frame_buffer_;

   CSpinLock   mutex_;

   // HVS 
   WindowStructure * current_structure_;
   int nb_windows_;

   unsigned int current_buffer_;
   unsigned int animation_step_;


      typedef struct {
      hvs_pixel_format format;            // format of the pixels in the plane
      hvs_pixel_order pixel_order;        // order of the components in each pixel
      unsigned short start_x;                   // x position of the left of the plane
      unsigned short start_y;                   // y position of the top of the plane
      unsigned short height;                    // height of the plane, in pixels
      unsigned short width;                     // width of the plane, in pixels
      unsigned short pitch;                     // number of bytes between the start of each scanline
      void* framebuffer;                  // pointer to the pixels in memory
   } hvs_plane;

   hvs_plane *plane_;


   static void write_plane(unsigned short* offset, hvs_plane plane);
   static void write_display_list(hvs_plane planes[], unsigned char count);

};