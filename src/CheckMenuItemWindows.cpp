//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "CheckMenuItemWindows.h"

#ifdef RASPPI
#include "BasicFrame.h"
#else
#include "DisplayPiDesktop.h"

#endif

#ifdef  __circle__
   #define PATH_FONT "SD:/FONTS/Facile_Sans.ttf"
#else
   #define PATH_FONT ".\\FONTS\\Facile_Sans.ttf" 
#endif


////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
CheckMenuItemWindows::CheckMenuItemWindows(BasicFrame* display) : MenuItemWindows(display), value_(nullptr)
{

}
CheckMenuItemWindows::~CheckMenuItemWindows()
{

}

void CheckMenuItemWindows::Create(const char* label, bool* value, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   MenuItemWindows::Create(label, parent, x, y, width, height);
   value_ = value;
}

void CheckMenuItemWindows::RedrawWindow()
{
   int x = 15;
   int y = 35;
   WindowsToDisplay(x, y);

   // Focus ?
   if (focus_ == this)
   {
      // draw it 
      display_->SelectFont(fnt_italic_);
      display_->SelectColor(0xFF0000);
   }
   else
   {
      display_->SelectFont(fnt_normal_);
      display_->SelectColor(0x000000);
   }

   // Draw the check box
   display_->WriteText((*value_) ? "[X]" : "[ ]", x, y);
   display_->WriteText(label_, x + 60, y);
}

IAction::ActionReturn CheckMenuItemWindows::HandleEvent(IEvent::Event event)
{
   //
   switch (event)
   {
   case IEvent::Event::SELECT:
      // Action !
      (*value_) = (*value_) ? false : true;
      if (action_ != nullptr)
      {
         return action_->DoAction();
      }
      break;
   default:
      if (parent_ != nullptr)
         return parent_->HandleEvent(event);
   }

   return IAction::ActionReturn::Action_None;
}
