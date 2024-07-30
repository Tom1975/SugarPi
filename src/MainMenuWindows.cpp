#include "MainMenuWindows.h"

#include <memory.h>
#include <math.h> 

MainMenuWindows::MainMenuWindows(BasicFrame* display) :
   Window(display)
{
   // Compute window size : Depending on display
   unsigned int w = display->GetWidth();
   unsigned int h = display->GetHeight();
   double ratio = (double)w / (double)h;

   Create(0, 0, 0, w, h);

   // Create Title bitmap 
   // Keep first quarter for it
   SugarboxLogo* bitmap_ = new SugarboxLogo();
   logo_ = new BitmapWindows(display);

   //logo_->Create(this, 240, 70, bitmap_);
   int lw, lh;
   bitmap_->GetSize(lw, lh);
   logo_->Create(this, (w - lw) / 2,
                               (h/4 - lh) / 2, bitmap_);

   // Create inner menu
   menu_ = new MenuWindows(display);
   menu_->Create(this, w/3, h / 3, w*2/3, 3 * h / 4);


}

MainMenuWindows::~MainMenuWindows()
{
   delete menu_;
}

void MainMenuWindows::ResetMenu()
{
   // Set focus to first item
   menu_->SetFocus(0);
}

void MainMenuWindows::Clear()
{
   Window::Clear();
}
