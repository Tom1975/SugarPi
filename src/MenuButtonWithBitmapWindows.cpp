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

void MenuButtonWithBitmapWindows::SetFocus()
{
   Window::SetFocus();
   button_.ShowWindow(true);
}

void MenuButtonWithBitmapWindows::RemoveFocus()
{
   Window::RemoveFocus();
   button_.ShowWindow(false);
}


void MenuButtonWithBitmapWindows::Create(const char* label, const char* description, SimpleBitmap* bmp, Window* parent, int x, int y, unsigned int width, unsigned int height,
   int bmp_x, int bmp_y, unsigned int bmp_width, unsigned int bmp_height)
{
   label_ = label;
   description_ = description;
   MenuItemWindows::Create(label_.c_str(), parent, x, y, width, height);
   button_x_ = bmp_x - x_;
   button_y_ = bmp_y - y_;
   button_w_ = bmp_width;
   button_h_ = bmp_height;

   // Center bitmap.
   int bmp_w, bmp_h;
   bmp->GetSize(bmp_w, bmp_h);
   int x_button = ((button_w_ - 2) - bmp_w) / 2 + button_x_ + 1;
   int y_button = (200 - bmp_h) / 2 + button_y_ + 1;
   button_.Create(this, x_button , y_button, bmp_w, bmp_h);
   button_.ShowWindow(false);
   button_.InitButton(bmp, x_button, y_button);
}

void MenuButtonWithBitmapWindows::RedrawWindow()
{
   // Draw button

   MenuItemWindows::RedrawWindow();

   if (GetFocus() == this)
   {
      // Draw a surrounding lines 
      DrawPoly(0xFF000000, {
         {0, 0}, {width_, 0},                            // line at top of menu
         {width_, button_y_},                            // go up to the top of bitmap
         {button_x_+ button_w_, button_y_},              // Upper line
         {button_x_ + button_w_, button_y_ + button_h_}, // right line, along the bitmap
         {button_x_ , button_y_ + button_h_},            // bottom line, juste bellow the bitmap
         {button_x_ , height_},                          // Left line, from bottom to bottom of line
         {0, height_},                                   // bottom line, under the menu
         {0, 0}                                          // Left line
         });

      // Draw bitmap
      button_.RedrawWindow();

      // Draw informations
      int y = button_y_ + 240;
      int x = button_x_ + 30;

      Window::WindowsToDisplay(x, y);

      SFT* oldfnt = display_->SelectFont(fnt_italic_);

      display_->SelectColor(0xFF000000);
      
      std::string current = description_.c_str();
      std::string to_display;
      for (auto& it : current)
      {
         if (it == '\n')
         {
            display_->WriteText(to_display.c_str(), x, y);
            y += 45;
            to_display.clear();
         }
         else
         {
            to_display += it;
         }
      }
      display_->WriteText(to_display.c_str(), x, y);

      display_->SelectFont(oldfnt);

      
   }   
}

void MenuButtonWithBitmapWindows::RedrawChildren()
{
   // Nothing to do !
}

