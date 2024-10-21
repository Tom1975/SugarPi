//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "BitmapWindows.h"

#ifdef RASPPI
#include "BasicFrame.h"
#else
#include "DisplayPiDesktop.h"

#define PROFILE

#ifdef PROFILE
#include <profileapi.h>
#define START_CHRONO  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);;QueryPerformanceCounter ((LARGE_INTEGER*)&s1);
#define STOP_CHRONO   QueryPerformanceCounter ((LARGE_INTEGER*)&s2);t=(DWORD)(((s2 - s1) * 1000000) / freq);
#define PROF_DISPLAY sprintf(s, "Duree displays Frame: %d us\n", t);OutputDebugString (s);
static __int64 s1, s2, freq;
static DWORD t;
static char s[1024];
#endif

#endif

#ifdef  __circle__
   #define PATH_FONT "SD:/FONTS/Facile_Sans.ttf"
#else
   #define PATH_FONT ".\\FONTS\\Facile_Sans.ttf" 
#endif

////////////////////////////////////////////////////////////////////////////////////

BitmapWindows::BitmapWindows(BasicFrame* display) : Window(display)
{

}

BitmapWindows::~BitmapWindows()
{

}

void BitmapWindows::Create(Window* parent, int x, int y, PiBitmap* bmp)
{
   bmp_ = bmp;
   bmp_->GetSize(width_, height_);
   Window::Create(parent, x, y, width_, height_);
}

void BitmapWindows::RedrawWindow()
{
   //CLogger::Get()->Write("BitmapWindows", LogNotice, "RedrawWindow");
   static float offset;
   for (int i = 0; i < height_; i++)
   {
      int* line = display_->GetBuffer(i + y_);
      bmp_->DrawLogo(i, &line[x_  /* + (int)(sinf(offset) * 10)*/]);
      offset += 0.002f;
   }
   //CLogger::Get()->Write("Window", LogNotice, "RedrawWindow end");

}

