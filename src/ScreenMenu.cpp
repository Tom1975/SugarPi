//
#include "ScreenMenu.h"

#include <memory.h>
#include <math.h> 

#include "files.h"
#include "SimpleBitmap.h"

#include "MenuButtonWithBitmapWindows.h"

#ifdef WIN32
   #define PROFILE

   #ifdef PROFILE
      #include <profileapi.h>
      #define START_CHRONO  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);;QueryPerformanceCounter ((LARGE_INTEGER*)&s1);
      #define STOP_CHRONO   QueryPerformanceCounter ((LARGE_INTEGER*)&s2);t=(DWORD)(((s2 - s1) * 1000000) / freq);
      #define PROF_DISPLAY sprintf(s, "Duree Chargement descriptions: %d us\n", t);OutputDebugString (s);
      static __int64 s1, s2, freq;
      static DWORD t;
      static char s[1024];
   #else
      #define START_CHRONO  
      #define STOP_CHRONO   
      #define PROF_DISPLAY 

   #endif
#else
      #define START_CHRONO  
      #define STOP_CHRONO   
      #define PROF_DISPLAY 

#endif

#ifdef  __circle__
#include <strings.h>
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

#define MAX_ITEM_PER_PAGE 10
#define MOVE_BASE 7

#define MAX_SIZE_BUFFER 256
#define INTERLINE_SPACE 40

#define OFFSET_SUBMENU_X 120
#define OFFSET_SUBMENU_Y 60

////////////////////////////////////////////////////////////////////////////////////
// Menu description
////////////////////////////////////////////////////////////////////////////////////
std::vector<IAction::ActionReturn(ScreenMenu::*)()> function_menu =
{
  &ScreenMenu::Handle,
  &ScreenMenu::SelectAmstrad,
  &ScreenMenu::InsertCartridge,
  &ScreenMenu::InsertDisk,
  &ScreenMenu::InsertTape,
  &ScreenMenu::SugarSetup,
  &ScreenMenu::ChangeLanguage,
  &ScreenMenu::Reset,
};

ScreenMenu::MenuItem base_menu[] =
{
   { "MENU_Resume",             &ScreenMenu::Resume},
   { "MENU_Select_Amstrad",     &ScreenMenu::SelectAmstrad},
   { "MENU_Insert_Cartridge",   &ScreenMenu::InsertCartridge},
   { "MENU_Insert_Disk",        &ScreenMenu::InsertDisk},
   { "MENU_Insert_Tape",        &ScreenMenu::InsertTape},
   { "MENU_SugarPi_Setup",      &ScreenMenu::SugarSetup},
   { "MENU_Select_Language",      &ScreenMenu::ChangeLanguage},
/*   { "Hardware Setup",     &ScreenMenu::HardwareSetup},
   { "Quick Save",         &ScreenMenu::Save},
   { "Quick Load",         &ScreenMenu::Load},*/
   { "MENU_Reset",              &ScreenMenu::Reset},
   //{ "MENU_Info",              &ScreenMenu::Info},
   { "MENU_Shutdown",           &ScreenMenu::ShutDown},
   { nullptr, nullptr}
};

////////////////////////////////////////////////////////////////////////////////////
// Configuration list
////////////////////////////////////////////////////////////////////////////////////
std::vector<ScreenMenu::AmstradConfiguration> ScreenMenu::config_list =
{
   { "CPC 464", "", PATH_RES INTER_FILE "Img464.bin",
      {
         {"English", "CPC464UK.cfg", ""},
         {"French", "CPC464FR.cfg", ""},
         {"Spanish", "CPC464SP.cfg", ""},
         {"Danish", "CPC464DK.cfg", ""},
      }
   },
  { "CPC 664", "", PATH_RES INTER_FILE "Img664.bin", {
         {"English", "CPC664UK.cfg", ""},
         }
  },
  { "CPC 6128", "", PATH_RES INTER_FILE "Img6128.bin", {
         {"English", "CPC6128UK.cfg", ""},
         {"French", "CPC6128FR.cfg", ""},
         {"Spanish", "CPC6128SP.cfg", ""},
         {"Danish", "CPC6128DK.cfg", ""},
         }
  },
  { "GX 4000", "", PATH_RES INTER_FILE "ImgGx4000.bin", {
         {"-", "GX4000.cfg", ""},
         }
  },
  { "464 plus", "", PATH_RES INTER_FILE "Img464_plus.bin", {
         {"English", "CPC464PLUSEN.cfg", ""},
         {"French", "CPC464PLUSFR.cfg", ""},
         }
  },
  { "6128 plus", "", PATH_RES INTER_FILE "Img6128_plus.bin", {
         {"English", "CPC6128PLUSEN.cfg", ""},
         {"French", "CPC6128PLUSFR.cfg", ""},
         }
  },

 };

