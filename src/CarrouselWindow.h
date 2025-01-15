#pragma once

#include "Window.h"
#include "BitmapWindows.h"
#include "MenuWindows.h"
#include "SugarboxLogo.h"


class CarrouselWindow : public Window
{
public:
   CarrouselWindow(BasicFrame* display);
   virtual ~CarrouselWindow();

   virtual void Clear();

protected:
   BitmapWindows* logo_;
   MenuWindows* menu_;
   SugarboxLogo* bitmap_;

};
