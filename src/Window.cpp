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
   for (int i = y; i < display_->GetHeight() && i < y + height_; i++)
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

void Window::RedrawWindow ()
{
}

void Window::RedrawChildren ()
{
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      (*current_queue)->wnd_->Clear();
      (*current_queue)->wnd_->RedrawWindow();
      (*current_queue)->wnd_->RedrawChildren();

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
   RedrawWindow ();
   RedrawChildren ();
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
   CLogger::Get()->Write("DoScreen", LogNotice, "First redraw");
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
MenuItemWindows::MenuItemWindows (BasicFrame* display) : Window(display), action_(nullptr)
{
   fnt_italic_.xOffset = 0;
   fnt_italic_.xScale = 40;
   fnt_italic_.yOffset = 0;
   fnt_italic_.yScale = 40;
   fnt_italic_.flags = SFT_DOWNWARD_Y;
   fnt_italic_.font = sft_loadfile( PATH_FONT );

   fnt_normal_.xOffset = 0;
   fnt_normal_.xScale = 32;
   fnt_normal_.yOffset = 0;
   fnt_normal_.yScale = 32;
   fnt_normal_.flags = SFT_DOWNWARD_Y;
   fnt_normal_.font = sft_loadfile(PATH_FONT);

}
MenuItemWindows::~MenuItemWindows ()
{
   sft_freefont(fnt_italic_.font);
   sft_freefont(fnt_normal_.font);
}

void MenuItemWindows::Create (const char* label, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   label_ = label;
   Window::Create ( parent, x, y, width, height);
}

void MenuItemWindows::SetAction (IAction* action)
{
   action_ = action;
}

void MenuItemWindows::RedrawWindow ( )
{
   CLogger::Get()->Write("MenuItemWindows", LogNotice, "RedrawWindow");
   int x = 30;
   // Set an offset for the text to be displayed
   int y = 15;
   WindowsToDisplay(x, y);   

   // Focus ?
   if (focus_==this)
   {
      // draw it 
      display_->SelectFont(&fnt_italic_);
      display_->SelectColor(0xFF0000);
      display_->WriteText(">", x-15, y);
      display_->WriteText(label_, x, y);
   }
   else
   {
      display_->SelectFont(&fnt_normal_);
      display_->SelectColor(0x000000);
      display_->WriteText(label_, x, y);
   }
   
   CLogger::Get()->Write("MenuItemWindows", LogNotice, "RedrawWindow end");
}

IAction::ActionReturn MenuItemWindows::HandleEvent( IEvent::Event event)
{
   //
   switch (event)
   {
      case IEvent::Event::SELECT:
         // Action !
         if (action_ != nullptr)
         {
            IAction::ActionReturn ret = action_->DoAction () ;
            return ret;
         }
         break;
      default:
         if ( parent_ != nullptr)
            return parent_->HandleEvent(event);
   }

   return IAction::ActionReturn::Action_None;
}

////////////////////////////////////////////////////////////////////////////////////
CheckMenuItemWindows::CheckMenuItemWindows (BasicFrame* display) : MenuItemWindows(display), value_(nullptr)
{

}
CheckMenuItemWindows::~CheckMenuItemWindows ()
{

}

void CheckMenuItemWindows::Create(const char* label, bool* value, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   MenuItemWindows::Create( label, parent, x, y, width, height);
   value_ = value;
}

void CheckMenuItemWindows::RedrawWindow ( )
{
   int x = 15;
   int y = 0;
   WindowsToDisplay(x, y);   

   // Focus ?
   if (focus_==this)
   {
      // draw it 
      display_->WriteText("*", x-15, y);
   }
   // Draw the check box
   display_->WriteText((*value_)?"[X]":"[ ]", x, y);
   display_->WriteText(label_, x + 30, y);
}

IAction::ActionReturn CheckMenuItemWindows::HandleEvent( IEvent::Event event)
{
   //
   switch (event)
   {
      case IEvent::Event::SELECT:
         // Action !
         (*value_) = (*value_)?false:true;
         if (action_ != nullptr)
         {
            return action_->DoAction () ;
         }
         break;
      default:
         if ( parent_ != nullptr)
            return parent_->HandleEvent(event);
   }

   return IAction::ActionReturn::Action_None;
}

////////////////////////////////////////////////////////////////////////////////////
ScrollWindows::ScrollWindows (BasicFrame* display) : Window (display), scroll_offset_x_(0), scroll_offset_y_(0)
{

}

ScrollWindows::~ScrollWindows ()
{
   
}


void ScrollWindows::RedrawChildren ()
{
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      if ( (*current_queue)->wnd_->GetX() + scroll_offset_x_>= 0 && (*current_queue)->wnd_->GetY() +scroll_offset_y_>= 0 
      && (*current_queue)->wnd_->GetWidth() + (*current_queue)->wnd_->GetX() + scroll_offset_x_<= width_ 
      && (*current_queue)->wnd_->GetHeight() + (*current_queue)->wnd_->GetY() + scroll_offset_y_<= height_ )
      {
         (*current_queue)->wnd_->RedrawWindow();
         (*current_queue)->wnd_->RedrawChildren();
      }

      current_queue = &((*current_queue)->next_);
   }
}

