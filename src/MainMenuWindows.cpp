
#include "MainMenuWindows.h"


MainMenuWindows::MainMenuWindows(BasicFrame* display) :
   Window(display)
{
   // Compute window size : Depending on display
   unsigned int w = display->GetWidth();
   unsigned int h = display->GetHeight();
   double ratio = (double)w / (double)h;

   Create(0, 10, 0, w-20, h);

   // Create Title bitmap 
   // Keep first quarter for it
   bitmap_ = new SugarboxLogo();
   logo_ = new BitmapWindows(display);

   //logo_->Create(this, 240, 70, bitmap_);
   int lw, lh;
   bitmap_->GetSize(lw, lh);
   logo_->Create(this, (w - lw) / 2,
                               (h/4 - lh) / 2, bitmap_);

   // Create inner menu
   menu_ = new MenuWindows(display);
   menu_->Create(this, 0, 20 + h / 4, w, 3 * h / 4 -40);


}

MainMenuWindows::~MainMenuWindows()
{
   delete menu_;
   delete logo_;
   delete bitmap_;
}

void MainMenuWindows::ResetMenu(unsigned int index)
{
   // Set focus to first item
   menu_->SetFocus(index);
}

void MainMenuWindows::Clear()
{
   Window::Clear();
}
