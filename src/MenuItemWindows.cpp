//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "MenuItemWindows.h"

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


////////////////////////////////////////////////////////////////////////////////////
SFT *MenuItemWindows::fnt_italic_ = nullptr;
SFT *MenuItemWindows::fnt_normal_ = nullptr;

MenuItemWindows::MenuItemWindows (BasicFrame* display) : Window(display), action_(nullptr)
{
   if (fnt_italic_ == nullptr)
   {
      fnt_italic_ = new SFT;
      fnt_italic_->xOffset = 0;
      fnt_italic_->xScale = 40;
      fnt_italic_->yOffset = 0;
      fnt_italic_->yScale = 40;
      fnt_italic_->flags = SFT_DOWNWARD_Y;

      CLogger::Get()->Write("MenuItemWindows", LogNotice, "Loading %s", PATH_FONT);
      fnt_italic_->font = sft_loadfile(PATH_FONT);
      CLogger::Get()->Write("MenuItemWindows", LogNotice, "Result : %X", fnt_italic_->font);

      fnt_normal_ = new SFT;
      fnt_normal_->xOffset = 0;
      fnt_normal_->xScale = 32;
      fnt_normal_->yOffset = 0;
      fnt_normal_->yScale = 32;
      fnt_normal_->flags = SFT_DOWNWARD_Y;
      fnt_normal_->font = sft_loadfile(PATH_FONT);
   }

}
MenuItemWindows::~MenuItemWindows ()
{
   //sft_freefont(fnt_italic_->font);
   //sft_freefont(fnt_normal_->font);
}

void MenuItemWindows::Create (const char* label, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   label_ = label;
   Window::Create ( parent, x, y, width, height);
}

void MenuItemWindows::ChangeLabel(const char* label)
{
   label_ = label;
}

void MenuItemWindows::SetAction (IAction* action)
{
   action_ = action;
}

void MenuItemWindows::RedrawWindow ( )
{
   int x = 30;
   // Set an offset for the text to be displayed
   int y = 15;
   WindowsToDisplay(x, y);   

   // Focus ?
   if (focus_==this)
   {
      // draw it 
      display_->SelectFont(fnt_italic_);
      display_->SelectColor(0xFF0000);
      display_->WriteText(">", x-15, y);
      display_->WriteText(label_, x, y);
   }
   else
   {
      display_->SelectFont(fnt_normal_);
      display_->SelectColor(0x000000);
      display_->WriteText(label_, x, y);
   }
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
