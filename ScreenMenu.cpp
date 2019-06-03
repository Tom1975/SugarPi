//
#include <memory.h>
#include "ScreenMenu.h"

ScreenMenu::ScreenMenu(CLogger* logger, DisplayPi* display) :
   logger_(logger),
   display_(display),
   BaseMenu(logger),
   current_menu_(&BaseMenu)
{
   logger_->Write("Menu", LogNotice, "MENU creation... ");
   BaseMenu.InitMenu(  new MenuItem ("Resume", &ScreenMenu::Resume),
                       new MenuItem ("Shut down", &ScreenMenu::ShutDown),
                       nullptr );

   logger_->Write("Menu", LogNotice, "MENU creation done.");
}

ScreenMenu::~ScreenMenu()
{
}

void ScreenMenu::Menu::InitMenu(MenuItem* item, ...)
{
   if (item != nullptr)
   {
      logger_->Write("Menu", LogNotice, "MENU added : %s", item->label_);
      items.push_back(item);
      logger_->Write("Menu", LogNotice, "MENU added : %s", items[0]->label_);
      logger_->Write("Menu", LogNotice, "MENU added items[i] : %p", items[0]);
      logger_->Write("Menu", LogNotice, "MENU added items[i]->label_: %p", items[0]->label_);
   }
   va_list args;
   va_start(args, item);

   MenuItem* next_item = va_arg(args, MenuItem*);
   int i = 1;
   while (next_item != nullptr)
   {
      items.push_back(next_item);
      logger_->Write("Menu", LogNotice, "MENU added : %s", next_item->label_);
      logger_->Write("Menu", LogNotice, "MENU added : %s", items[i]->label_);
      logger_->Write("Menu", LogNotice, "MENU added items[i] : %p", items[i]);
      logger_->Write("Menu", LogNotice, "MENU added items[i]->label_: %p", items[i]->label_);
      i++;
      next_item = va_arg(args, MenuItem*);
   }

   logger_->Write("Menu", LogNotice, "END MENU added : size = %i", items.size());
   for (int i = 1; i >= 0; i--)
   {
      logger_->Write("Menu", LogNotice, "END MENU added items[i] : %p", items[i]);
      logger_->Write("Menu", LogNotice, "END MENU added : %s", items[i]->label_);
      logger_->Write("Menu", LogNotice, "END MENU added items[i]->label_: %p", items[i]->label_);

   }
   va_end(args);
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

   logger_->Write("Menu", LogNotice, "DisplayMenu : Size = %i", menu->items.size());
   for (unsigned int i = 0; i < menu->items.size(); i++)
   {
      logger_->Write("Menu", LogNotice, "DisplayMenu : %i", i);
      logger_->Write("Menu", LogNotice, "menu->items[i] : %p", menu->items[i]);
      logger_->Write("Menu", LogNotice, "menu->items[i]->label_ : %p", menu->items[i]->label_);
      logger_->Write("Menu", LogNotice, "MENU : %s", menu->items.at(i)->label_);
   }
}


void ScreenMenu::Handle()
{
   logger_->Write("Menu", LogNotice, "Opening menu");

   // Display menu
   DisplayMenu(current_menu_);

   // Handle it

}
