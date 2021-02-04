//
#include "Windows.h"


Windows::Windows() : x_(0), y_(0), width_(0), height_(0), parent_(nullptr), windows_children_(nullptr)
{
}

Windows::~Windows()
{
}

void Windows::CreateWindow (Windows* parent, int x, int y, unsigned int width, unsigned int height)
{
   parent_ = parent;
   x_ = x;
   y_ = y;
   width_ = width;
   height_ = height;

   if ( parent_ != nullptr)
   {
      parent_->AddChild(this);
   }
}

void Windows::AddChild(Windows* child)
{
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      current_queue = &((*current_queue)->next_);
   }
   *current_queue = new WindowsQueue;
   (*current_queue)->wnd_ = child;
   (*current_queue)->next_ = nullptr;
}

void Windows::Redraw ()
{
   // Redraw window
   // Redraw children
}


CMenuWindows::CMenuWindows ()
{

}

CMenuWindows::~CMenuWindows ()
{

}