////////////////////////////////////////////////////////////////////////////////////
// ctor / dtor
////////////////////////////////////////////////////////////////////////////////////
ScreenMenu::ScreenMenu(IEngine* engine, ILog* log, CLogger* logger, DisplayPi* display, SoundMixer* sound_mixer, KeyboardPi* keyboard, Motherboard* motherboard, SugarPiSetup* setup, MultiLanguage* language) :
   language_(language),
   engine_(engine),
   logger_(logger),
   display_(display),
   sound_mixer_(sound_mixer),
   keyboard_(keyboard),
   setup_(setup),
   motherboard_(motherboard),
   current_menu_(nullptr),
   snapshot_(nullptr),
   main_menu_(nullptr)
{
   snapshot_ = new CSnapshot(log);
   snapshot_->SetMachine(motherboard_);
   /////////////////////////////////////////////////
   // Window creation

   // Create Main window menu 
   Reload();
}

ScreenMenu::~ScreenMenu()
{
   delete snapshot_;
   delete main_menu_;
  
}

void FillShortDescription(std::string& description, ConfigurationManager& cfg)
{
   // Add Full description
   int hardware_type = cfg.GetConfigurationInt("Hardware", "Type", 0);
   switch (hardware_type)
   {
   case 0: description.append("CPC 464\n"); break;
   case 1: description.append("CPC 664\n"); break;
   case 2: description.append("CPC 6128\n"); break;
   case 3: description.append("Amstrad 464 PLUS\n"); break;
   case 4: description.append("Amstrad 6128 PLUS\n"); break;
   case 5: description.append("GX 4000\n"); break;
   }
   

   // Add RAM size
   int ram = cfg.GetConfigurationInt("Memory", "RAM", 0);
   switch (ram)
   {
   case 0:
      description.append("RAM : 64 Ko\n"); 
      break;
   case 1:
      description.append("RAM : 128 Ko\n");
      break;
   case 0xFF:
      description.append("RAM : 512 Ko\n");
      break;
   }

   // Add CRTC Type
   int type_crtc = cfg.GetConfigurationInt("Hardware", "Type_CRTC", 0);
   description.append("CRTC Type : ");
   description.append(std::to_string(type_crtc));
   description.append("\n");

   // Add Disk reader presence
   description.append("Disk Drive : ");
   description.append( (cfg.GetConfigurationInt("Hardware", "FDC", 1) == 1) ? "YES" : "NO");
   description.append("\n");

}

