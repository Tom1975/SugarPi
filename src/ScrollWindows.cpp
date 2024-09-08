//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "ScrollWindows.h"

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
ScrollWindows::ScrollWindows(BasicFrame* display) : Window(display), scroll_offset_x_(0), scroll_offset_y_(0)
{

}

ScrollWindows::~ScrollWindows()
{

}


void ScrollWindows::RedrawChildren()
{
   WindowsQueue** current_queue = &windows_children_;
   while (*current_queue != nullptr)
   {
      if ((*current_queue)->wnd_->GetX() - scroll_offset_x_ >= 0 && (*current_queue)->wnd_->GetY() - scroll_offset_y_ >= 0
         && (*current_queue)->wnd_->GetWidth() + (*current_queue)->wnd_->GetX() - scroll_offset_x_ <= width_
         && (*current_queue)->wnd_->GetHeight() + (*current_queue)->wnd_->GetY() - scroll_offset_y_ <= height_)
      {
         (*current_queue)->wnd_->RedrawWindow();
         (*current_queue)->wnd_->RedrawChildren();
      }

      current_queue = &((*current_queue)->next_);
   }
}

void ScrollWindows::WindowsToDisplay(int& x, int& y)
{
   x += x_ - scroll_offset_x_;
   y += y_ - scroll_offset_y_;
   if (parent_ != nullptr)
   {
      parent_->WindowsToDisplay(x, y);
   }
}

void ScrollWindows::Scroll(int offset_x, int offset_y)
{
   scroll_offset_x_ = offset_x;
   scroll_offset_y_ = offset_y;
}
