#pragma once

#include "Window.h"
#include "SimpleBitmap.h"

class Button : public Window
{
public:
   Button(BasicFrame* display);
   virtual ~Button();

   virtual void InitButton(SimpleBitmap* bmp, unsigned int x, unsigned int y);
   virtual void RedrawWindow();

private:
   // bitmap associé
   SimpleBitmap* bitmap_;
};
