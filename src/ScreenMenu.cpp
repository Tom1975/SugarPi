//
#include "ScreenMenu.h"

#include <memory.h>
#include <math.h> 

#include "files.h"

#ifdef  __circle__
#include <strings.h>
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

#include "res/button_1.h"
//#include "res/coolspot.h"

#define MAX_ITEM_PER_PAGE 10
#define MOVE_BASE 7

#define MAX_SIZE_BUFFER 256


ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",             &ScreenMenu::Resume},
   { "Select Amstrad",     &ScreenMenu::SelectAmstrad},
   { "Insert Cartridge",   &ScreenMenu::InsertCartridge},
   { "Insert Disk",        &ScreenMenu::InsertDisk},
   { "Insert Tape",        &ScreenMenu::InsertTape},
   { "SugarPi Setup",      &ScreenMenu::SugarSetup},
/*   { "Hardware Setup",     &ScreenMenu::HardwareSetup},
   { "Quick Save",         &ScreenMenu::Save},
   { "Quick Load",         &ScreenMenu::Load},*/
   { "Reset",              &ScreenMenu::Reset},
   { "Info",              &ScreenMenu::Info},
   { "Shutdown",           &ScreenMenu::ShutDown},
   { nullptr, nullptr}
};

ScreenMenu::ScreenMenu(IEngine* engine, ILog* log, CLogger* logger, DisplayPi* display, SoundMixer* sound_mixer, KeyboardPi* keyboard, Motherboard* motherboard, SugarPiSetup* setup) :
   engine_(engine),
   logger_(logger),
   display_(display),
   sound_mixer_(sound_mixer),
   keyboard_(keyboard),
   setup_(setup),
   motherboard_(motherboard),
   snapshot_(nullptr)
{
   snapshot_ = new CSnapshot(log);
   snapshot_->SetMachine(motherboard_);
   /////////////////////////////////////////////////
   // Window creation

   // Create Main window menu 
   unsigned int i = 0;
   main_menu_ = new MainMenuWindows (display_->GetMenuFrame());
   while (base_menu[i].label_ != nullptr && i < MAX_ITEM_PER_PAGE)
   {
      // Display menu bitmap
      main_menu_->GetMenu()->AddMenuItem(base_menu[i].label_, new ActionMenu(this, base_menu[i].function));
      
      i++;
   }
   
}

ScreenMenu::~ScreenMenu()
{
   delete snapshot_;
   delete main_menu_;
  
}

IAction::ActionReturn ScreenMenu::SetSync(bool* value)
{
   setup_->SetSync (*value ? SugarPiSetup::SYNC_FRAME:SugarPiSetup::SYNC_SOUND);
   setup_->Save();
   return IAction::Action_Update;
}

IAction::ActionReturn ScreenMenu::Back()
{
   return IAction::Action_Back;
}

