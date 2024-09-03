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
#include "PiBitmap.h"

#include "schrift.h"

class IAction
{
public:
   enum ActionReturn
   {
      Action_None,
      Action_Update,
      Action_QuitMenu,
      Action_Back,
      Action_Shutdown,
   };
   virtual ActionReturn DoAction() = 0;
};

class IEvent
{
public:
   enum Event
   {
      NONE,
      SELECT,
      BACK,
      DOWN,
      UP,
      LEFT,
      RIGHT,
   };

   virtual Event GetEvent () = 0;
};

class Window
{
public:
   Window(BasicFrame* display);
   virtual ~Window();

   virtual IAction::ActionReturn DoScreen (IEvent* event_handler);

   virtual void Create(Window* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void AddChild(Window* child);

   virtual void WindowsToDisplay(int& x, int& y);
   virtual void Clear();
   virtual void ClearAll();
   virtual void Redraw (bool clear = true);
   virtual void RedrawWindow ();
   virtual void RedrawChildren ();

   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

   virtual void SetFocus ();
   virtual void RemoveFocus ();

   void Invalidate ();
   static Window* GetFocus() { return focus_;}
   static void SetFocus(Window* focus) { focus_ = focus;}
   int GetX(){return x_;}
   int GetY(){return y_;}
   int GetWidth(){return width_;}
   int GetHeight(){return height_;}

   void ForceStop();

protected:

   // Display
   BasicFrame* display_;
   static bool stop_;

   // Coordinate
   int x_;
   int y_;
   int width_;
   int height_;

   // current Focus window
   static Window* focus_;

   // Windows parent & child
   Window* parent_;

   struct WindowsQueue
   {
      Window* wnd_;
      WindowsQueue* next_;
   };
   WindowsQueue* windows_children_;

   //CoolspotFont* font_;
   

};

class MenuItemWindows : public Window
{
public:
   MenuItemWindows (BasicFrame* display);
   virtual ~MenuItemWindows ();

   virtual void Create(const char* label, Window* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void SetAction (IAction* action);
   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

protected:
   CString label_;
   IAction* action_;
   static SFT *fnt_italic_;
   static SFT *fnt_normal_;
};

class ScrollWindows : public Window
{
public:
   ScrollWindows (BasicFrame* display);
   virtual ~ScrollWindows ();

   virtual void RedrawChildren ();
   virtual void WindowsToDisplay(int& x, int& y);
   virtual void Scroll ( int offset_x, int offset_y);

protected:
   int scroll_offset_x_;
   int scroll_offset_y_;

};

class MenuWindows : public Window
{
public:
   MenuWindows (BasicFrame* display);
   virtual ~MenuWindows ();

   virtual void Create( Window* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void AddMenuItem (const char* label, IAction* action = nullptr);
   virtual void AddCheckMenuItem (const char* label, bool* value, IAction* action = nullptr);
   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);
   virtual void SetFocus (unsigned int index = 0);

protected:
   void ComputeScroller();

   int current_focus_;
   ScrollWindows scroll_window_;
   std::vector<MenuItemWindows*> list_item_;

};

class CheckMenuItemWindows : public MenuItemWindows
{
public:
   CheckMenuItemWindows (BasicFrame* display);
   virtual ~CheckMenuItemWindows ();

   virtual void Create(const char* label, bool* value, Window* parent, int x, int y, unsigned int width, unsigned int height);

   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

protected:
   bool * value_;
};

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

