//
#include "Windows.h"


Windows::Windows() : x_(0), y_(0), width_(0), height_(0), focus_(nullptr), parent_(nullptr), windows_children_(nullptr)
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
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      (*current_queue)->wnd_->Redraw();
      current_queue = &((*current_queue)->next_);
   }

}

unsigned int Windows::DoScreen (IEvent* event_handler)
{
   // Redraw the window
   Redraw ();
    
   // Wait for an event
   bool exit_function = false;

   while (exit_function )
   {
      IEvent::Event event = event_handler->GetEvent();
      if (event == IEvent::NONE)
      {
          // Wait a bit
         CTimer::Get ()->MsDelay (10);
      }
      else
      {
         // Send it to focused window
         if ( HandleEvent (event) )
            exit_function = true;
      }
   }
}

bool Windows::HandleEvent( IEvent::Event event)
{
   // try to pass event to focused window
   if ( focus_ != nullptr)
   {
      return focus_->HandleEvent (event);
   }

   // Otherwise, nothing to do here... which means we can leave !
   return true;
}

CMenuWindows::CMenuWindows ()
{

}

CMenuWindows::~CMenuWindows ()
{

}

