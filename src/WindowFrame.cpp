//
#include "WindowFrame.h"
#include <math.h> 

WindowFrame::WindowFrame() : 
   BasicFrame()
{

}

WindowFrame::~WindowFrame()
{

}

void WindowFrame::Init(int width, int height, int nb_buffers)
{
   CLogger::Get ()->Write("DIS", LogNotice, "WindowFrame");
   BasicFrame::Init(width , height , nb_buffers);
}

void WindowFrame::Draw ()
{
   CLogger::Get ()->Write("DIS", LogNotice, "Draw WindowFrame");
}

void WindowFrame::Refresh ()
{
}