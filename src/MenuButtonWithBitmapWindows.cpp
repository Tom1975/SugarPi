//
#include <memory.h>
#include <math.h>
#include <stdlib.h>

#include "MenuButtonWithBitmapWindows.h"

////////////////////////////////////////////////////////////////////////////////////
MenuButtonWithBitmapWindows::MenuButtonWithBitmapWindows(BasicFrame* display) : MenuItemWindows(display), button_(display)
{

}

MenuButtonWithBitmapWindows::~MenuButtonWithBitmapWindows()
{

}

void MenuButtonWithBitmapWindows::Create(SimpleBitmap* bmp, Window* parent, int x, int y, unsigned int width, unsigned int height,
   int bmp_x, int bmp_y, unsigned int bmp_width, unsigned int bmp_height)
{
   MenuItemWindows::Create("", parent, x, y, width, height);
   button_.Create(parent, bmp_x, bmp_y, bmp_width, bmp_height);
   button_.InitButton(bmp, 1, 1);
}

void MenuButtonWithBitmapWindows::RedrawWindow()
{
   // Draw button
   int color = (focus_ == this) ? 0xFFFF0000 : 0xFF000000;

   int x = 0, y = 0;
   Window::WindowsToDisplay(x, y);

   int* line = display_->GetBuffer(y) + x;
   for (int ix = 0; ix < width_; ix++)
   {
      *line++ = color;
   }

   for (int iy = y + 1; iy < y + width_ - 2; iy++)
   {
      line = display_->GetBuffer(iy) + x;
      line[0] = color;
      line[width_-1] = color;
   }
   line = display_->GetBuffer(y + height_ - 1) + x;
   for (int ix = 0; ix < width_; ix++)
   {
      *line++ = color;
   }

   if (focus_ == this)
      button_.RedrawWindow();
   
}

