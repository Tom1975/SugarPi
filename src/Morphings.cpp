//
#include "Morphings.h"

#include <math.h> 

#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4


////////////////////////////////////////////////////////////////////////
// Morphing functions
////////////////////////////////////////////////////////////////////////
Morphings::Morphings(BasicFrame* frame, int x, int y, int w, int h, int nbframe) : frame_(frame)
{
   // Initial values
   base_x = frame->display_x_;
   base_y = frame->display_y_;
   base_width = frame->display_width_;
   base_height = frame->display_height_;

   // Compute : quarter of the screen, at the bottom.
   final_x = x; 
   final_y = y; 

   final_width = w; 
   final_height = h;
  
   // Count 
   max_count_ = nbframe;
   count_ = 0;


}

Morphings::~Morphings()
{

}

bool Morphings::Refresh()
{
   float val = sinf( count_ / max_count_ * M_PI_2 );

   count_++;

   frame_->display_x_ = base_x + (final_x - base_x) * val;
   frame_->display_y_ = base_y + (final_y - base_y) * val;
   frame_->display_width_ = base_width + (final_width - base_width) * val;
   frame_->display_height_ = base_height + (final_height - base_height) * val;

   CLogger::Get ()->Write("Morphings", LogNotice, "Refresh - x:%i y:%i w:%i h:%i", 
      frame_->display_x_,
      frame_->display_y_,
      frame_->display_width_,
      frame_->display_height_
      );

   frame_->current_change_ = CHANGED_DEST_RECT;

   return (count_ == max_count_);
}



