//
#include <memory.h>
#include "ScreenMenu.h"

ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",    &ScreenMenu::Resume},
   { "Shut down", &ScreenMenu::ShutDown},
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
/*
void ScreenMenu::Menu::InitMenu(MenuItem* item, ...)
{
   if (item != nullptr)
   {
      items.push_back(item);
   }
   va_list args;
   va_start(args, item);

   MenuItem* next_item = va_arg(args, MenuItem*);
   int i = 1;
   while (next_item != nullptr)
   {
      items.push_back(next_item);
      i++;
      next_item = va_arg(args, MenuItem*);
   }

   va_end(args);
}

void ScreenMenu::Menu::Down()
{
   if (selected_ + 1 < items.size())
   {
      selected_++;
      logger_->Write("Menu", LogNotice, "Up : %i", selected_);
   }
}
void ScreenMenu::Menu::Up()
{
   if (selected_ > 0)
   {
      selected_--;
      logger_->Write("Menu", LogNotice, "Down : %i", selected_);
   }
}
void ScreenMenu::Menu::Select()
{
   // Launch function
  //int ret = (*(items[selected_]->function_))();
}
*/

int ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   return 0;
}

int ScreenMenu::Resume()
{
   logger_->Write("Menu", LogNotice, "ACTION : RESUME");
   return 0;
}

void ScreenMenu::DisplayMenu(MenuItem* menu)
{
   logger_->Write("Menu", LogNotice, "DisplayMenu");

   unsigned int i = 0;

   //for (unsigned int i = 0; i < menu->items.size(); i++)
   while (menu[i].function != nullptr)
   {
      logger_->Write("Menu", LogNotice, "%s %s", selected_==i?"*":" ", menu[i].label_);
      i++;
   }
}

void ScreenMenu::Handle()
{
   logger_->Write("Menu", LogNotice, "Opening menu");

   // Display menu
   DisplayMenu(current_menu_);

   // Handle it
   while (true)
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
