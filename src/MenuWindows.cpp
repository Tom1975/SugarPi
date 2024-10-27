//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "MenuWindows.h"

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

#define INTERLINE_SPACE 40

////////////////////////////////////////////////////////////////////////////////////
MenuWindows::MenuWindows(BasicFrame* display) : Window(display), current_focus_(-1), scroll_window_(display)
{

}

MenuWindows::~MenuWindows()
{
   // Clear items
   for (auto& it : list_item_)
   {
      delete it;
   }
   list_item_.clear();
}

void MenuWindows::Create(Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   Window::Create(parent, x, y, width, height);

   // Add a simple windows that will be used for scrolling
   scroll_window_.Create(this, 0, 0, width, height);
}

MenuItemWindows* MenuWindows::GetMenuItem(unsigned int index)
{
   return (index < list_item_.size()) ? list_item_[index] : nullptr;
}

void MenuWindows::AddMenuItem(MenuItemWindows* item)
{
   list_item_.push_back(item);

   if (current_focus_ == -1)
      current_focus_ = 0;

   ComputeScroller();
}

MenuItemWindows* MenuWindows::AddMenuItem(const char* label, int x, int y, int w, int h, IAction* action)
{
   //CLogger::Get ()->Write("Menu", LogNotice, "add menu : %s ", label);

   // Add item to menu
   MenuItemWindows* item = new MenuItemWindows(display_);
   item->Create(label, &scroll_window_, x, y, w, h);
   item->SetAction(action);

   AddMenuItem(item);
   return item;
}

void MenuWindows::AddCheckMenuItem(const char* label, bool* value, int x, int y, int w, int h, IAction* action)
{
   // Add item to menu
   //CLogger::Get ()->Write("Menu", LogNotice, "add menucheck : %s ", label);
   CheckMenuItemWindows* item = new CheckMenuItemWindows(display_);
   item->Create(label, value, &scroll_window_, x, y, w, h);
   item->SetAction(action);

   list_item_.push_back(item);
   Redraw(true);

   if (current_focus_ == -1)
      current_focus_ = 0;

   ComputeScroller();
}


void MenuWindows::RedrawWindow()
{
   int x = 450;
   int y = 47;
   WindowsToDisplay(x, y);
}

void MenuWindows::ComputeScroller()
{
   // check current focus, depending on windows size
   int distant_to_top = current_focus_ * INTERLINE_SPACE;
   int distant_to_bottom = (static_cast<int>(list_item_.size()) - (current_focus_ + 1)) * INTERLINE_SPACE;

   int win_h = distant_to_top - height_ / 2;
   int win_h2b = distant_to_bottom - height_ / 2;

   // rules : 
   int scroll_y;
   if (win_h < 0)
   {
      scroll_y = 0;
   }
   else
   {
      scroll_y = win_h;
   }
   scroll_window_.Scroll(0, scroll_y);
}

IAction::ActionReturn MenuWindows::HandleEvent(IEvent::Event event)
{
   // do something
   switch (event)
   {
   case IEvent::Event::DOWN:
      // Go down in the menu
      if (current_focus_ < static_cast<int>(list_item_.size()) - 1)
      {
         current_focus_++;
         list_item_.at(current_focus_)->SetFocus();
         ComputeScroller();
         //Redraw();
         Invalidate();
      }
      break;
   case IEvent::Event::UP:
      // Go up in the menu
      if (current_focus_ > 0)
      {
         current_focus_--;
         list_item_.at(current_focus_)->SetFocus();
         ComputeScroller();
         //Redraw();
         Invalidate();
      }
      break;
   case IEvent::Event::BACK:
      return IAction::ActionReturn::Action_Back;
      break;
   default:
      break;
   }

   return IAction::ActionReturn::Action_None;
}

void MenuWindows::SetFocus(unsigned int index)
{
   // Set focus to first item
   if (list_item_.size() > index)
   {
      current_focus_ = index;
      list_item_.at(index)->SetFocus();
      ComputeScroller();
   }

}
