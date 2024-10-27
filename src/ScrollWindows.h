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
#include "Window.h"


class ScrollWindows : public Window
{
public:
   ScrollWindows(BasicFrame* display);
   virtual ~ScrollWindows();

   virtual void RedrawChildren();
   virtual void WindowsToDisplay(int& x, int& y);
   virtual void Scroll(int offset_x, int offset_y);

protected:
   int scroll_offset_x_;
   int scroll_offset_y_;

};
