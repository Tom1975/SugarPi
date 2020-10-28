#pragma once

#include <circle/logger.h>
#include <circle/string.h>

#include "CPCCore/CPCCoreEmu/simple_vector.hpp"
#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/Snapshot.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"

#include "DisplayPi.h"
#include "KeyboardPi.h"

#define MAX_LANGUAGE 1

#pragma pack(push, 1)

class CoolspotFont;

class ScreenMenu 
{

public:
   ScreenMenu(ILog* log, CLogger* logger, DisplayPi* display, SoundMixer* sound_mixer, KeyboardPi* keyboard, Motherboard* motherboard);
   virtual ~ScreenMenu();

   void Handle();

   int Resume();
   int InsertCartridge();
   int SugarPiSetup();
   int HardwareSetup();
   int Reset();
   int ShutDown();
   int Load();
   int Save();
   int SetSyncVbl();
   int SetSyncSound();



   void Down();
   void Up();
   void Select();

   class MenuItem
   {
   public:
      const char* label_;
      //Func function_;
      int (ScreenMenu::* function)();
   };

protected:
   int HandleMenu( MenuItem* menu);
   void DisplayMenu(MenuItem* menu);
   void DisplayButton(MenuItem* menu, int x, int y, bool selected);
   void DisplayText(const char* txt, int x, int y, bool selected);
   void BuildMenuSync(MenuItem * sync_menu);

   int LoadCprFromBuffer(const char* filepath);

   CLogger*    logger_;
   DisplayPi* display_;
   SoundMixer* sound_mixer_;
   KeyboardPi* keyboard_;

   // Menus 
   CoolspotFont *font_;
   MenuItem* current_menu_;
   MenuItem *sugarpi_setup_menu_ ;
   unsigned int selected_;
   unsigned int index_base_;

   Motherboard* motherboard_;

   // Pending actions
   bool resume_;
   CSnapshot* snapshot_;

};

#pragma pack(pop)