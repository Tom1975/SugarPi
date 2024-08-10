#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif

#include "Morphing.h"
#include "schrift.h"

#define CHANGED_NONE      0
#define CHANGED_LAYER     0x01
#define CHANGED_OPACITY   0x02
#define CHANGED_DEST_RECT 0x04
#define CHANGED_SRC_RECT  0x08

class BasicFrame
{
public:
   BasicFrame();
   virtual ~BasicFrame();

   /////////// Internal setup
   virtual void Init(int width, int height, int nb_buffers = 1);
   virtual void Reset(int buffer = -1);

   /////////// Internal size
   virtual int GetFullWidth(){return internal_width_;}
   virtual int GetFullHeight(){return internal_height_;}

   /////////// Display
   // Internal bitmap : offset of the top/left corner to display, in pixel
   virtual int GetOffsetX(){return x_;};
   virtual int GetOffsetY(){return y_;};

   // Internal bitmap : width/height to display, in pixel
   virtual int GetWidth(){return width_;}
   virtual int GetHeight(){return height_;}

   // internal bitmap, pitch
   virtual int GetPitch(){return back_pitch_;}

   /////////// Display setup
   virtual void SetDisplay(int x, int y){display_x_ = x;display_y_ = y;}
   virtual void SetDisplaySize(int width, int height){ display_width_full_ = display_width_ = width; display_height_full_ = display_height_ = height;}
   virtual int GetDisplayX(){return display_x_;}
   virtual int GetDisplayY(){return display_y_;}
   virtual int GetDisplayWidth(){return display_width_;}
   virtual int GetDisplayHeight(){return display_height_;}

   ////////////////////////////////
   // Text functions
   SFT* SelectFont(SFT* fnt);
   int SelectColor(int color);
   void WriteText(const char* text, int x, int y);



   virtual void Draw () = 0;
   virtual void Refresh ();

   // Change window size
   virtual void Minimize();
   virtual void Maximize();

   // Get changed attributes since last call
   virtual unsigned int AttributesHasChanged();

   // Get current buffer to write
   virtual unsigned char * GetBuffer();
   virtual int* GetBuffer(unsigned int y);
   // Current buffer is ready
   virtual void FrameIsReady();

   // Get next buffer ready to be displayed
   virtual unsigned char * GetReadyBuffer();
   // Current displayed frame can be recycled
   virtual void FrameIsDisplayed();

   virtual bool HasFrameChanged(){return buffer_has_changed_;}

   virtual bool CanDrawNewFrame() {return nb_frame_in_queue_ < ((nb_buffers_ > 2 )?1:0);}

   friend class Morphings;

protected:

   // Memory internal bitmap
   int internal_width_;
   int internal_height_;
   int back_pitch_;

   // Extract to display from this ressource
   int x_;
   int y_;
   int width_;
   int height_;

   // display size
   int display_x_;
   int display_y_;
   int display_width_;
   int display_height_;
   int display_width_full_;
   int display_height_full_;

   // buffer handling   
   int nb_buffers_;
   int current_buffer_;
   int display_buffer_;
   unsigned char** display_frame_buffer_;
   
   typedef enum
   {
      FR_FREE,
      FR_USED,
      FR_READY
   } FrameState;
   volatile FrameState* frame_used_;
   unsigned int *frame_queue_;
   volatile unsigned int nb_frame_in_queue_;

   volatile unsigned int current_change_;
   volatile bool buffer_has_changed_;

   // Update function
   Morphing* current_morph;

   // Text context
   SFT * sft_;
   int text_color_;
};