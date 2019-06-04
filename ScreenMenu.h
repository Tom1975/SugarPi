#pragma once

#include <circle/logger.h>
#include <circle/string.h>

#include "CPCCore/CPCCoreEmu/simple_vector.hpp"
#include "DisplayPi.h"
#include "KeyboardPi.h"

#define MAX_LANGUAGE 1

#pragma pack(push, 1)

class ScreenMenu 
{

public:
   ScreenMenu(CLogger* logger, DisplayPi* display, KeyboardPi* keyboard);
   virtual ~ScreenMenu();

   void Handle();

   void Resume();
   void ShutDown();

   void Down();
   void Up();
   void Select();

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
      Menu(CLogger* logger):
         logger_(logger),
         selected_(0)
      {
         items.clear();

      }
      void InitMenu(MenuItem* item, ...);
      void Down();
      void Up();
      void Select();

      std::vector<MenuItem*> items;
      CLogger* logger_;
      unsigned int selected_;
   };

   

protected:
   void DisplayMenu(Menu* menu);

   CLogger*    logger_;
   DisplayPi* display_;
   KeyboardPi* keyboard_;

   // Menus 
   Menu BaseMenu;
   Menu* current_menu_;

};

#pragma pack(pop)