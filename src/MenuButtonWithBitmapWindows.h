#pragma once

//
#include <string>

#include "BasicFrame.h"
#include "MenuItemWindows.h"
#include "Button.h"


class MenuButtonWithBitmapWindows : public MenuItemWindows
{
public:
   MenuButtonWithBitmapWindows (BasicFrame* display);
   virtual ~MenuButtonWithBitmapWindows();

   virtual void Create(const char* label, const char* description, SimpleBitmap* SimpleBitmap, Window* parent, int x, int y, unsigned int width, unsigned int height,
      int bmp_x, int bmp_y, unsigned int bmp_width, unsigned int bmp_height);

   virtual void RedrawWindow();
   virtual void RedrawChildren();

   virtual void SetFocus();
   virtual void RemoveFocus();


protected:
   Button button_;
   std::string label_;
   std::string description_;

   // inner button
   int button_x_;
   int button_y_;
   int button_w_;
   int button_h_;
};
