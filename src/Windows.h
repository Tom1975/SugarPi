#pragma once

//
#include <circle/logger.h>

class Windows 
{
public:
   Windows();
   virtual ~Windows();

   virtual void CreateWindow (Windows* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void AddChild(Windows* child);
   virtual void Redraw ();

protected:


   // Coordinate
   int x_;
   int y_;
   unsigned int width_;
   unsigned int height_;

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
