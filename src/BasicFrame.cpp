//
#include <math.h> 
#include <memory.h>

#include "BasicFrame.h"
#include "Morphings.h"

#ifndef ALIGN_UP
#define ALIGN_UP(x,y)  ((x + (y)-1) & ~((y)-1))
#endif

BasicFrame::BasicFrame() : 
   internal_width_(0),
   internal_height_(0),
   back_pitch_(0),
   x_(0),
   y_(0),
   width_(0),
   height_(0),
   nb_buffers_(0),
   current_buffer_(0),
   display_frame_buffer_(nullptr),
   frame_used_(nullptr),
   frame_queue_(nullptr),
   nb_frame_in_queue_(0),
   current_change_(0),
   buffer_has_changed_(false),
   current_morph(nullptr)
{

}

BasicFrame::~BasicFrame()
{

}

void BasicFrame::Init(int width, int height, int nb_buffers)
{
   CLogger::Get ()->Write("BasicFrame", LogNotice, "Init width : %i, height : %i; nb_buffers : %i", width, height, nb_buffers);
   internal_width_ = width_ = width;
   internal_height_ = height_ = height;
   x_ = y_ = 0;

   current_change_ = CHANGED_DEST_RECT|CHANGED_SRC_RECT;
   
   back_pitch_ = ALIGN_UP(internal_width_*4, 32);
   nb_buffers_ = nb_buffers;
   display_frame_buffer_ = new unsigned char*[nb_buffers_];
   frame_used_ = new FrameState[nb_buffers_];
   frame_queue_ = new unsigned int[nb_buffers_];

   for (int i = 0; i < nb_buffers_; i++)
   {
      display_frame_buffer_[i] = new unsigned char [back_pitch_ * internal_height_];
      frame_used_[i] = FR_FREE;
      frame_queue_[i] = 0;
   }
   // Set current buffer
   current_buffer_ = 0;
   nb_frame_in_queue_ = 0;
   frame_used_[current_buffer_] = FR_USED;
   
   Draw();
}

void BasicFrame::Reset(int buffer)
{
   if ( buffer == -1)
   {
      for (int i = 0; i < nb_buffers_; i++)
      {
         memset( display_frame_buffer_[i], 0, back_pitch_ * internal_height_ * 4);
      }
   }
   else
   {
      if ( buffer < nb_buffers_)
         memset( display_frame_buffer_[buffer], 0, back_pitch_ * internal_height_ * 4);
   }
}

unsigned int BasicFrame::AttributesHasChanged()
{
   return current_change_;
}

unsigned char * BasicFrame::GetBuffer()
{
   return display_frame_buffer_[current_buffer_];
}

int* BasicFrame::GetBuffer(unsigned int y)
{
   return (int*) &(display_frame_buffer_[current_buffer_][y * back_pitch_]);
}

void BasicFrame::FrameIsReady()
{
   buffer_has_changed_ = true;

   // Add current buffer to display list
   frame_used_[current_buffer_] = FR_READY;
   frame_queue_[nb_frame_in_queue_++] = current_buffer_;

   //CLogger::Get ()->Write("BasicFrame", LogNotice, "FrameIsReady - current_buffer_ : %i, nb_frame_in_queue_ = %i", current_buffer_, nb_frame_in_queue_);
   // Set a new buffer to be displayed
   bool found = false;
   for (int i = 0; i < nb_buffers_ && !found; i++)
   {
      if (frame_used_[i] == FR_FREE)
      {
         //CLogger::Get ()->Write("BasicFrame", LogNotice, "FR_FREE - %i", i);
         frame_used_[i] = FR_USED;
         current_buffer_ = i;
         found = true;
         break;
      }
   }   
}

unsigned char * BasicFrame::GetReadyBuffer()
{
   if (nb_frame_in_queue_ > 0)
   {
      // A frame is ready ? return it.
      //CLogger::Get ()->Write("BasicFrame", LogNotice, "GetReadyBuffer - %i", frame_queue_[0]);
      return display_frame_buffer_[frame_queue_[0]];
   }
   else
   {
      // No frame ready ? use the current one !
      // TODO : may be improved, using the last-displayed-frame
      return display_frame_buffer_[current_buffer_];
   }
}

void BasicFrame::FrameIsDisplayed()
{
   buffer_has_changed_ = false;
   int frame_index = -1;

   if (nb_frame_in_queue_ > 0)
   {
      frame_index = frame_queue_[0];
      nb_frame_in_queue_--;

      memmove(frame_queue_, &frame_queue_[1], nb_frame_in_queue_ * sizeof(unsigned int));
      
      frame_used_[frame_index] = FR_FREE;
   }
   else
   {
       // we displayed a random frame, dont change anything
   }
}

void BasicFrame::Maximize()
{
   // Minimize : Set the function to minimize
   current_morph = new Morphings(this,
      0,
      0,
      display_width_full_,
      display_height_full_,
      25
   );
}

void BasicFrame::Minimize()
{
   // Minimize : Set the function to minimize
   current_morph = new Morphings(this, 
      display_width_full_*0.70,
      display_height_full_ * 0.70,
      display_width_full_ * 0.2,
      display_height_full_ * 0.2,
      25
      );
}

void BasicFrame::Refresh()
{
   CLogger::Get ()->Write("BasicFrame", LogNotice, "BasicFrame::Refresh");
   // Any moving function is to be updated ?
   if (current_morph != nullptr)
   {
      if (current_morph->Refresh())
      {
         delete current_morph;
         current_morph = nullptr;
      }
   }
   else
   {
      current_change_ = 0;
   }
}