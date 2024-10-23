#pragma once

#include "Window.h"
#include "SimpleBitmap.h"

class Button : public Window
{
public:
   Button(BasicFrame* display);
   virtual ~Button();

   virtual void InitButton(SimpleBitmap*bmp, unsigned int x, unsigned int y);
   virtual void InitButton( unsigned int x, unsigned int y, unsigned int w, unsigned int h, const char* SimpleBitmap);
   virtual void RedrawWindow();

private:
   // bitmap associé
   SimpleBitmap* bitmap_;
   /*int x_;
   int y_;
   int w_;
   int h_;*/
};
