#include "MainMenuWindows.h"

#include <memory.h>
#include <math.h> 

MainMenuWindows::MainMenuWindows(BasicFrame* display) :
   Window(display),
   full_line_(nullptr)
{
   offset_grid = 0;
   offset_grid_y = 0;

   Create(0, 0, 0, 640, 480);
   // Create Title bitmap
   SugarboxLogo* bitmap_ = new SugarboxLogo();
   logo_ = new BitmapWindows(display);
   logo_->Create(this, 240, 70, bitmap_);

   // Create inner menu
   menu_ = new MenuWindows(display);
   menu_->Create(this, 240, 200, 1000, 800);

   // Checkboard
   int w = display_->GetWidth();

   int patter_1[0x10];
   int patter_2[0x10];
   for (int l = 0; l < 0x10; l++)
   {
      patter_1[l] = 0xCCCCCC;
      patter_2[l] = 0xDDDDDD;
   }

   max_w_ = w < (x_ + width_) ? w : (x_ + width_);
   int right_border = max_w_ - 0x10;

   right_border &= 0xFFFFFF0;

   int max_width_check = (max_w_ / 0x10) + 2;
   full_line_ = new int[max_width_check * 0x10];

   bool b = true;
   for (int l = 0; l < max_width_check; l++)
   {
      memcpy(&full_line_[l * 0x10], b ? patter_1 : patter_2, sizeof(patter_1));
      b = !b;
   }
}

MainMenuWindows::~MainMenuWindows()
{
   delete menu_;
   delete[]full_line_;
}

void MainMenuWindows::ResetMenu()
{
   // Set focus to first item
   menu_->SetFocus(0);
}

void MainMenuWindows::Clear()
{
   Window::Clear();
   /*int h = display_->GetHeight();
   int bottom_border = (h < y_ + height_) ? h : (y_ + height_);

   int* line = display_->GetBuffer(y_);
   int s = display_->GetPitch();
   for (int i = y_; i < bottom_border; i++)
   {
      bool bcolor = ((offset_grid_y + i) & 0x10);
      memcpy(&line[x_], &full_line_[(offset_grid + (bcolor ? 0x10 : 0)) & 0x1F], max_w_ * sizeof(int));
      line += s;
   }

   // Move thec background !
   static float var_x = 0.0, var_y = 0.0;

   var_x += 0.005;
   var_y += 0.008;

   offset_grid = 200 * sinf(var_x);
   offset_grid &= 0x1F;
   offset_grid_y = 120 * sinf(var_y);
   offset_grid_y &= 0x1F;
   */

}
