#pragma once

//
#include <circle/logger.h>

class Windows 
{
public:
   Windows(Windows* parent = nullptr);
   virtual ~Windows();

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