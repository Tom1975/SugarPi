#pragma once

#include "Window.h"
#include "schrift.h"


class MenuItemWindows : public Window
{
public:
   MenuItemWindows (BasicFrame* display);
   virtual ~MenuItemWindows ();

   virtual void Create(const char* label, Window* parent, int x, int y, unsigned int width, unsigned int height);
   virtual void ChangeLabel(const char* label);
   virtual void SetAction (IAction* action);
   virtual void RedrawWindow ();
   virtual IAction::ActionReturn HandleEvent( IEvent::Event event);

protected:
   CString label_;
   IAction* action_;
   static SFT *fnt_italic_;
   static SFT *fnt_normal_;
};
