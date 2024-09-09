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

   int* line = display_->GetBuffer(y_);
   for (int x = 0; x < width_; x++)
   {
      *line++ = color;
   }

   for (int y = y_ + 1; y < y_ + width_ - 2; y++)
   {
      line = display_->GetBuffer(y);
      line[0] = color;
      line[width_-1] = color;
   }
   line = display_->GetBuffer(y_ + height_ - 1);
   for (int x = 0; x < width_; x++)
   {
      *line++ = color;
   }


   button_.Redraw();
   
}

