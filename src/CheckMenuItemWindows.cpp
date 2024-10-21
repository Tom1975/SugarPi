//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "CheckMenuItemWindows.h"

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


////////////////////////////////////////////////////////////////////////////////////
CheckMenuItemWindows::CheckMenuItemWindows(BasicFrame* display) : MenuItemWindows(display), value_(nullptr)
{

}
CheckMenuItemWindows::~CheckMenuItemWindows()
{

}

void CheckMenuItemWindows::Create(const char* label, bool* value, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   MenuItemWindows::Create(label, parent, x, y, width, height);
   value_ = value;
}

void CheckMenuItemWindows::RedrawWindow()
{
   int x = 15;
   int y = 0;
   WindowsToDisplay(x, y);

   // Focus ?
   if (focus_ == this)
   {
      // draw it 
      display_->WriteText("*", x - 15, y);
   }
   // Draw the check box
   display_->WriteText((*value_) ? "[X]" : "[ ]", x, y);
   display_->WriteText(label_, x + 30, y);
}

IAction::ActionReturn CheckMenuItemWindows::HandleEvent(IEvent::Event event)
{
   //
   switch (event)
   {
   case IEvent::Event::SELECT:
      // Action !
      (*value_) = (*value_) ? false : true;
      if (action_ != nullptr)
      {
         return action_->DoAction();
      }
      break;
   default:
      if (parent_ != nullptr)
         return parent_->HandleEvent(event);
   }

   return IAction::ActionReturn::Action_None;
}
