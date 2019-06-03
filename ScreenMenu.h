#pragma once

#include <circle/logger.h>
#include <circle/string.h>

#include "CPCCore/CPCCoreEmu/simple_vector.hpp"
#include "DisplayPi.h"

#define MAX_LANGUAGE 1

#pragma pack(push, 1)

class ScreenMenu 
{

public:
   ScreenMenu(CLogger* logger, DisplayPi* display);
   virtual ~ScreenMenu();

   void Handle();

   void Resume();
   void ShutDown();

   typedef void (ScreenMenu::* Func)();
   class MenuItem
   {
   public:
      MenuItem(const char* label, Func function): function_(function)
      {
         label_ = new char[strlen(label)+1];
         strcpy(label_, label);
      }
      char* label_;
      Func function_;
   };

   class Menu
   {
   public:
      Menu(CLogger* logger):logger_(logger)
      {
         items.clear();
      }
      void InitMenu(MenuItem* item, ...);
      std::vector<MenuItem*> items;
      CLogger* logger_;
   };

   

protected:
   void DisplayMenu(Menu* menu);

   CLogger*    logger_;
   DisplayPi* display_;
   // Menus 
   Menu BaseMenu;
   Menu* current_menu_;

};

#pragma pack(pop)