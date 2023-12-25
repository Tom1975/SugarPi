//
#include <memory.h>

#include "Window.h"

#ifdef RASPPI
#include "DisplayPi.h"
#else
#include "DisplayPiDesktop.h"
#endif


Window* Window::focus_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////
Window::Window(DisplayPi* display) : display_(display), x_(0), y_(0), width_(0), height_(0), parent_(nullptr), windows_children_(nullptr)
{
}

Window::~Window()
{
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
      int* line = display_->GetVideoBuffer(i);
      memset(line, 0x0, sizeof(int) * display_->GetWidth());
   }
}

void Window::Clear()
{
   // Background
   for (int i = x_; i < display_->GetHeight() && i < y_ + height_; i++)
   {
      int* line = display_->GetVideoBuffer(i);
      int size_to_clear = width_ + y_;
      if (size_to_clear > width_)
         size_to_clear = width_;

      memset(&line[y_], 0x0, sizeof(int) * size_to_clear);
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
   /*Windows* wnd = this;
   while ( wnd->parent_ != nullptr)
   {
      wnd = wnd->parent_;
      x += wnd->x_;
      y += wnd->y_;
   }*/
}

void Window::RedrawWindow ()
{
   CLogger::Get ()->Write("Menu", LogNotice, "Windows::RedrawWindow");
}

void Window::RedrawChildren ()
{
   CLogger::Get ()->Write("Menu", LogNotice, "Windows::RedrawChildren");
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      // If a part of the windows can be displayed : do it !
      /*if ( (*current_queue)->wnd_->x_ + (*current_queue)->wnd_->width_>= 0 && (*current_queue)->wnd_->y_ + (*current_queue)->wnd_->height_ >= 0 
      && (*current_queue)->wnd_->x_ <= width_ 
      && (*current_queue)->wnd_->y_ <= height_ )*/
      {
         CLogger::Get ()->Write("Menu", LogNotice, "Draw a child window...");
         (*current_queue)->wnd_->RedrawWindow();
         (*current_queue)->wnd_->RedrawChildren();
      }

      current_queue = &((*current_queue)->next_);
   }
   CLogger::Get ()->Write("Menu", LogNotice, "Windows::RedrawChildren : Done !");

}

void Window::Invalidate ()
{
   // Clear from top windows
   Redraw ( true);
}

void Window::Redraw (bool clear)
{
   if (clear)
      Clear();

   // Redraw window
   RedrawWindow ();

   // Redraw children
   RedrawChildren ();

   // Sync
   display_->VSync();

}

IAction::ActionReturn Window::DoScreen (IEvent* event_handler)
{
   // Redraw the window
   Redraw (true);
    
   // Wait for an event
   IAction::ActionReturn exit_function = IAction::Action_None;
   while (exit_function == IAction::Action_None)
   {
      IEvent::Event event = event_handler->GetEvent();
      if (event == IEvent::NONE)
      {
          // Wait a bit
         Redraw(true);
         WAIT(10);
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
               Redraw (true);
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
MenuItemWindows::MenuItemWindows (DisplayPi* display) : Window(display), action_(nullptr)
{

}
MenuItemWindows::~MenuItemWindows ()
{

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
   CLogger::Get ()->Write("Menu", LogNotice, "MenuItemWindows::RedrawWindow");
   int x = 15;
   int y = 0;
   WindowsToDisplay(x, y);   

   // Focus ?
   if (focus_==this)
   {
      // draw it 
      display_->DisplayText ("*", x-15, y, focus_==this);
   }
   display_->DisplayText (label_, x, y, focus_==this);
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
CheckMenuItemWindows::CheckMenuItemWindows (DisplayPi* display) : MenuItemWindows(display), value_(nullptr)
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
   CLogger::Get ()->Write("Menu", LogNotice, "CheckMenuItemWindows::RedrawWindow");
   int x = 15;
   int y = 0;
   WindowsToDisplay(x, y);   

   // Focus ?
   if (focus_==this)
   {
      // draw it 
      display_->DisplayText ("*", x-15, y, focus_==this);
   }
   // Draw the check box
   display_->DisplayText ((*value_)?"[X]":"[ ]", x, y);
   display_->DisplayText(label_, x + 30, y, focus_ == this);
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
ScrollWindows::ScrollWindows (DisplayPi* display) : Window (display), scroll_offset_x_(0), scroll_offset_y_(0)
{

}

ScrollWindows::~ScrollWindows ()
{
   
}


void ScrollWindows::RedrawChildren ()
{
   CLogger::Get ()->Write("Menu", LogNotice, "ScrollWindows::RedrawChildren");
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      if ( (*current_queue)->wnd_->GetX() + scroll_offset_x_>= 0 && (*current_queue)->wnd_->GetY() +scroll_offset_y_>= 0 
      && (*current_queue)->wnd_->GetWidth() + (*current_queue)->wnd_->GetX() + scroll_offset_x_< width_ 
      && (*current_queue)->wnd_->GetHeight() + (*current_queue)->wnd_->GetY() + scroll_offset_y_< height_ )
      {
         (*current_queue)->wnd_->RedrawWindow();
         (*current_queue)->wnd_->RedrawChildren();
      }

      current_queue = &((*current_queue)->next_);
   }
   CLogger::Get ()->Write("Menu", LogNotice, "ScrollWindows::RedrawChildren Done");
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
MenuWindows::MenuWindows (DisplayPi* display) : Window (display), current_focus_(-1), scroll_window_(display)
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
   item->Create( label, &scroll_window_, 10, list_item_.size()*20, 800, 19);
   item->SetAction(action);

   list_item_.push_back(item);

   if ( current_focus_ == -1)
      current_focus_ = 0;
}

void MenuWindows::AddCheckMenuItem (const char* label, bool* value, IAction* action)
{
   // Add item to menu
   CLogger::Get ()->Write("Menu", LogNotice, "add menucheck : %s ", label);
   CheckMenuItemWindows* item = new CheckMenuItemWindows (display_);
   item->Create( label, value, &scroll_window_, 10, list_item_.size()*20, 800, 19);
   item->SetAction(action);

   list_item_.push_back(item);
   Redraw (true);

   if ( current_focus_ == -1)
      current_focus_ = 0;
}


void MenuWindows::RedrawWindow ()
{
   CLogger::Get ()->Write("Menu", LogNotice, "MenuWindows::RedrawWindow");
   int x = 450;
   int y = 47;
   WindowsToDisplay(x, y);
}

void MenuWindows::ComputeScroller()
{
   CLogger::Get ()->Write("Menu", LogNotice, "ComputeScroller");
   // check current focus, depending on windows size
   int distant_to_top = current_focus_ * 20;
   int distant_to_bottom = (list_item_.size() - (current_focus_+1)) *20;
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
            Redraw (true);
         }
         break;
      case IEvent::Event::UP:
         // Go up in the menu
         if  (current_focus_ > 0)
         {
            current_focus_--;
            list_item_.at(current_focus_)->SetFocus ();
            Redraw (true);
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
   }
   
}

BitmapWindows::BitmapWindows(DisplayPi* display): Window(display)
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
   static float offset;
   for (int i = 0; i < height_; i++)
   {
      int* line = display_->GetVideoBuffer(i + y_);
      bmp_->DrawLogo(i, &line[x_ + (int) (sin(offset)*20)]);
      offset += 0.02;
   }
   

}