IAction::ActionReturn ScreenMenu::Resume()
{
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::LoadAmstradSetup( const char* path)
{
   CString fullpath = PATH_CONFIGS;
   fullpath.Append( INTER_FILE );
   fullpath.Append( path);
   logger_->Write("Menu", LogNotice, "Load Amstrad Setup : %s", (const char*)fullpath);

   // Todo : add
   engine_->LoadConfiguration((const char*)fullpath);
   
   logger_->Write("Amstrad Setup ", LogNotice, "file loaded. Exiting menu");

   return IAction::Action_QuitMenu;
}

void ScreenMenu::LoadConfiguration  (const char* config_name, const char* ini_file)
{
   ConfigurationManager* configuration_manager = setup_-> GetConfigurationManager ();
   if (configuration_manager == nullptr) return;
   char tmp_buffer [MAX_SIZE_BUFFER ];

   /*configuration_manager->GetConfiguration(config_name, "LimitSpeed", "Y", tmp_buffer, MAX_SIZE_BUFFER, ini_file);
   if ( tmp_buffer[0] == 'F') speed_limit_ = E_FULL;
   else if ( tmp_buffer[0] == 'V') speed_limit_ = E_VBL ;
   else speed_limit_ = E_NONE ;
*/
   configuration_manager->GetConfiguration(config_name, "FD1_Path", "", tmp_buffer, MAX_SIZE_BUFFER, ini_file);
   if ( strlen(tmp_buffer) > 0)
   {
      motherboard_->GetFDC()->LoadDisk (0, tmp_buffer, false);
   }
   configuration_manager->GetConfiguration(config_name, "FD2_Path", "", tmp_buffer, MAX_SIZE_BUFFER, ini_file);
   if ( strlen(tmp_buffer) > 0)
   {
      motherboard_->GetFDC()->LoadDisk (1, tmp_buffer, false);
   }

   // Configuration
   CString default_path_cfg = PATH_CONFIGS;
   default_path_cfg.Append( INTER_FILE );
   default_path_cfg.Append( "CPC6128PLUSEN.cfg");

   
}

IAction::ActionReturn ScreenMenu::SelectAmstrad()
{
   CString search_path = PATH_CONFIGS;
#ifndef __circle__
   search_path.Append("\\*.*");
#endif

   DIR Directory;
   FILINFO *FileInfo = new FILINFO;
   FRESULT Result = f_findfirst(&Directory, FileInfo, search_path, "*.cfg");
   std::vector<FILINFO*> config_list;
   
   int limit = 0;
   // Create menu
   logger_->Write("Menu", LogNotice, "Amstrad Setup : Start of directory reading...");
   unsigned int i = 0;
   for (i = 0; Result == FR_OK && FileInfo->fname[0]; i++)
   {
      limit++;
      if ((FileInfo->fattrib & (AM_HID | AM_SYS | AM_DIR)) == 0)
      {
         config_list.push_back(FileInfo);
      }
      else
      {
         delete FileInfo;
      }
      FileInfo = new FILINFO;
      Result = f_findnext(&Directory, FileInfo);
   }
   logger_->Write("Menu", LogNotice, "Amstrad Setup : End of directory reading");
   logger_->Write("Menu", LogNotice, "Amstrad Setup : Start of alphabetical sorting...");
   // Alphabetical Order
   FILINFO** array_ordered = new FILINFO* [config_list.size()];
   unsigned int nb_file_ordered = 0;

   for (auto& it:config_list)
   {
      unsigned int place = nb_file_ordered;
      // find right place
      for (unsigned int i = 0; i < nb_file_ordered && place == nb_file_ordered; i++)
      {
         if ( stricmp (it->fname, array_ordered[i]->fname)< 0)
         {
            place = i;
         }
      }
      // insert it  :
      // Move everything after place
      if ( place != nb_file_ordered)
      {
         //for (unsigned int i = 0; i < nb_file_ordered - place; i++)
         // TODO : Fix this mess !
         for (unsigned int i = nb_file_ordered; i > place ; i++)
         {
            array_ordered [i] = array_ordered [i-1];
            //array_ordered [nb_file_ordered - i] = array_ordered [nb_file_ordered - 1 - i];
         }
      }
      // insert new item
      array_ordered[place] = it;
      nb_file_ordered++;
   }

   logger_->Write("Menu", LogNotice, "Amstrad Setup : End of alphabetical sorting");

   logger_->Write("Menu", LogNotice, "Amstrad SetupInsert Media : Strat of Menu creation...");

   // Create selection menu
   Window* focus = Window::GetFocus();

   MainMenuWindows* file_menu = new MainMenuWindows (display_->GetMenuFrame());

   file_menu->GetMenu()->AddMenuItem("..", new ActionMenu( this, &ScreenMenu::Back) );

   for (unsigned int i = 0; i < nb_file_ordered; i++)
   {
      // Display menu bitmap
      file_menu->GetMenu()->AddMenuItem(array_ordered[i]->fname, new ActionMenuWithParameter<const char*>(this, &ScreenMenu::LoadAmstradSetup, array_ordered[i]->fname) );
      
      i++;
   }
   logger_->Write("Menu", LogNotice, " : End of Menu creation");
   file_menu->ResetMenu ();

   IAction::ActionReturn return_value = file_menu->DoScreen (this);

   logger_->Write("Menu", LogNotice, "file_menu->DoScreen : %i", return_value);

   delete file_menu;
   delete [] array_ordered;
   for (auto& it:config_list)
   {
      delete it;
   }
   Window::SetFocus(focus);
   main_menu_->Invalidate ();

   logger_->Write("Menu", LogNotice, "Return from SelectAmstrad : %i", return_value);

   return return_value;
}

IAction::ActionReturn ScreenMenu::LoadCartridge( const char* path)
{
   CString fullpath = PATH_CARTIRDGE;
   fullpath.Append( INTER_FILE );
   fullpath.Append( path);
   logger_->Write("Menu", LogNotice, "Load cartridge fullpath : %s", (const char*)fullpath);
   setup_->LoadCartridge (fullpath);
   setup_->Save();
   logger_->Write("Cartridge", LogNotice, "file loaded.Exiting menu");

   motherboard_->OnOff();
   return IAction::Action_QuitMenu;
}
IAction::ActionReturn ScreenMenu::LoadDisk( const char* path)
{
   CString fullpath = PATH_DISK;
   fullpath.Append( INTER_FILE );
   fullpath.Append( path);
   logger_->Write("Menu", LogNotice, "Load Disk fullpath : %s", (const char*)fullpath);
   
   int ret = motherboard_->GetFDC()->LoadDisk(0, fullpath, false);
   logger_->Write("Disk", LogNotice, "file loaded : %i. Exiting menu", ret);

   setup_->Save();

   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::LoadTape( const char* path)
{
   CString fullpath = PATH_TAPE;
   fullpath.Append( INTER_FILE );
   fullpath.Append( path);
   logger_->Write("Menu", LogNotice, "Load Tape fullpath : %s", (const char*)fullpath);
   motherboard_->GetTape()->InsertTape (fullpath);
   setup_->Save();
   logger_->Write("Tape", LogNotice, "file loaded. Exiting menu");

   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::InsertMedia(const char* path, IAction::ActionReturn (ScreenMenu::* load_action)(const char*))
{
   DIR Directory;
   CString search_path = path;
#ifndef __circle__
   search_path.Append("\\*.*");
#endif
   FILINFO *FileInfo = new FILINFO;
   FRESULT Result = f_findfirst(&Directory, FileInfo, search_path, "*");
   std::vector<FILINFO*> cartridge_list;

   int limit = 0;
   // Create menu
   logger_->Write("Menu", LogNotice, "Insert Media : Start of directory reading...");
   unsigned int i = 0;
   for (i = 0; Result == FR_OK && FileInfo->fname[0]; i++)
   {
      limit++;
      if ((FileInfo->fattrib & (AM_HID | AM_SYS|AM_DIR)) == 0)
      {
         cartridge_list.push_back(FileInfo);
      }
      FileInfo = new FILINFO;

      Result = f_findnext(&Directory, FileInfo);
   }
   logger_->Write("Menu", LogNotice, "Insert Media : End of directory reading");
   logger_->Write("Menu", LogNotice, "Insert Media : Start of alphabetical sorting...");
   // Alphabetical Order
   FILINFO** array_ordered = new FILINFO* [cartridge_list.size()];
   unsigned int nb_file_ordered = 0;
   
   for (auto& it:cartridge_list)
   {
      unsigned int place = nb_file_ordered;
      // find right place
      for (unsigned int i = 0; i < nb_file_ordered && place == nb_file_ordered; i++)
      {
         if ( stricmp (it->fname, array_ordered[i]->fname)< 0)
         {
            place = i;
         }
      }
      // insert it  :
      // Move everything after place
      if ( place != nb_file_ordered)
      {
         for (unsigned int i = 0; i < nb_file_ordered - place; i++)
         {
            array_ordered [nb_file_ordered - i] = array_ordered [nb_file_ordered - 1 - i];
         }
      }
      // insert new item
      array_ordered[place] = it;
      nb_file_ordered++;
   }
   logger_->Write("Menu", LogNotice, "Insert Media : End of alphabetical sorting");

   logger_->Write("Menu", LogNotice, "Insert Media : Strat of Menu creation...");

   // Create selection menu
   Window* focus = Window::GetFocus();

   MainMenuWindows* file_menu = new MainMenuWindows (display_->GetMenuFrame());

   file_menu->GetMenu()->AddMenuItem("..", new ActionMenu( this, &ScreenMenu::Back) );

   for (unsigned int i = 0; i < nb_file_ordered; i++)
   {
      // Display menu bitmap
      file_menu->GetMenu()->AddMenuItem(array_ordered[i]->fname, new ActionMenuWithParameter<const char*>(this, load_action, array_ordered[i]->fname) );
      
      i++;
   }
   logger_->Write("Menu", LogNotice, "Insert Media : End of Menu creation");
   file_menu->ResetMenu ();

   IAction::ActionReturn return_value = file_menu->DoScreen (this);
   logger_->Write("Menu", LogNotice, "file_menu->DoScreen : %i", return_value);

   delete file_menu;
   delete [] array_ordered;
   for (auto& it:cartridge_list)
   {
      delete it;
   }
   Window::SetFocus(focus);
   main_menu_->Invalidate ();

   logger_->Write("Menu", LogNotice, "Return from InsertMedia : %i", return_value);
   return return_value;
}

IAction::ActionReturn ScreenMenu::InsertCartridge()
{
   // List cartridge available
   // Show contents of root directory
   return InsertMedia (PATH_CARTIRDGE, &ScreenMenu::LoadCartridge);
}

IAction::ActionReturn ScreenMenu::InsertDisk()
{
   // List cartridge available
   // Show contents of root directory
   return InsertMedia (PATH_DISK, &ScreenMenu::LoadDisk);
}

IAction::ActionReturn ScreenMenu::InsertTape()
{
   // List cartridge available
   // Show contents of root directory
   return InsertMedia (PATH_TAPE, &ScreenMenu::LoadTape);
}

IAction::ActionReturn ScreenMenu::SugarSetup()
{
   Window* focus = Window::GetFocus();
   MainMenuWindows* setup_menu = new MainMenuWindows (display_->GetMenuFrame());

   setup_menu->GetMenu()->AddMenuItem("..", new ActionMenu( this, &ScreenMenu::Back) );

   // Add Synchro menu
   bool sync = display_-> IsSyncOnFrame();   
   setup_menu->GetMenu()->AddCheckMenuItem ( "Set synchro on Frame", &sync,  new ActionMenuWithParameter<bool*>(this, &ScreenMenu::SetSync, &sync));

   setup_menu->ResetMenu ();
   IAction::ActionReturn return_value = setup_menu->DoScreen(this);
   delete setup_menu;

   Window::SetFocus(focus);
   main_menu_->Invalidate ();

   return return_value;
}

IAction::ActionReturn ScreenMenu::HardwareSetup()
{
   logger_->Write("Menu", LogNotice, "ACTION : Select setup");
   return IAction::Action_None;
}

IAction::ActionReturn ScreenMenu::Save()
{
   snapshot_->SaveSnapshot(PATH_QUICK_SNA);
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::Load()
{
   snapshot_->LoadSnapshot(PATH_QUICK_SNA);
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::Reset()
{
   motherboard_->OnOff();
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::Info()
{
   Window* focus = Window::GetFocus();
   MainMenuWindows* setup_menu = new MainMenuWindows(display_->GetMenuFrame());

   setup_menu->GetMenu()->AddMenuItem("Exit", new ActionMenu(this, &ScreenMenu::Back));

   setup_menu->ResetMenu();
   IAction::ActionReturn return_value = setup_menu->DoScreen(this);
   delete setup_menu;

   Window::SetFocus(focus);
   main_menu_->Invalidate();

   return return_value;
}

IAction::ActionReturn ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   return IAction::Action_QuitMenu;
}

IEvent::Event ScreenMenu::GetEvent()
{
   IEvent::Event event = IEvent::NONE;

   if (keyboard_->IsButton(TGamePadButton::GamePadButtonDown))
   {
      event = IEvent::DOWN;
   }
   else if (keyboard_->IsButton(TGamePadButton::GamePadButtonUp))
   {
      event = IEvent::UP;
   }
   else if (keyboard_->IsAction())
   {
      event = IEvent::SELECT;
   }   
   else if (keyboard_->IsButton(TGamePadButton::GamePadButtonLeft))
   {
      event = IEvent::LEFT;
   }   
   return event;
}

void ScreenMenu::ForceStop()
{
   main_menu_->ForceStop();
}

IAction::ActionReturn ScreenMenu::Handle()
{
   logger_->Write("Menu", LogNotice, "MENU ENTER");

   // Minimize emulation
   display_->GetEmulationFrame()->Minimize();

   IAction::ActionReturn action = IAction::Action_None;
   keyboard_->ClearBuffer();
   display_->SetFullResolution(true);

   // Wait till next vsync
   main_menu_->ClearAll();
   //display_->VSync();

   // Reset menu
   logger_->Write("Menu", LogNotice, "Reset menu...");
   main_menu_->ResetMenu ();

   // Display menu
   logger_->Write("Menu", LogNotice, "Do screen...");
   main_menu_->DoScreen (this);

   logger_->Write("Menu", LogNotice, "MENU EXITING !");
   display_->GetEmulationFrame()->Maximize();

   display_->SetFullResolution(false);
   display_->VSync();

   return action;
}
