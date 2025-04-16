//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "Window.h"
#include "WindowFrame.h"

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
Window::Window(DisplayPi* display) :
   display_(display), 
   frame_(nullptr),
   x_(0), y_(0), 
   width_(0), height_(0), 
   visible_(true),
   parent_(nullptr), 
   windows_children_(nullptr)
   
{
}

Window::~Window()
{
   if ( focus_ == this)
      focus_ = nullptr;

   if (parent_ != nullptr)
   {
      parent_->RemoveChild(this);
   }

}

void Window::Create (Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   // Ask for a new frame
   BasicFrame* frame = new BasicFrame();
   frame->Init(width, height, 1);
   frame->SetDisplaySize(width, height);

   parent_ = parent;
   x_ = x;
   y_ = y;
   width_ = width;
   height_ = height;

   WindowsToDisplay(x, y);
   frame->Move(x, y);

   frame_ = display_->CreateFrame(frame, width, height);
   display_->AddFrame(frame_);


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
      int* line = GetBuffer(i);
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
      int* line = GetBuffer(i);
      int size_to_clear = width_;
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

void Window::RemoveChild(Window* child)
{
   WindowsQueue** current_queue = &windows_children_;
   WindowsQueue** previous = nullptr;
   while (*current_queue != nullptr)
   {
      if ((*current_queue)->wnd_ == child)
      {
         if (previous == nullptr)
         {
            windows_children_ = (*current_queue)->next_;
         }
         else
         {
            (*previous)->next_ = (*current_queue)->next_;
         }
         return;
      }
      current_queue = &((*current_queue)->next_);
   }
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
   Window::WindowsToDisplay(x, y);

   int bmp_with, bmp_height;
   bmp->GetSize(bmp_with, bmp_height);
   for (int i = 0; i < bmp_height && i+y < GetFullHeight(); i++)
   {
      int* line = GetBuffer(i + y);
      bmp->DrawLogo(i, &line[x ]);
   }
}

// Bresenham algorithm, thanks to Wikipedia
void Window::DrawLine(int x0, int y0, int x1, int y1, unsigned int color)
{
   int dx = abs(x1 - x0);
   int sx = x0 < x1 ? 1 : -1;
   int dy = -abs(y1 - y0);
   int sy = y0 < y1 ? 1 : -1;
   int error = dx + dy;

   while (true)
   {
      int* ptr = GetBuffer(y0);
      if (ptr != nullptr && x0 < GetWidth())
         *&ptr[x0] = color;
      if (x0 == x1 && y0 == y1) break;

      int e2 = 2 * error;
      if (e2 >= dy)
      {
         error = error + dy;
         x0 = x0 + sx;
      }
      if (e2 <= dx)
      {
         error = error + dx;
         y0 = y0 + sy;
      }
   }
}

void Window::DrawPoly(unsigned int color, std::vector<Window::Point> list_pt)
{
   if (list_pt.size() == 0)
   {
      return;
   }

   int current_x = list_pt[0].x;
   int current_y = list_pt[0].y;
   Window::WindowsToDisplay(current_x, current_y);
   for (auto& it : list_pt)
   {
      Window::WindowsToDisplay(it.x, it.y);
      DrawLine(current_x, current_y, it.x, it.y, color);
      current_x = it.x;
      current_y = it.y;
   }
}

void Window::DrawRectangle(int x, int y, int w, int h, unsigned int color)
{
   Window::WindowsToDisplay(x, y);

   int* line = GetBuffer(y) + x;
   for (int ix = 0; ix < w; ix++)
   {
      *line++ = color;
   }

   for (int iy = y + 1; iy < y + h - 2; iy++)
   {
      line = GetBuffer(iy) + x;
      line[0] = color;
      line[width_ - 1] = color;
   }
   line = GetBuffer(y + h - 1) + x;
   for (int ix = 0; ix < w; ix++)
   {
      *line++ = color;
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

   if (visible_)
   {
      ClearAll();
      RedrawWindow();
      RedrawChildren();
   }
   FrameIsReady();

#ifdef PROFILE
   STOP_CHRONO
   nb_frame++;
   DWORD elapsed = (DWORD)(((s2 - s3) * 1000000) / freq);
   if (elapsed > 1000000)
   {
      //sprintf(s, "FPS : %f\n", nb_frame / (elapsed /1000000.0));
      sprintf(s, "Time for a frame : %i\n", elapsed / 1000000);
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
               Invalidate();
               exit_function = retval;
               break;
            case IAction::Action_Update:
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

void Window::WriteText(const char* text, int x, int y)
{
   if (frame_)
   {
      frame_->frame_->WriteText(text, x, y);
   }
}

SFT* Window::SelectFont(SFT* fnt)
{
   if (frame_)
   {
      return frame_->frame_->SelectFont(fnt);
   }
   return nullptr;
}

int Window::SelectColor(int color)
{
   if (frame_)
   {
      return frame_->frame_->SelectColor(color);
   }
   return 0;
}

void Window::FrameIsReady()
{
   if (frame_)
   {
      frame_->frame_->FrameIsReady();
   }
}

int* Window::GetBuffer(int y)
{
   if (frame_)
   {
      return frame_->frame_->GetBuffer(y);
   }
   return nullptr;
}

int Window::GetFullHeight()
{
   if (frame_)
   {
      return frame_->frame_->GetFullHeight();
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////

