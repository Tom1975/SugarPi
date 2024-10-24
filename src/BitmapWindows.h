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

class BitmapWindows : public Window
{
public:
   BitmapWindows(BasicFrame* display);
   virtual ~BitmapWindows();

   virtual void Create(Window* parent, int x, int y, PiBitmap* bmp);
   virtual void RedrawWindow();

protected:
   PiBitmap* bmp_;
   int width_, height_;
};
