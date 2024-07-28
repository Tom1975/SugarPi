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

void MenuFrame::Draw ()
{
   CLogger::Get ()->Write("DIS", LogNotice, "Draw MenuFrame");
}

void MenuFrame::Refresh ()
{
   BasicFrame::Refresh();
}