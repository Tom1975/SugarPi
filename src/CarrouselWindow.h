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

   virtual void Create(Window* parent, int x, int y, unsigned int width, unsigned int height);

   virtual void Clear();

protected:
   class GameDescription
   {
   public:
      std::string name_;
      std::string description_;

      std::string sna_path_;
   };

   BitmapWindows* logo_;
   SugarboxLogo* bitmap_;

};
