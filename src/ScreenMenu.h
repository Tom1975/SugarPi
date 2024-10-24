#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#include <circle/string.h>

#define stricmp strcasecmp
#define strnicmp strncasecmp

#else
#include "CLogger.h"
#include "CString.h"
#endif


#include <vector>
#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/Snapshot.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "MultiLanguage.h"
#include "DisplayPi.h"
#include "SimpleBitmap.h"
#include "KeyboardPi.h"

#ifdef  __circle__
#include "SugarPiSetup.h"
#else
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
private:
   class AmstradCompleteConf
   {
   public:
      std::string fullname_;
      const char* config_path_;
   };

   class AmstradConfiguration
   {
   public:
      std::string name_;
      SimpleBitmap associatedBmp_;
      std::vector<AmstradCompleteConf> languages_;
   };


public:

   ScreenMenu(IEngine* engine, ILog* log, CLogger* logger, 
         DisplayPi* display, 
         SoundMixer* sound_mixer, 
         KeyboardPi* keyboard, 
         Motherboard* motherboard, 
         SugarPiSetup* setup, 
         MultiLanguage* language);

   virtual ~ScreenMenu();

   void Reload();

   IEvent::Event GetEvent ();

   IAction::ActionReturn Handle();

   IAction::ActionReturn Back();
   IAction::ActionReturn ChangeLanguage();
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
   IAction::ActionReturn SelectAmstradFinal(ScreenMenu::AmstradConfiguration& config);
   IAction::ActionReturn SelectAmstradCustom();
   IAction::ActionReturn SetLanguage(int value);
   IAction::ActionReturn SetSync(bool* value);
   IAction::ActionReturn ShutDown();
   IAction::ActionReturn SugarSetup();
   
   IAction::ActionReturn InsertMedia(const char* path, IAction::ActionReturn (ScreenMenu::* load_action)(const char*));

   void LoadConfiguration  (const char* config_name, const char* ini_file);
   void ForceStop();

   MultiLanguage* language_;

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

   static std::vector<ScreenMenu::AmstradConfiguration> config_list;
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