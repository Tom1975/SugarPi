#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#include <circle/string.h>
#else
#include "CLogger.h"
#include "CString.h"
#endif


#include "CPCCore/CPCCoreEmu/simple_vector.hpp"
#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/Snapshot.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"

#include "DisplayPi.h"

#ifdef  __circle__
#include "KeyboardPi.h"
#include "SugarPiSetup.h"
#else
#include "KeyboardPiDesktop.h"
#include "SugarPiSetupDesktop.h"
#endif

#include "Window.h"
#include "MainMenuWindows.h"

#define MAX_LANGUAGE 1

#pragma pack(push, 1)

class IEngine
{
public: 
   virtual void LoadConfiguration(const char* config) = 0;

};

class ScreenMenu : public IEvent
{

public:

   ScreenMenu(IEngine* engine, ILog* log, CLogger* logger, DisplayPi* display, SoundMixer* sound_mixer, KeyboardPi* keyboard, Motherboard* motherboard, SugarPiSetup* setup);
   virtual ~ScreenMenu();

   IEvent::Event GetEvent ();

   IAction::ActionReturn Handle();

   IAction::ActionReturn Back();
   IAction::ActionReturn HardwareSetup();
   IAction::ActionReturn InsertCartridge();
   IAction::ActionReturn InsertDisk();
   IAction::ActionReturn InsertTape();
   IAction::ActionReturn Load();
   IAction::ActionReturn LoadAmstradSetup( const char* path);
   IAction::ActionReturn LoadCartridge ( const char* path);
   IAction::ActionReturn LoadDisk ( const char* path);
   IAction::ActionReturn LoadTape ( const char* path);
   IAction::ActionReturn Info();
   IAction::ActionReturn Reset();
   IAction::ActionReturn Resume();
   IAction::ActionReturn Save();
   IAction::ActionReturn SelectAmstrad();
   IAction::ActionReturn SetSync(bool* value);
   IAction::ActionReturn ShutDown();
   IAction::ActionReturn SugarSetup();
   
   IAction::ActionReturn InsertMedia(const char* path, IAction::ActionReturn (ScreenMenu::* load_action)(const char*));

   void LoadConfiguration  (const char* config_name, const char* ini_file);
   void ForceStop();

   class MenuItem
   {
   public:
      const char* label_;
      //Func function_;
      IAction::ActionReturn (ScreenMenu::* function)();
   };

protected:
   IEngine*       engine_;
   CLogger*       logger_;
   DisplayPi*     display_;
   SoundMixer*    sound_mixer_;
   KeyboardPi*    keyboard_;
   SugarPiSetup*  setup_;

   Motherboard*   motherboard_;

   // Pending actions
   CSnapshot*        snapshot_;
   MainMenuWindows*  main_menu_;
};

class ActionMenu : public IAction
{
public:
   ActionMenu (ScreenMenu* menu, IAction::ActionReturn (ScreenMenu::*pfnAction)()): menu_(menu), pfnAction_(pfnAction)
   {};

   IAction::ActionReturn DoAction()
   {
      return (menu_->*pfnAction_)();
   };

protected:
   ScreenMenu* menu_;
   IAction::ActionReturn (ScreenMenu::* pfnAction_)();
};

template<typename T>
class ActionMenuWithParameter : public IAction
{
public:
   ActionMenuWithParameter(ScreenMenu* menu, IAction::ActionReturn (ScreenMenu::*pfnAction)(T), T param) : menu_(menu), pfnAction_(pfnAction), parameter_(param)
   {};

   IAction::ActionReturn DoAction()
   {
      return (menu_->*pfnAction_)(parameter_);
   };

protected:
   ScreenMenu* menu_;
   IAction::ActionReturn (ScreenMenu::* pfnAction_)(T);
   T parameter_;
};


#pragma pack(pop)