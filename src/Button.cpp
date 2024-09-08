/* GIMP RGBA C-Source image dump (coolspot.c) */

#include "Button.h"

#include "files.h"
#include <CPCCore/CPCCoreEmu/stdafx.h>


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

void Button::InitButton(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const char* bitmap)
{
   x_ = x;
   y_ = y;
   w_ = w;
   h_ = h;
   if (bitmap_ != nullptr)
      delete bitmap_;
   bitmap_ = new SimpleBitmap(bitmap);
}

void Button::RedrawWindow()
{
   // Draw background of button
   int x = x_;
   int y = y_;
   Window::WindowsToDisplay(x, y);
   // Draw inner bitmap
   DrawBitmap(bitmap_, x, y);
}