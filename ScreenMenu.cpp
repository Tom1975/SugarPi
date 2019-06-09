//
#include <memory.h>
#include "ScreenMenu.h"

#include "res\button_1.h"
#include "res\PressStart2P.c"


ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",    &ScreenMenu::Resume},
   { "Shutdown", &ScreenMenu::ShutDown},
   { nullptr, nullptr}
};

ScreenMenu::ScreenMenu(CLogger* logger, DisplayPi* display, KeyboardPi* keyboard) :
   logger_(logger),
   display_(display),
   keyboard_(keyboard),
   //BaseMenu(logger),
   current_menu_(base_menu),
   selected_(0)
{
}

ScreenMenu::~ScreenMenu()
{
}

int ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   return 0;
}

int ScreenMenu::Resume()
{
   resume_ = true;
   logger_->Write("Menu", LogNotice, "ACTION : RESUME");
   return 0;
}

void ScreenMenu::DisplayText(const char* txt, int x, int y, bool selected)
{
   // Display text
   int i = 0;

   logger_->Write("Menu", LogNotice, "Display Text");

   char buff[16];
   memset(buff, 0, sizeof buff);
   strncpy(buff, txt, 15);
   logger_->Write("Menu", LogNotice, "Display : %s", buff);
   

   while (txt[i] != '\0' )
   {
      // Display character
      unsigned char c = buff[i];

      // x / y of this char
      unsigned int y_c = (c - font_1.first_ascii) / font_1.char_per_line;
      unsigned int x_c = (c - font_1.first_ascii) - (y_c* font_1.char_per_line);

      logger_->Write("Menu", LogNotice, "Display : %c, x=%i, y=%i; display : x=%i, y=%i", (char)c, x_c, y_c, x + i * (font_1.char_width + 1), y);

      // Blit it 
      for (int display_y = 0; display_y <= font_1.char_height; display_y++)
      {
         int* line = display_->GetVideoBuffer(display_y+y);
         unsigned int offset = (y_c * font_1.width * font_1.char_height + display_y * font_1.width + x_c * font_1.char_width) * font_1.bytes_per_pixel;

         for (int display_x = 0; display_x <= font_1.char_width; display_x++)
         {
            unsigned char* col_buffer = (unsigned char*)&line[x + display_x + i* (font_1.char_width +1)];
            for (int col = 0; col < font_1.bytes_per_pixel; col++)
            {
               col_buffer[col] = font_1.pixel_data[offset++];
            }
/*            line[display_x+x] = font_1.pixel_data[offset] << 8
               | font_1.pixel_data[offset + 1]
               | font_1.pixel_data[offset + 2] << 16
               | font_1.pixel_data[offset + 3] << 24;
            offset += 4;*/
         }
      }
      i++;
   }
}

void ScreenMenu::DisplayButton(MenuItem* menu, int x, int y, bool selected)
{
   int index_bmp = 0;
   /*for (int j = y; j < y + Button_1_h; j++)
   {
      int* line = display_->GetVideoBuffer(j);
      for (int i = x; i < x + Button_1_w; i++)
      {
         if (selected)
         {
            line[i] = Button_1[index_bmp] << 8
               | Button_1[index_bmp + 1] << 16
               | Button_1[index_bmp + 2] ;
         }
         else
         {
            line[i] = Button_1[index_bmp] << 8
               | Button_1[index_bmp + 1]
               | Button_1[index_bmp + 2] << 16;
         }
         index_bmp += 3;
      }
   }*/
   // Display text
   DisplayText(menu->label_, x + 2, y + 25, selected);
}

void ScreenMenu::DisplayMenu(MenuItem* menu)
{
   logger_->Write("Menu", LogNotice, "DisplayMenu");

   // Background
   for (int i = 0; i < display_->GetHeight(); i++)
   {
      int* line = display_->GetVideoBuffer(i);
      for (int x = 0; x < display_->GetWidth(); x++)
      {
         line[x] = ((i << 6)&0xFF00) | ((x << 14)&0xFF0000);
      }
   }

   DisplayText("SugarPi", 450, 47, false);

   unsigned int i = 0;
   //for (unsigned int i = 0; i < menu->items.size(); i++)
   while (menu[i].function != nullptr)
   {
      // Display menu bitmap
      DisplayButton(&menu[i], 250, i * 40 + 70, selected_ == i);
    
      // log
      logger_->Write("Menu", LogNotice, "%s %s", selected_==i?"*":" ", menu[i].label_);
      i++;
   }
}

void ScreenMenu::Handle()
{

   logger_->Write("Menu", LogNotice, "Opening menu");

   // Display menu
   DisplayMenu(current_menu_);

   // Reinit actions
   resume_ = false;
   keyboard_->ClearBuffer();

   // Handle it
   while (resume_ == false)
   {
      // Any key pressed ?
      if (keyboard_->IsDown())
      {
         Down();
      }
      if (keyboard_->IsUp())
      {
         Up();
      }
      if (keyboard_->IsAction())
      {
         Select();
      }

   }
}

void ScreenMenu::Down()
{
   if (current_menu_[selected_ + 1].function != nullptr)
   {
      selected_++;
      logger_->Write("Menu", LogNotice, "Up : %i", selected_);
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Up()
{
   if (selected_ > 0)
   {
      selected_--;
      logger_->Write("Menu", LogNotice, "Down : %i", selected_);
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Select()
{
   (this->*(current_menu_[selected_].function))();
   DisplayMenu(current_menu_);
}
