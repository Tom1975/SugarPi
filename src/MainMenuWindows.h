#pragma once

#include "Window.h"
#include "BitmapWindows.h"
#include "MenuWindows.h"
#include "SugarboxLogo.h"


class MainMenuWindows : public Window
{
public:
   MainMenuWindows(BasicFrame* display);
   virtual ~MainMenuWindows();

   void ResetMenu(unsigned int index = 0);
   MenuWindows* GetMenu() { return menu_; };
   virtual void Clear();

protected:
   BitmapWindows* logo_;
   MenuWindows* menu_;
   SugarboxLogo* bitmap_;

};
