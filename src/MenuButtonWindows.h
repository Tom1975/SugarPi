#pragma once

//
#include "BasicFrame.h"
#include "MenuItemWindows.h"
#include "Button.h"


class MenuButtonWindows : public MenuItemWindows
{
public:
   MenuButtonWindows(BasicFrame* display);
   virtual ~MenuButtonWindows();

   virtual void Create(SimpleBitmap* SimpleBitmap, Window* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void RedrawWindow();

protected:
   Button button_;
};