void ScrollWindows::WindowsToDisplay(int& x, int& y)
{
   x += x_ + scroll_offset_x_;
   y += y_ + scroll_offset_y_;
   if ( parent_ != nullptr)
   {
      parent_->WindowsToDisplay ( x, y);
   }
}

void ScrollWindows::Scroll ( int offset_x, int offset_y)
{
   scroll_offset_x_ = offset_x;
   scroll_offset_y_ = offset_y;
}

////////////////////////////////////////////////////////////////////////////////////
MenuWindows::MenuWindows (BasicFrame* display) : Window (display), current_focus_(-1), scroll_window_(display)
{

}

MenuWindows::~MenuWindows ()
{
   // Clear items
   for (auto& it:list_item_)
   {
      delete it;
   }
   list_item_.clear();
}

void MenuWindows::Create( Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   Window::Create( parent, x, y, width, height);

   // Add a simple windows that will be used for scrolling
   scroll_window_.Create( this, 0, 0, width, height);
}


void MenuWindows::AddMenuItem (const char* label, IAction* action)
{
   CLogger::Get ()->Write("Menu", LogNotice, "add menu : %s ", label);

   // Add item to menu
   MenuItemWindows* item = new MenuItemWindows (display_);
   item->Create( label, &scroll_window_, 10, list_item_.size()*40, width_ - 10, 38);
   item->SetAction(action);

   list_item_.push_back(item);

   if ( current_focus_ == -1)
      current_focus_ = 0;

   ComputeScroller();
}

void MenuWindows::AddCheckMenuItem (const char* label, bool* value, IAction* action)
{
   // Add item to menu
   CLogger::Get ()->Write("Menu", LogNotice, "add menucheck : %s ", label);
   CheckMenuItemWindows* item = new CheckMenuItemWindows (display_);
   item->Create( label, value, &scroll_window_, 10, list_item_.size()*20, width_, 19);
   item->SetAction(action);

   list_item_.push_back(item);
   Redraw (true);

   if ( current_focus_ == -1)
      current_focus_ = 0;

   ComputeScroller();
}


void MenuWindows::RedrawWindow ()
{
   int x = 450;
   int y = 47;
   WindowsToDisplay(x, y);
}

void MenuWindows::ComputeScroller()
{
   CLogger::Get ()->Write("Menu", LogNotice, "ComputeScroller");
   // check current focus, depending on windows size
   int distant_to_top = current_focus_ * 20;
   int distant_to_bottom = (static_cast<int>(list_item_.size()) - (current_focus_+1)) *20;
   int win_h = distant_to_top - height_ / 2;
   int win_h2b = distant_to_bottom - height_ / 2;

   // rules : 
   int scroll_y;
   if ( win_h < 0 || win_h2b < 0)
   {
      scroll_y = 0;
   } 
   else
   {
      scroll_y = win_h;
   }
   scroll_window_.Scroll ( 0, scroll_y);
   CLogger::Get ()->Write("Menu", LogNotice, "ComputeScroller Done; y = %i", scroll_y);
}

IAction::ActionReturn MenuWindows::HandleEvent( IEvent::Event event)
{
   // do something
   switch (event)
   {
      case IEvent::Event::DOWN:
         // Go down in the menu
         if  (current_focus_ < static_cast<int>(list_item_.size())-1)
         {
            current_focus_++;
            list_item_.at(current_focus_)->SetFocus ();
            ComputeScroller();
            //Redraw();
            Invalidate();
         }
         break;
      case IEvent::Event::UP:
         // Go up in the menu
         if  (current_focus_ > 0)
         {
            current_focus_--;
            list_item_.at(current_focus_)->SetFocus ();
            ComputeScroller();
            //Redraw();
            Invalidate();
         }
         break;
      case IEvent::Event::BACK:
         return IAction::ActionReturn::Action_Back;
         break;         
      default:
         break;
   }
   
   return IAction::ActionReturn::Action_None;
}

void MenuWindows::SetFocus (unsigned int index)
{
   // Set focus to first item
   if ( list_item_.size() > index)
   {
      current_focus_ = index;
      list_item_.at(index)->SetFocus ();
      ComputeScroller();
   }
   
}

BitmapWindows::BitmapWindows(BasicFrame* display): Window(display)
{

}

BitmapWindows::~BitmapWindows()
{

}

void BitmapWindows::Create(Window* parent, int x, int y, PiBitmap* bmp)
{
   bmp_ = bmp;
   bmp_->GetSize(width_, height_);
   Window::Create(parent, x, y, width_, height_);  
}

void BitmapWindows::RedrawWindow()
{
   CLogger::Get()->Write("BitmapWindows", LogNotice, "RedrawWindow");
   static float offset;
   for (int i = 0; i < height_; i++)
   {
      int* line = display_->GetBuffer(i + y_);
      bmp_->DrawLogo(i, &line[x_  /* + (int)(sinf(offset) * 10)*/]);
      offset += 0.002f;
   }
   //CLogger::Get()->Write("Window", LogNotice, "RedrawWindow end");

}

