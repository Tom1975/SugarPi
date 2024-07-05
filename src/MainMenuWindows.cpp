#include "MainMenuWindows.h"

#include <memory.h>
#include <math.h> 

MainMenuWindows::MainMenuWindows(BasicFrame* display) :
   Window(display)
{

   Create(0, 0, 0, 640, 480);
   // Create Title bitmap
   SugarboxLogo* bitmap_ = new SugarboxLogo();
   logo_ = new BitmapWindows(display);
   logo_->Create(this, 240, 70, bitmap_);

   // Create inner menu
   menu_ = new MenuWindows(display);
   menu_->Create(this, 240, 200, 1000, 800);


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
