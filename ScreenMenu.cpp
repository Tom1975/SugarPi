//
#include <memory.h>
#include "ScreenMenu.h"

#include "res\button_1.h"
#include "res\coolspot.h"


static CoolspotFont font;

ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",             &ScreenMenu::Resume},
   { "Insert Cartridge",   &ScreenMenu::InsertCartridge},
   { "Hardware Setup",     &ScreenMenu::HardwareSetup},
   { "Reset",              &ScreenMenu::Reset},
   { "Shutdown",           &ScreenMenu::ShutDown},
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




int ScreenMenu::Resume()
{
   resume_ = true;
   logger_->Write("Menu", LogNotice, "ACTION : RESUME");
   return 0;
}

int ScreenMenu::InsertCartridge()
{
   logger_->Write("Menu", LogNotice, "ACTION : InsertCartridge");
   return 0;
}

int ScreenMenu::HardwareSetup()
{
   logger_->Write("Menu", LogNotice, "ACTION : InsertCartridge");
   return 0;
}

int ScreenMenu::Reset()
{
   logger_->Write("Menu", LogNotice, "ACTION : Reset");
   return 0;
}

int ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   return 0;
}

void ScreenMenu::DisplayText(const char* txt, int x, int y, bool selected)
{
   // Display text
   int i = 0;

   char buff[16];
   memset(buff, 0, sizeof buff);
   strncpy(buff, txt, 15);
   
   unsigned int x_offset_output = 0;

   while (txt[i] != '\0' )
   {
      // Display character
      unsigned char c = buff[i];

      // Look for proper bitmap position (on first line only)
      for (int display_y = 0; display_y < font.GetLetterHeight(c); display_y++)
      {
         int* line = display_->GetVideoBuffer(display_y + y);
         font.CopyLetter(c, display_y, &line[x + x_offset_output], logger_);
      }
      x_offset_output += font.GetLetterLength(c);

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
   if (selected)
   {
      DisplayText("*", x + 2, y , selected);
   }

   DisplayText(menu->label_, x + 20, y , selected);
}

void ScreenMenu::DisplayMenu(MenuItem* menu)
{
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
      DisplayButton(&menu[i], 250, i * 20 + 70, selected_ == i);
    
      // log
      i++;
   }
}

void ScreenMenu::Handle()
{
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
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Up()
{
   if (selected_ > 0)
   {
      selected_--;
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Select()
{
   (this->*(current_menu_[selected_].function))();
   DisplayMenu(current_menu_);
}

