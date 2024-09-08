#pragma once

//
#ifdef  __circle__
#include <circle/logger.h>
#include <circle/string.h>
#define WAIT(x) CTimer::Get ()->MsDelay(x)
#else
#include <string>
#include <chrono>
#include <thread>

#include "CLogger.h"
#include "CString.h"
#define WAIT(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));
#endif

#include <vector>
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
