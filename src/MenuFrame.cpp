//
#include <memory.h>

#include "MenuFrame.h"


MenuFrame::MenuFrame() : 
   BasicFrame()
{

}

MenuFrame::~MenuFrame()
{

}

void MenuFrame::Init(int width, int height, int nb_buffers)
{
   CLogger::Get ()->Write("DIS", LogNotice, "MenuFrame");
   BasicFrame::Init(width, height, nb_buffers );
   memset (display_frame_buffer_[current_buffer_], 0x80, internal_height_*back_pitch_);
   CLogger::Get ()->Write("DIS", LogNotice, "MenuFrame : pitch = %x ", back_pitch_);
}

unsigned int  MenuFrame::AttributesHasChanged()
{
#ifdef TRACE_ENABLED
   if ( current_change_)
   {
      CLogger::Get ()->Write("DIS", LogNotice, "MenuFrame::AttributesHasChanged");
      CLogger::Get ()->Write("DIS", LogNotice, "Values - src : %i, %i, %i, %i => %i, %i, %i, %i", GetOffsetX(), GetOffsetY(), GetWidth(), GetHeight(),
            GetDisplayX(), GetDisplayY(), GetDisplayWidth(), GetDisplayHeight() );
   }
#endif
   return BasicFrame::AttributesHasChanged();
}

void MenuFrame::Draw ()
{
   CLogger::Get ()->Write("DIS", LogNotice, "Draw MenuFrame");
}

void MenuFrame::Refresh ()
{
   BasicFrame::Refresh();
}