//
#include <memory.h>

#include <circle/logger.h>

#include "Windows.h"

Windows* Windows::focus_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////
Windows::Windows(DisplayPi* display) : display_(display), x_(0), y_(0), width_(0), height_(0), parent_(nullptr), windows_children_(nullptr)
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

void Windows::Clear()
{
   // Background
   for (int i = 0; i < display_->GetHeight(); i++)
   {
      int* line = display_->GetVideoBuffer(i);
      memset(line, 0x0, sizeof(int) * display_->GetWidth());
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

void Windows::WindowsToDisplay(int& x, int& y)
{
   Windows* wnd = this;
   while ( wnd->parent_ != nullptr)
   {
      wnd = wnd->parent_;
      x += wnd->x_;
      y += wnd->y_;
   }
}

void Windows::RedrawWindow ()
{
}

void Windows::RedrawChildren ()
{
   WindowsQueue** current_queue = &windows_children_;
   while ( *current_queue != nullptr)
   {  
      (*current_queue)->wnd_->RedrawWindow();
      (*current_queue)->wnd_->RedrawChildren();
      current_queue = &((*current_queue)->next_);
   }


}

void Windows::Invalidate ()
{
   // Clear from top windows
   Redraw ( true);
}

void Windows::Redraw (bool clear)
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

IAction::ActionReturn Windows::DoScreen (IEvent* event_handler)
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
         CTimer::Get ()->MsDelay (10);
      }
      else
      {
         // Send it to focused window
         IAction::ActionReturn retval = HandleEvent (event);
         switch( retval )
         {
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

IAction::ActionReturn Windows::HandleEvent( IEvent::Event event)
{
   // try to pass event to focused window
   if ( focus_ != nullptr)
   {
      return focus_->HandleEvent (event);
   }

   // Otherwise, nothing to do here... which means we can leave !
   return IAction::ActionReturn::Action_QuitMenu;
}

void Windows::SetFocus ()
{
   if ( focus_ != nullptr)
   {
      focus_->RemoveFocus ();
   }
   focus_ = this;
}

void Windows::RemoveFocus ()
{
}

////////////////////////////////////////////////////////////////////////////////////
MenuItemWindows::MenuItemWindows (DisplayPi* display) : Windows(display), action_(nullptr)
{

}
MenuItemWindows::~MenuItemWindows ()
{

}

void MenuItemWindows::CreateWindow (const char* label, Windows* parent, int x, int y, unsigned int width, unsigned int height)
{
   label_ = label;
   Windows::CreateWindow ( parent, x, y, width, height);
}

void MenuItemWindows::SetAction (IAction* action)
{
   action_ = action;
}

void MenuItemWindows::RedrawWindow ( )
{
   int x = x_ + 15;
   int y = y_;
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

void CheckMenuItemWindows::CreateWindow (const char* label, bool* value, Windows* parent, int x, int y, unsigned int width, unsigned int height)
{
   MenuItemWindows::CreateWindow( label, parent, x, y, width, height);
   value_ = value;
}

void CheckMenuItemWindows::RedrawWindow ( )
{
   int x = x_ + 15;
   int y = y_;
   WindowsToDisplay(x, y);   

   // Focus ?
   if (focus_==this)
   {
      // draw it 
      display_->DisplayText ("*", x-15, y, focus_==this);
   }
   // Draw the check box
   display_->DisplayText ((*value_)?"[X]":"[ ]", x, y);

   display_->DisplayText (label_, x+30, y, focus_==this);
   
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
MenuWindows::MenuWindows (DisplayPi* display) : Windows (display), current_focus_(-1)
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

void MenuWindows::AddMenuItem (const char* label, IAction* action)
{
   // Add item to menu
   MenuItemWindows* item = new MenuItemWindows (display_);
   item->CreateWindow ( label, this, 10, list_item_.size()*20, 800, 19);
   item->SetAction(action);

   list_item_.push_back(item);

   if ( current_focus_ == -1)
      current_focus_ = 0;
}

void MenuWindows::AddCheckMenuItem (const char* label, bool* value, IAction* action)
{
   // Add item to menu
   CheckMenuItemWindows* item = new CheckMenuItemWindows (display_);
   item->CreateWindow ( label, value, this, 10, list_item_.size()*20, 800, 19);
   item->SetAction(action);

   list_item_.push_back(item);
   Redraw (true);

   if ( current_focus_ == -1)
      current_focus_ = 0;
}


void MenuWindows::RedrawWindow ()
{
   int x = 450;
   int y = 47;
   WindowsToDisplay(x, y);
   display_->DisplayText("SugarPi", x, y);
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