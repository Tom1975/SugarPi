#pragma once

//
#include <circle/logger.h>
#include <circle/string.h>

#include "CPCCore/CPCCoreEmu/simple_vector.hpp"

#include "DisplayPi.h"

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

class Windows 
{
public:
   Windows(DisplayPi* display);
   virtual ~Windows();

   virtual IAction::ActionReturn DoScreen (IEvent* event_handler);

   virtual void CreateWindow (Windows* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void AddChild(Windows* child);

   void WindowsToDisplay(int& x, int& y);
   virtual void Clear();
   virtual void Redraw (bool clear = true);
   virtual void RedrawWindow ();
   virtual void RedrawChildren ();

   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

   virtual void SetFocus ();
   virtual void RemoveFocus ();

   void Invalidate ();
   static Windows* GetFocus() { return focus_;}
   static void SetFocus(Windows* focus) { focus_ = focus;}

protected:

   // Display
   DisplayPi* display_;

   // Coordinate
   int x_;
   int y_;
   unsigned int width_;
   unsigned int height_;

   // current Focus window
   static Windows* focus_;

   // Windows parent & child
   Windows* parent_;

   struct WindowsQueue
   {
      Windows* wnd_;
      WindowsQueue* next_;
   };
   WindowsQueue* windows_children_;
};

class MenuItemWindows : public Windows
{
public:
   MenuItemWindows (DisplayPi* display);
   virtual ~MenuItemWindows ();

   virtual void CreateWindow (const char* label, Windows* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void SetAction (IAction* action);
   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

protected:
   CString label_;
   IAction* action_;
};

class MenuWindows : public Windows
{
public:
   MenuWindows (DisplayPi* display);
   virtual ~MenuWindows ();

   virtual void AddMenuItem (const char* label, IAction* action = nullptr);
   virtual void AddCheckMenuItem (const char* label, bool* value, IAction* action = nullptr);
   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);
   virtual void SetFocus (unsigned int index = 0);

protected:
   int current_focus_;
   std::vector<MenuItemWindows*> list_item_;


};

class CheckMenuItemWindows : public MenuItemWindows
{
public:
   CheckMenuItemWindows (DisplayPi* display);
   virtual ~CheckMenuItemWindows ();

   virtual void CreateWindow (const char* label, bool* value, Windows* parent, int x, int y, unsigned int width, unsigned int height);

   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

protected:
   bool * value_;
};
