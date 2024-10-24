/* GIMP RGBA C-Source image dump (coolspot.c) */

#include "Button.h"


Button::Button(BasicFrame* display) : Window(display), bitmap_(nullptr)
{

}

Button::~Button()
{
}

void Button::InitButton(SimpleBitmap* bmp, unsigned int x, unsigned int y)
{
   x_ = x;
   y_ = y;
   bitmap_ = bmp;
}


void Button::RedrawWindow()
{
   // Draw background
   // todo
   // Draw inner bitmap
   DrawBitmap(bitmap_, 0, 0);
}