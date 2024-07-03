//
#include "BackFrame.h"
#include <math.h> 

#define BACK_MOVE 32

BackFrame::BackFrame() : 
   BasicFrame(),
   value_(0.0f)
{

}

BackFrame::~BackFrame()
{

}

void BackFrame::Init(int width, int height, int nb_buffers)
{
   CLogger::Get ()->Write("DIS", LogNotice, "BackFrame");
   BasicFrame::Init(width + BACK_MOVE, height + BACK_MOVE, nb_buffers);
   width_ = width;
   height_ = height;
}

void BackFrame::Draw ()
{
   CLogger::Get ()->Write("DIS", LogNotice, "Draw BackFrame");
   for (int i = 0; i < internal_height_; i++)
   {
      for (int j = 0; j < internal_width_; j++)
      {
         if ( (( (j & 0x3F) < 0x20) && ((i&0x3F) < 0x20)  )
         ||(( (j & 0x3F) >= 0x20) && ((i&0x3F) >= 0x20)) )
         {
            *(int*)(&display_frame_buffer_[current_buffer_][i*back_pitch_ + j * 4]) = 0xFFCCCCCC;
         }
         else
         {
            *(int*)(&display_frame_buffer_[current_buffer_][i*back_pitch_ + j * 4]) = 0xFFDDDDDD;
         }
      }
   }
   buffer_has_changed_ = true;
   CLogger::Get ()->Write("DIS", LogNotice, "Draw : nb_buffers_ = %i - back_pitch_ = %i - height_ = %i, internal_width_= %i, internal_height_ = %i",
    nb_buffers_, back_pitch_, height_, internal_width_, internal_height_);
}

void BackFrame::Refresh ()
{
   x_ = (BACK_MOVE + sinf(value_)*BACK_MOVE);
   y_ = (BACK_MOVE + cos(value_*2)*BACK_MOVE);

   value_ += 0.04;

   current_change_ = CHANGED_SRC_RECT;
}