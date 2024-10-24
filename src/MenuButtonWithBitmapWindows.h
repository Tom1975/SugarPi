#pragma once

//
#include "BasicFrame.h"
#include "MenuItemWindows.h"
#include "Button.h"


class MenuButtonWithBitmapWindows : public MenuItemWindows
{
public:
   MenuButtonWithBitmapWindows (BasicFrame* display);
   virtual ~MenuButtonWithBitmapWindows();

   virtual void Create(SimpleBitmap* SimpleBitmap, Window* parent, int x, int y, unsigned int width, unsigned int height,
      int bmp_x, int bmp_y, unsigned int bmp_width, unsigned int bmp_height);

   virtual void RedrawWindow();

   virtual void SetFocus();
   virtual void RemoveFocus();


protected:
   Button button_;
};
