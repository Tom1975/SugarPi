//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "Window.h"

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


Window* Window::focus_ = nullptr;
bool Window::stop_ = false;

////////////////////////////////////////////////////////////////////////////////////
Window::Window(BasicFrame* display) :
   display_(display), 
   x_(0), y_(0), 
   width_(0), height_(0), 
   visible_(true),
   parent_(nullptr), 
   windows_children_(nullptr)
   
{
   //font_ = new CoolspotFont(display_->GetPitch());
}

Window::~Window()
{
   if ( focus_ == this)
      focus_ = nullptr;
}

void Window::Create (Window* parent, int x, int y, unsigned int width, unsigned int height)
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

void Window::ClearAll()
{
   // Background
   for (int i = 0; i < display_->GetHeight() ; i++)
   {
      int* line = display_->GetBuffer(i);
      memset(line, 0x0, sizeof(int) * display_->GetWidth());
   }
}

void Window::Clear()
{
   // Background
   int x = 0, y = 0;
   WindowsToDisplay(x, y);
   for (int i = std::max<int> (0, y); i < display_->GetHeight() && i < y + height_; i++)
   {
      int* line = display_->GetBuffer(i);
      int size_to_clear = width_ + x;
      if (size_to_clear + x > display_->GetWidth())
         size_to_clear = display_->GetWidth() - x;

      memset(&line[x], 0x0, sizeof(int) * size_to_clear);
   }

}

void Window::AddChild(Window* child)
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

void Window::WindowsToDisplay(int& x, int& y)
{
   x += x_;
   y += y_;
   if ( parent_ != nullptr)
   {
      parent_->WindowsToDisplay ( x, y);
   }

}

void Window::DrawBitmap(PiBitmap* bmp, int x, int y)
{
   // Draw background of button
   int x2 = x_ + x;
   int y2 = y_ + y;
   Window::WindowsToDisplay(x2, y2);

   int bmp_with, bmp_height;
   bmp->GetSize(bmp_with, bmp_height);
   for (int i = 0; i < bmp_height && i+y2 < display_->GetFullHeight(); i++)
   {
      int* line = display_->GetBuffer(i + y2);
      bmp->DrawLogo(i, &line[x2 ]);
   }
}

void Window::RedrawWindow ()
{
}

void Window::RedrawChildren ()
{
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      if ((*current_queue)->wnd_->visible_)
      {
         (*current_queue)->wnd_->Clear();
         (*current_queue)->wnd_->RedrawWindow();
         (*current_queue)->wnd_->RedrawChildren();
      }

      current_queue = &((*current_queue)->next_);
   }
}

void Window::Invalidate ()
{
   // Clear from top windows
   if (parent_ != nullptr)
   {
      parent_->Invalidate();
   }
   else
   {
      Redraw(true);
   }
}

void Window::Redraw (bool clear)
{
#ifdef PROFILE
   static unsigned int nb_frame = 0;
   START_CHRONO
   static __int64 s3 = s1;
#endif

   ClearAll();
   if (visible_)
   {
      RedrawWindow();
      RedrawChildren();
   }
   display_->FrameIsReady();

#ifdef PROFILE
   STOP_CHRONO
   nb_frame++;
   DWORD elapsed = (DWORD)(((s2 - s3) * 1000000) / freq);
   if (elapsed > 1000000)
   {
      sprintf(s, "FPS : %f\n", nb_frame / (elapsed /1000000.0));
      OutputDebugString(s);
      s3 = s2;
      nb_frame = 0;
   }
   //PROF_DISPLAY
#endif
}


void Window::ForceStop()
{
   stop_ = true;
}

IAction::ActionReturn Window::DoScreen (IEvent* event_handler)
{
   // Redraw the window
   Redraw (true);
    
   // Wait for an event
   IAction::ActionReturn exit_function = IAction::Action_None;
   while (exit_function == IAction::Action_None && !stop_)
   {
      IEvent::Event event = event_handler->GetEvent();
      if (event == IEvent::NONE)
      { 
         WAIT(1);
      }
      else
      {
         // Send it to focused window
         IAction::ActionReturn retval = IAction::Action_None;
         if ( focus_ != nullptr)
         {
            retval = focus_->HandleEvent (event);
         }         
         switch( retval )
         {
            case IAction::Action_None:
               WAIT(1);
               break;
            case IAction::Action_Back:
            case IAction::Action_QuitMenu:
            case IAction::Action_Shutdown:
            case IAction::Action_Reload:
               exit_function = retval;
               break;
            case IAction::Action_Update:
               //Redraw (true);
               Invalidate();
               break;
            default:
               break;
         }
      }
   }
   // Back is only meant to exit one menu.
   if ( exit_function == IAction::Action_Back) exit_function = IAction::Action_None;
   return exit_function;
}

IAction::ActionReturn Window::HandleEvent( IEvent::Event event)
{
   if ( parent_ != nullptr)
   {
      return parent_->HandleEvent(event);
   }

   // try to pass event to focused window
   /*
   if ( focus_ != nullptr)
   {
      return focus_->HandleEvent (event);
   }*/

   // Otherwise, nothing to do here... which means we can leave !
   return IAction::ActionReturn::Action_QuitMenu;
}

void Window::SetFocus ()
{
   if ( focus_ != nullptr)
   {
      focus_->RemoveFocus ();
   }
   focus_ = this;
}

void Window::RemoveFocus ()
{
}


////////////////////////////////////////////////////////////////////////////////////
