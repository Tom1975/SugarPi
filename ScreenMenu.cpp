//
#include <memory.h>
#include "ScreenMenu.h"

ScreenMenu::ScreenMenu(CLogger* logger, DisplayPi* display, KeyboardPi* keyboard) :
   logger_(logger),
   display_(display),
   keyboard_(keyboard),
   BaseMenu(logger),
   current_menu_(&BaseMenu)
{
   BaseMenu.InitMenu(  new MenuItem ("Resume", &ScreenMenu::Resume),
                       new MenuItem ("Shut down", &ScreenMenu::ShutDown),
                       nullptr );
}

ScreenMenu::~ScreenMenu()
{
}

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
      selected_--;
   }
}
void ScreenMenu::Menu::Up()
{
   if (selected_ + 1 < items.size())
   {
      selected_--;
   }
}
void ScreenMenu::Menu::Select()
{
   // Launch function
   //*(items[selected_]->function_)();
}


void ScreenMenu::ShutDown()
{

}

void ScreenMenu::Resume()
{

}

void ScreenMenu::DisplayMenu(Menu* menu)
{
   logger_->Write("Menu", LogNotice, "DisplayMenu");

   for (unsigned int i = 0; i < menu->items.size(); i++)
   {
      logger_->Write("Menu", LogNotice, "%s %s", menu->selected_==i?"*":" ", menu->items.at(i)->label_);
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
   current_menu_->Down();
   DisplayMenu(current_menu_);
}

void ScreenMenu::Up()
{
   current_menu_->Up();
   DisplayMenu(current_menu_);
}

void ScreenMenu::Select()
{
   current_menu_->Select();
   DisplayMenu(current_menu_);
}
