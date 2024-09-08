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
#include "Window.h"
#include "MenuItemWindows.h"
#include "CheckMenuItemWindows.h"
#include "ScrollWindows.h"

class MenuWindows : public Window
{
public:
   MenuWindows(BasicFrame* display);
   virtual ~MenuWindows();

   virtual void Create(Window* parent, int x, int y, unsigned int width, unsigned int height);
   virtual MenuItemWindows* GetMenuItem(unsigned int index);
   virtual void AddMenuItem(MenuItemWindows*);

   virtual MenuItemWindows* AddMenuItem(const char* label, int x, int y, int w, int h, IAction* action = nullptr);
   virtual void AddCheckMenuItem(const char* label, bool* value, int x, int y, int w, int h, IAction* action = nullptr);

   virtual void RedrawWindow();
   virtual IAction::ActionReturn HandleEvent(IEvent::Event event);
   virtual void SetFocus(unsigned int index = 0);

protected:
   void ComputeScroller();

   int current_focus_;
   ScrollWindows scroll_window_;
   std::vector<MenuItemWindows*> list_item_;

};