void ScreenMenu::LoadDescriptions()
{
   START_CHRONO

   char tmp_buffer[128];

   // Parse each AmstradConfiguration element, and generate a description
   for (auto& it : config_list)
   {
      // Load bitmap 
      it.associatedBmp_.Init();

      CString fullpath = PATH_CONFIGS;
      fullpath.Append(INTER_FILE);
      fullpath.Append(it.languages_[0].config_path_);

      ConfigurationManager cfg;
      cfg.OpenFile(fullpath);
      FillShortDescription(it.description_, cfg);

      for (auto& it2 : it.languages_)
      {
         fullpath = PATH_CONFIGS;
         fullpath.Append(INTER_FILE);
         fullpath.Append(it2.config_path_);

         ConfigurationManager cfg;
         cfg.OpenFile(fullpath);
         it2.description_ = it.description_;

         cfg.GetConfiguration("Keyboard", "Type", "", tmp_buffer, sizeof(tmp_buffer));
         it2.description_.append("Keyboard type : ");
         it2.description_.append(tmp_buffer);
         //cfg.CloseFile();

      }
   }

   STOP_CHRONO
   PROF_DISPLAY
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void ScreenMenu::LauchMenu(unsigned int function)
{
   if (function > 0 && function <= function_menu.size())
   {
      // Get proper menu
      IAction::ActionReturn(ScreenMenu:: * menu)() = function_menu[function];

      logger_->Write("Menu", LogNotice, "MENU ENTER");

      // Minimize emulation
      display_->GetEmulationFrame()->Minimize();

      IAction::ActionReturn action = IAction::Action_None;
      keyboard_->ClearBuffer();

      // Wait till next vsync
      (this->*menu)();

      logger_->Write("Menu", LogNotice, "MENU EXITING !");
      display_->GetEmulationFrame()->Maximize();

      display_->VSync();
   }
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void ScreenMenu::Reload()
{
   unsigned int i = 0;
   if (main_menu_ == nullptr)
   {
      main_menu_ = new MainMenuWindows(display_->GetMenuFrame());
   }

   while (base_menu[i].label_ != nullptr && i < MAX_ITEM_PER_PAGE)
   {
      // Display menu bitmap
      MenuItemWindows* item = main_menu_->GetMenu()->GetMenuItem(i);
      if (item == nullptr)
      {
         item = main_menu_->GetMenu()->AddMenuItem(language_->GetString(base_menu[i].label_),
            10, i* INTERLINE_SPACE, main_menu_->GetMenu()->GetWidth()-10, INTERLINE_SPACE - 2, new ActionMenu(this, base_menu[i].function));
      }
      else
      {
         item->ChangeLabel(language_->GetString(base_menu[i].label_));
      }
      i++;
   }
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::SetLanguage(int value)
{
   language_->ChangeLanguage(value);
   return IAction::Action_Back;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::SetSync(bool* value)
{
   setup_->SetSync (*value ? SugarPiSetup::SYNC_FRAME:SugarPiSetup::SYNC_SOUND);
   setup_->Save();
   return IAction::Action_Update;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Back()
{
   return IAction::Action_Back;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Resume()
{
   return IAction::Action_QuitMenu;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Select amstrad function:
/// 
/// This functions will allow an easy selection of the precise Amstrad to Emulate.
/// First, user can choose between : 
///   - Amstrad CPC 464
///   - Amstrad CPC 664
///   - Amstrad CPC 6128
///   - Amstrad 664 PLUS
///   - Amstrad 6128 PLUS
///   - GX4000
/// 
/// Then, for each computer (464, 664, 6128), the origin language will have to be chosen.
/// We so have the following structure :
/// 
/// 
/// Next, addons will be added / removed through anoter menu
/// 
/// </summary>
/// <returns></returns>
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::SelectAmstrad()
{
   // Create menu from config_list 
   Window* focus = Window::GetFocus();

   MainMenuWindows* cfg_menu = new MainMenuWindows(display_->GetMenuFrame());

   cfg_menu->GetMenu()->AddMenuItem("..", 10, 0, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2
      , new ActionMenu(this, &ScreenMenu::Back));

   int offset_y = INTERLINE_SPACE + 2;
   for (auto& it : config_list)
   {
      MenuButtonWithBitmapWindows* item = new MenuButtonWithBitmapWindows(display_->GetMenuFrame());
      item->Create(it.name_.c_str(), it.description_.c_str(), & it.associatedBmp_, cfg_menu->GetMenu()->GetScrollWindow(), 10, offset_y,
         400, INTERLINE_SPACE,
         410, INTERLINE_SPACE + 2 , 800, 450);
      item->SetAction(new ActionMenuWithParameter<ScreenMenu::AmstradConfiguration&>(this, &ScreenMenu::SelectAmstradFinal, it));

      cfg_menu->GetMenu()->AddMenuItem(item);
         

      offset_y += INTERLINE_SPACE +2;
   }

   cfg_menu->ResetMenu();

   current_menu_ = cfg_menu;
   IAction::ActionReturn return_value = cfg_menu->DoScreen(this);

   logger_->Write("Menu", LogNotice, "file_menu->DoScreen : %i", return_value);

   delete cfg_menu;
   Window::SetFocus(focus);
   main_menu_->Invalidate();

   logger_->Write("Menu", LogNotice, "Return from SelectAmstrad : %i", return_value);

   return return_value;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::SelectAmstradFinal(ScreenMenu::AmstradConfiguration& config)
{
   // Create submenu
   // Display the current name / associated bitmap
   // Display a menu with all the languages
   Window* focus = Window::GetFocus();
   MainMenuWindows* config_menu = new MainMenuWindows(display_->GetMenuFrame());

   config_menu->GetMenu()->AddMenuItem("..", OFFSET_SUBMENU_X, OFFSET_SUBMENU_Y, 280, INTERLINE_SPACE - 2
      , new ActionMenu(this, &ScreenMenu::Back));

   // Add Synchro menu

   int offset_y = OFFSET_SUBMENU_Y + INTERLINE_SPACE + 2;
   int i = 0;
   for (auto& it : config.languages_)
   {
      MenuButtonWithBitmapWindows* item = new MenuButtonWithBitmapWindows(display_->GetMenuFrame());
      item->Create(it.fullname_.c_str(), it.description_.c_str(), &config.associatedBmp_, config_menu->GetMenu()->GetScrollWindow(), OFFSET_SUBMENU_X, offset_y,
         290, INTERLINE_SPACE,
         410, INTERLINE_SPACE + 2, 800, 450);
      item->SetAction(new ActionMenuWithParameter<const char*>(this, &ScreenMenu::LoadAmstradSetup, it.config_path_));

      config_menu->GetMenu()->AddMenuItem(item);


      offset_y += INTERLINE_SPACE + 2;
   }

   config_menu->ResetMenu(1); // By default, select first item
   IAction::ActionReturn return_value = config_menu->DoScreen(this);
   delete config_menu;

   Window::SetFocus(focus);
   current_menu_->Invalidate();

   return return_value;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::SelectAmstradCustom()
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
   delete FileInfo;

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

   file_menu->GetMenu()->AddMenuItem("..", 10, i * INTERLINE_SPACE, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2
      , new ActionMenu(this, &ScreenMenu::Back));

   for (unsigned int i = 0; i < nb_file_ordered; i++)
   {
      // Display menu bitmap
      file_menu->GetMenu()->AddMenuItem(array_ordered[i]->fname, 10, (i + 1) * INTERLINE_SPACE, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2,
         new ActionMenuWithParameter<const char*>(this, &ScreenMenu::LoadAmstradSetup, array_ordered[i]->fname));
      
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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
      else
      {
         delete FileInfo;
      }
      FileInfo = new FILINFO;

      Result = f_findnext(&Directory, FileInfo);
   }
   delete FileInfo;
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

   file_menu->GetMenu()->AddMenuItem("..", 10, 0, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2
      , new ActionMenu(this, &ScreenMenu::Back));

   for (unsigned int i = 0; i < nb_file_ordered; i++)
   {
      // Display menu bitmap
      file_menu->GetMenu()->AddMenuItem(array_ordered[i]->fname, 10, (i + 1) * INTERLINE_SPACE, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2
         , new ActionMenuWithParameter<const char*>(this, load_action, array_ordered[i]->fname));
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::InsertCartridge()
{
   // List cartridge available
   // Show contents of root directory
   return InsertMedia (PATH_CARTIRDGE, &ScreenMenu::LoadCartridge);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::InsertDisk()
{
   // List cartridge available
   // Show contents of root directory
   return InsertMedia (PATH_DISK, &ScreenMenu::LoadDisk);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::InsertTape()
{
   // List cartridge available
   // Show contents of root directory
   return InsertMedia (PATH_TAPE, &ScreenMenu::LoadTape);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::ChangeLanguage()
{
   Window* focus = Window::GetFocus();
   MainMenuWindows* setup_menu = new MainMenuWindows(display_->GetMenuFrame());

   setup_menu->GetMenu()->AddMenuItem("..", 10, 0, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2, new ActionMenu(this, &ScreenMenu::Back));

   // Add Synchro menu
   int nb_language = language_->GetLanguageNumber();

   for (int i = 0; i < nb_language; i++)
   {
      auto str = language_->GetLanguage(i);
      setup_menu->GetMenu()->AddMenuItem(str, 10, (i + 1) * INTERLINE_SPACE, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2
         , new ActionMenuWithParameter<int>(this, &ScreenMenu::SetLanguage, i));
   }

   setup_menu->ResetMenu();
   IAction::ActionReturn return_value = setup_menu->DoScreen(this);
   delete setup_menu;

   Reload();
   Window::SetFocus(focus);
   main_menu_->Invalidate();

   return return_value;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::SugarSetup()
{
   Window* focus = Window::GetFocus();
   MainMenuWindows* setup_menu = new MainMenuWindows (display_->GetMenuFrame());

   setup_menu->GetMenu()->AddMenuItem("..", 10, 0, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2
      , new ActionMenu(this, &ScreenMenu::Back));

   // Add Synchro menu
   bool sync = display_-> IsSyncOnFrame();   
   int offset_y = 60 + 2;
   setup_menu->GetMenu()->AddCheckMenuItem("Set synchro on Frame", &sync,
      10, offset_y, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2,
      new ActionMenuWithParameter<bool*>(this, &ScreenMenu::SetSync, &sync));

   setup_menu->ResetMenu ();
   IAction::ActionReturn return_value = setup_menu->DoScreen(this);
   delete setup_menu;

   Window::SetFocus(focus);
   main_menu_->Invalidate ();

   return return_value;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::HardwareSetup()
{
   logger_->Write("Menu", LogNotice, "ACTION : Select setup");
   return IAction::Action_None;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Save()
{
   snapshot_->SaveSnapshot(PATH_QUICK_SNA);
   return IAction::Action_QuitMenu;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Load()
{
   snapshot_->LoadSnapshot(PATH_QUICK_SNA);
   return IAction::Action_QuitMenu;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Reset()
{
   motherboard_->OnOff();
   return IAction::Action_QuitMenu;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Info()
{
   Window* focus = Window::GetFocus();
   MainMenuWindows* setup_menu = new MainMenuWindows(display_->GetMenuFrame());

   setup_menu->GetMenu()->AddMenuItem(language_->GetString("MENU_Exit"), 
      10, 0, main_menu_->GetMenu()->GetWidth() - 10, INTERLINE_SPACE - 2,
      new ActionMenu(this, &ScreenMenu::Back));

   setup_menu->ResetMenu();
   IAction::ActionReturn return_value = setup_menu->DoScreen(this);
   delete setup_menu;

   Window::SetFocus(focus);
   main_menu_->Invalidate();

   return return_value;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   return IAction::Action_QuitMenu;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IEvent::Event ScreenMenu::GetEvent()
{
   IEvent::Event event = IEvent::NONE;

   if (keyboard_->IsButton(TGamePadButton::GamePadButtonDown))
   {
      logger_->Write("Menu", LogNotice, "ACTION : DOWN");
      event = IEvent::DOWN;
   }
   else if (keyboard_->IsButton(TGamePadButton::GamePadButtonUp))
   {
      logger_->Write("Menu", LogNotice, "ACTION : UP");
      event = IEvent::UP;
   }
   else if (keyboard_->IsAction())
   {
      logger_->Write("Menu", LogNotice, "ACTION : SELECT");
      event = IEvent::SELECT;
   }   
   else if (keyboard_->IsButton(TGamePadButton::GamePadButtonLeft))
   {
      event = IEvent::LEFT;
   }   
   return event;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void ScreenMenu::ForceStop()
{
   main_menu_->ForceStop();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
IAction::ActionReturn ScreenMenu::Handle()
{
   /*logger_->Write("Menu", LogNotice, "MENU ENTER");

   // Minimize emulation
   display_->GetEmulationFrame()->Minimize();

   IAction::ActionReturn action = IAction::Action_None;
   keyboard_->ClearBuffer();
   */
   // Wait till next vsync
   Reload();
   main_menu_->ClearAll();
   //display_->VSync();

   // Reset menu
   logger_->Write("Menu", LogNotice, "Reset menu...");
   main_menu_->ResetMenu ();

   // Display menu
   logger_->Write("Menu", LogNotice, "Do screen...");
   IAction::ActionReturn action = main_menu_->DoScreen (this);
   /*
   logger_->Write("Menu", LogNotice, "MENU EXITING !");
   display_->GetEmulationFrame()->Maximize();

   display_->VSync();
   */
   return action;
}
