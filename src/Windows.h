#pragma once

//
#include <circle/logger.h>


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
   Windows();
   virtual ~Windows();

   virtual unsigned int DoScreen (IEvent* event_handler);

   virtual void CreateWindow (Windows* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void AddChild(Windows* child);
   virtual void Redraw ();

   bool HandleEvent( IEvent::Event event);

protected:


   // Coordinate
   int x_;
   int y_;
   unsigned int width_;
   unsigned int height_;

   // curernt Focus window
   Windows* focus_;

   // Windows parent & child
   Windows* parent_;

   struct WindowsQueue
   {
      Windows* wnd_;
      WindowsQueue* next_;
   };
   WindowsQueue* windows_children_;
};

class CMenuWindows : public Windows
{
public:
   CMenuWindows ();
   virtual ~CMenuWindows ();
};
