//
#include "EmulationFrame.h"

EmulationFrame::EmulationFrame() : 
   BasicFrame()
{

}

EmulationFrame::~EmulationFrame()
{

}

void EmulationFrame::Init(int width, int height, int nb_buffers)
{
   CLogger::Get ()->Write("DIS", LogNotice, "EmulationFrame");
   BasicFrame::Init(width, height, nb_buffers );

   x_ = 147;
   y_ = 47;
   width_ = (768-147);
   height_ = (277-47);
}

void EmulationFrame::Draw ()
{
   CLogger::Get ()->Write("DIS", LogNotice, "Draw EmulationFrame");
}

void EmulationFrame::Refresh ()
{
   BasicFrame::Refresh();
}