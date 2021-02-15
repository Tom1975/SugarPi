//
#include "ScreenMenu.h"

#include <memory.h>

#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#include "res/button_1.h"
#include "res/coolspot.h"

#define MAX_ITEM_PER_PAGE 10
#define MOVE_BASE 7

#define DRIVE		"SD:"

#define PATH_CARTIRDGE "SD:/CART"
#define PATH_QUICK_SNA "SD:/quick.sna"


MainMenuWindows::MainMenuWindows (DisplayPi* display) : Windows (display)
{
   // Create Title bitmap
   // todo

   // Create inner menu
   menu_ = new MenuWindows (display);
   menu_->CreateWindow ( this, 240, 70, 1000, 800);
}

MainMenuWindows::~MainMenuWindows ()
{
   delete menu_;
}

void MainMenuWindows::ResetMenu()
{
   // Set focus to first item
   menu_->SetFocus(0);
}

ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",             &ScreenMenu::Resume},
   { "Insert Cartridge",   &ScreenMenu::InsertCartridge},
   { "SugarPi Setup",      &ScreenMenu::SugarSetup},
/*   { "Hardware Setup",     &ScreenMenu::HardwareSetup},
   { "Quick Save",         &ScreenMenu::Save},
   { "Quick Load",         &ScreenMenu::Load},*/
   { "Reset",              &ScreenMenu::Reset},
   { "Shutdown",           &ScreenMenu::ShutDown},
   { nullptr, nullptr}
};

