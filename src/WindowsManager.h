#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif


typedef enum {
   /* 8bpp */
   HVS_PIXEL_FORMAT_RGB332 = 0,

   /* 16bpp */
   HVS_PIXEL_FORMAT_RGBA4444 = 1,
   HVS_PIXEL_FORMAT_RGB555 = 2,
   HVS_PIXEL_FORMAT_RGBA5551 = 3,
   HVS_PIXEL_FORMAT_RGB565 = 4,

   /* 24bpp */
   HVS_PIXEL_FORMAT_RGB888 = 5,
   HVS_PIXEL_FORMAT_RGBA6666 = 6,

   /* 32bpp */
   HVS_PIXEL_FORMAT_RGBA8888 = 7,
} hvs_pixel_format;

typedef enum {
   HVS_PIXEL_ORDER_RGBA = 0,
   HVS_PIXEL_ORDER_BGRA = 1,
   HVS_PIXEL_ORDER_ARGB = 2,
   HVS_PIXEL_ORDER_ABGR = 3
} hvs_pixel_order;

class IAnimate;
class WindowStructure
{
public:
   // Pixel format
   hvs_pixel_format format_;
   // order of component in each pixels
   hvs_pixel_order order_;
   // pos x
   unsigned short x_;
   // pos y
   unsigned short y_;
   // height
   unsigned short h_;
   // width
   unsigned short w_;
   // pitch
   unsigned short pitch_;
   // framebuffer numbers (multiple buffering)
   unsigned char nb_buffers_;
   // framebuffer list
   unsigned char** buffer_;

   // Animation (if any)
   IAnimate* animation_;

   // Main buffer 
   bool main_;

};
class IAnimate
{
public:
   virtual void MoveAround(WindowStructure) = 0;
};


class WindowsManager
{
public:

   enum WindowsType {
      Start = 0,
      Menu = 1,
      Main = 2,

      EndOfWindowsType
   } ;

   WindowsManager(CLogger* logger);
   virtual ~WindowsManager();

   virtual void SetSetup(WindowsType setup) = 0;
   virtual void GetFrameBuffer(int w, int h, int p, int color) = 0;

   
private:
   CLogger* logger_;

   // Configuration
   WindowStructure* windows_structures_[EndOfWindowsType];
   int window_structures_size_[EndOfWindowsType];
};