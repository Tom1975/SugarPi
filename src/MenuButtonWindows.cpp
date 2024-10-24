//
#include <memory.h>
#include <math.h>
#include <stdlib.h>

#include "MenuButtonWindows.h"

////////////////////////////////////////////////////////////////////////////////////
MenuButtonWindows::MenuButtonWindows(BasicFrame* display) : MenuItemWindows(display), button_(display)
{

}

MenuButtonWindows::~MenuButtonWindows()
{

}

void MenuButtonWindows::Create(SimpleBitmap* bmp, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   MenuItemWindows::Create("", parent, x, y, width, height);
   button_.Create(this, 1, 1, width-2, height-2);
   button_.InitButton(bmp, 1, 1);
}

void MenuButtonWindows::RedrawWindow()
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


   button_.RedrawWindow();
   
}

