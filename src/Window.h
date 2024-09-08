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
      Action_Reload,
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

   virtual void DrawBitmap(PiBitmap* bmp, int x, int y);

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