ScreenMenu::ScreenMenu(ILog* log, CLogger* logger, DisplayPi* display, SoundMixer* sound_mixer, KeyboardPi* keyboard, Motherboard* motherboard, SugarPiSetup* setup) :
   logger_(logger),
   display_(display),
   sound_mixer_(sound_mixer),
   keyboard_(keyboard),
   setup_(setup),
   //BaseMenu(logger),
   current_menu_(base_menu),
   selected_(0),
   index_base_(0),
   motherboard_(motherboard),
   resume_(false), 
   snapshot_(nullptr)
{
   font_ = new CoolspotFont(logger_);
   snapshot_ = new CSnapshot(log);
   snapshot_->SetMachine(motherboard_);
   /////////////////////////////////////////////////
   // Windows creation

   // Create Main window menu 
   unsigned int i = 0;
   main_menu_ = new MainMenuWindows (display_);
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
   delete font_;

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
   resume_ = true;
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::LoadCartridge( const char* path)
{
   CString fullpath = PATH_CARTIRDGE;
   fullpath.Append( "/" );
   fullpath.Append( path);
   logger_->Write("Menu", LogNotice, "Load cartridge fullpath : %s", (const char*)fullpath);
   setup_->LoadCartridge (fullpath);
   setup_->Save();
   logger_->Write("Cartridge", LogNotice, "file loaded.Exiting menu");

   motherboard_->OnOff();
   resume_ = true;
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::InsertCartridge()
{
   logger_->Write("Menu", LogNotice, "ACTION : InsertCartridge. Sizeof FILINFO : %i", sizeof(FILINFO));

   // List cartridge available
   // Show contents of root directory
   DIR Directory;
   FILINFO *FileInfo = new FILINFO;
   FRESULT Result = f_findfirst(&Directory, FileInfo, PATH_CARTIRDGE, "*");
   std::vector<FILINFO*> cartridge_list(20);

   int limit = 0;
   // Create menu
   unsigned int i = 0;
   for (i = 0; Result == FR_OK && FileInfo->fname[0] ; i++)
   {
      limit++;
      if (!(FileInfo->fattrib & (AM_HID | AM_SYS)))
      {
         logger_->Write("Menu", LogNotice, "Added next %s", FileInfo->fname);
         cartridge_list.push_back(FileInfo);
      }
      FileInfo = new FILINFO;
      Result = f_findnext(&Directory, FileInfo);
   }

   // Alphabetical Order
   FILINFO** array_ordered = new FILINFO* [cartridge_list.size()];
   unsigned int nb_file_ordered = 0;
   
   for (auto& it:cartridge_list)
   {
      int place = nb_file_ordered;
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
      logger_->Write("Menu", LogNotice, "Added %s, here : %i", it->fname, place);
      nb_file_ordered++;
   }

   // Create selection menu
   Windows* focus = Windows::GetFocus();

   MainMenuWindows* file_menu = new MainMenuWindows (display_);

   file_menu->GetMenu()->AddMenuItem("..", new ActionMenu( this, &ScreenMenu::Back) );

   for (unsigned int i = 0; i < nb_file_ordered; i++)
   {
      // Display menu bitmap
      logger_->Write("Menu", LogNotice, "Added %s to menu", array_ordered[i]->fname);
      file_menu->GetMenu()->AddMenuItem(array_ordered[i]->fname, new ActionMenuWithParameter<const char*>(this, &ScreenMenu::LoadCartridge, array_ordered[i]->fname) );
      
      i++;
   }
   file_menu->ResetMenu ();

   IAction::ActionReturn return_value = file_menu->DoScreen (this);
   logger_->Write("Menu", LogNotice, "file_menu->DoScreen : %i", return_value);

   delete file_menu;
   delete [] array_ordered;
   for (auto& it:cartridge_list)
   {
      delete it;
   }
   Windows::SetFocus(focus);
   main_menu_->Invalidate ();

   logger_->Write("Menu", LogNotice, "Return from InsertCartridge : %i", return_value);
   return return_value;
}

IAction::ActionReturn ScreenMenu::SugarSetup()
{
   Windows* focus = Windows::GetFocus();
   MainMenuWindows* setup_menu = new MainMenuWindows (display_);

   setup_menu->GetMenu()->AddMenuItem("..", new ActionMenu( this, &ScreenMenu::Back) );

   // Add Synchro menu
   bool sync = display_-> IsSyncOnFrame();   
   setup_menu->GetMenu()->AddCheckMenuItem ( "Set synchro on Frame", &sync,  new ActionMenuWithParameter<bool*>(this, &ScreenMenu::SetSync, &sync));

   setup_menu->ResetMenu ();
   IAction::ActionReturn return_value = setup_menu->DoScreen(this);
   delete setup_menu;

   Windows::SetFocus(focus);
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
   resume_ = true;
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::Load()
{
   snapshot_->LoadSnapshot(PATH_QUICK_SNA);
   resume_ = true;
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::Reset()
{
   motherboard_->OnOff();
   resume_ = true;
   return IAction::Action_QuitMenu;
}

IAction::ActionReturn ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   resume_ = true;
   return IAction::Action_QuitMenu;
}

IEvent::Event ScreenMenu::GetEvent ()
{
   IEvent::Event event = IEvent::NONE;

   if (keyboard_->IsDown())
   {
      event = IEvent::DOWN;
   }
   if (keyboard_->IsUp())
   {
      event = IEvent::UP;
   }
   if (keyboard_->IsAction())
   {
      event = IEvent::SELECT;
   }   
   return event;
}

IAction::ActionReturn ScreenMenu::Handle()
{
   logger_->Write("Menu", LogNotice, "MENU ENTER");

   IAction::ActionReturn action = IAction::Action_None;
   keyboard_->ClearBuffer();
   display_->SetFullResolution(true);

   // Wait till next vsync
   display_->VSync();

   // Reset menu
   logger_->Write("Menu", LogNotice, "Reset menu...");
   main_menu_->ResetMenu ();

   // Display menu
   logger_->Write("Menu", LogNotice, "Do screen...");
   main_menu_->DoScreen (this);

   logger_->Write("Menu", LogNotice, "MENU EXITING !");
   display_->SetFullResolution(false);
   display_->VSync();

   return action;
}
