#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#include <circle/string.h>
#else
#include "CLogger.h"
#include "CString.h"
#endif

#include "BasicFrame.h"
#include "Window.h"

#include "res/SugarboxLogo.h"


class MainMenuWindows : public Window
{
public:
   MainMenuWindows(BasicFrame* display);
   virtual ~MainMenuWindows();

   void ResetMenu();
   MenuWindows* GetMenu() { return menu_; };
   virtual void Clear();

protected:
   BitmapWindows* logo_;
   MenuWindows* menu_;
   SugarboxLogo* bitmap_;

};
