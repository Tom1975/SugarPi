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

ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",             &ScreenMenu::Resume},
   { "Insert Cartridge",   &ScreenMenu::InsertCartridge},
   { "SugarPi Setup",      &ScreenMenu::SugarSetup},
   { "Hardware Setup",     &ScreenMenu::HardwareSetup},
   { "Quick Save",         &ScreenMenu::Save},
   { "Quick Load",         &ScreenMenu::Load},
   { "Reset",              &ScreenMenu::Reset},
   { "Shutdown",           &ScreenMenu::ShutDown},
   { nullptr, nullptr}
};

ScreenMenu::ScreenMenu(ILog* log, CLogger* logger, DisplayPi* display, SoundMixer* sound_mixer, KeyboardPi* keyboard, Motherboard* motherboard, SugarPiSetup* setup) :
   logger_(logger),
   display_(display),
   sound_mixer_(sound_mixer),
   keyboard_(keyboard),
   //BaseMenu(logger),
   current_menu_(base_menu),
   selected_(0),
   index_base_(0),
   motherboard_(motherboard),
   snapshot_(nullptr),
   setup_(setup)
{
   font_ = new CoolspotFont(logger_);
   snapshot_ = new CSnapshot(log);
   snapshot_->SetMachine(motherboard_);
   sugarpi_setup_menu_ = new MenuItem[3];
   sugarpi_setup_menu_[0] =  { "...Back",             &ScreenMenu::Resume};
   sugarpi_setup_menu_[2] = {nullptr, nullptr};
}

ScreenMenu::~ScreenMenu()
{
   delete snapshot_;
   delete []sugarpi_setup_menu_;
}

int ScreenMenu::SetSyncVbl()
{
   setup_->SetSync (SugarPiSetup::SYNC_FRAME);
   setup_->Save();
   BuildMenuSync (&sugarpi_setup_menu_[1]);
}

int ScreenMenu::SetSyncSound()
{
   setup_->SetSync (SugarPiSetup::SYNC_SOUND);
   setup_->Save();
   BuildMenuSync (&sugarpi_setup_menu_[1]);
}

int ScreenMenu::Resume()
{
   resume_ = true;
   return 0;
}

int ScreenMenu::InsertCartridge()
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

   ScreenMenu::MenuItem* submenu = new ScreenMenu::MenuItem[cartridge_list.size()+2];
   submenu[0].function = nullptr;
   submenu[0].label_ = "...Back";

   for (int i = 1; i < cartridge_list.size(); i++)
   {
      submenu[i].function= nullptr;
      submenu[i].label_ = cartridge_list[i]->fname;
      logger_->Write("Menu", LogNotice, "Added %s", cartridge_list[i]->fname);
   }
   logger_->Write("Menu", LogNotice, "Loop ended");
   submenu[cartridge_list.size()].function = nullptr;
   submenu[cartridge_list.size()].label_= nullptr;

   // Display menu !
   MenuItem* old_menu = current_menu_;
   selected_ = 0;
   unsigned int old_index = index_base_;
   index_base_ = 0;

   current_menu_ = submenu;

   logger_->Write("Menu", LogNotice, "Will now display submenu");
   DisplayMenu(current_menu_);
   logger_->Write("Menu", LogNotice, "Will submenu displayed");

   // wait for command
//   while (!keyboard_->IsAction());
   keyboard_->ReinitSelect();
   bool end_menu = false;
   while (end_menu == false)
   {
      // Any key pressed ?
      if (keyboard_->IsDown())
      {
         Down();
         logger_->Write("Menu", LogNotice, "Selection down %i", selected_);
      }
      if (keyboard_->IsUp())
      {
         Up();
         logger_->Write("Menu", LogNotice, "Selection up %i", selected_);
      }
      if (keyboard_->IsAction())
      {
         logger_->Write("Menu", LogNotice, "Selection  : %i ", selected_);
         // If 0 : end; otherwise, load menu
         if (selected_ == 0)
         {
            end_menu = true;
         }
         else
         {

            CString fullpath = PATH_CARTIRDGE;
            fullpath.Append( "/" );
            fullpath.Append( cartridge_list[selected_]->fname);
            logger_->Write("Menu", LogNotice, "Load cartridge fullpath : %s", (const char*)fullpath);
            setup_->LoadCartridge (fullpath);
            setup_->Save();
            logger_->Write("Cartridge", LogNotice, "file loaded.Exiting menu");

            end_menu = true;
            motherboard_->OnOff();
            resume_ = true;
         }
      }

   }
   logger_->Write("Cartridge", LogNotice, "Exiting loop...");
   delete []submenu;

   logger_->Write("Cartridge", LogNotice, "deleting cartridge_list...");

   for (int i = 0; i < cartridge_list.size(); i++)
   {
      delete cartridge_list[i];
   }
   logger_->Write("Cartridge", LogNotice, "deleting cartridge_list done !");
      
   index_base_ = old_index;
   current_menu_ = old_menu;
   selected_ = 0;
   return 0;
}

void ScreenMenu::BuildMenuSync(MenuItem * sync_menu)
{
   
   if (display_-> IsSyncOnFrame())
   {
      *sync_menu =  { "Set synchro on Frame [X]",   &ScreenMenu::SetSyncSound};
   }
   else
   {
      *sync_menu =  { "Set synchro on Frame [ ]",   &ScreenMenu::SetSyncVbl};
      
   }
}

int ScreenMenu::SugarSetup()
{
   BuildMenuSync (&sugarpi_setup_menu_[1]);
  
   HandleMenu (sugarpi_setup_menu_);
}

int ScreenMenu::HandleMenu( MenuItem* menu)
{
   logger_->Write("Menu", LogNotice, "ACTION : Select Sugarpi setup");

   // Display menu !
   MenuItem* old_menu = current_menu_;
   selected_ = 0;
   unsigned int old_index = index_base_;
   index_base_ = 0;

   current_menu_ = menu;

   logger_->Write("Menu", LogNotice, "Will now display submenu");
   DisplayMenu(current_menu_);
   logger_->Write("Menu", LogNotice, "Will submenu displayed");

   // wait for command
   keyboard_->ReinitSelect();
   bool end_menu = false;
   while (end_menu == false)
   {
      // Any key pressed ?
      if (keyboard_->IsDown())
      {
         Down();
         logger_->Write("Menu", LogNotice, "Selection down %i", selected_);
      }
      if (keyboard_->IsUp())
      {
         Up();
         logger_->Write("Menu", LogNotice, "Selection up %i", selected_);
      }
      if (keyboard_->IsAction())
      {
         Select();
         logger_->Write("Menu", LogNotice, "Selection  : %i ", selected_);
         if (selected_ == 0)
         {
            end_menu = true;
         }         
      }
   }

   index_base_ = old_index;
   current_menu_ = old_menu;
   selected_ = 0;

   return 0;
}


int ScreenMenu::HardwareSetup()
{
   logger_->Write("Menu", LogNotice, "ACTION : Select setup");
   return 0;
}

int ScreenMenu::Save()
{
   snapshot_->SaveSnapshot(PATH_QUICK_SNA);
   resume_ = true;
   return 0;
}

int ScreenMenu::Load()
{
   snapshot_->LoadSnapshot(PATH_QUICK_SNA);
   resume_ = true;
   return 0;
}

int ScreenMenu::Reset()
{
   motherboard_->OnOff();
   resume_ = true;
   return 0;
}

int ScreenMenu::ShutDown()
{
   logger_->Write("Menu", LogNotice, "ACTION : SHUTDOWN");
   return 0;
}

void ScreenMenu::DisplayText(const char* txt, int x, int y, bool selected)
{
   // Display text
   int i = 0;

   char buff[16];
   memset(buff, 0, sizeof buff);
   strncpy(buff, txt, 15);
   
   unsigned int x_offset_output = 0;

   while (txt[i] != '\0' )
   {

      // Display character
      unsigned char c = txt[i];

      if ( c == ' ')
      {
         x_offset_output += 10;
      }
      else
      {
         // Look for proper bitmap position (on first line only)
         for (int display_y = 0; display_y < font_->GetLetterHeight(c); display_y++)
         {
            int* line = display_->GetVideoBuffer(display_y + y);
            font_->CopyLetter(c, display_y, &line[x + x_offset_output]);
         }
         x_offset_output += font_->GetLetterLength(c);
      }
      i++;
      
   }
}

void ScreenMenu::DisplayButton(MenuItem* menu, int x, int y, bool selected)
{
   int index_bmp = 0;

   // Display text
   if (selected)
   {
      DisplayText("*", x + 2, y , selected);
   }

   DisplayText(menu->label_, x + 20, y , selected);
}

void ScreenMenu::DisplayMenu(MenuItem* menu)
{
   // Background
   for (int i = 0; i < display_->GetHeight(); i++)
   {
      int* line = display_->GetVideoBuffer(i);
      memset(line, 0x0, sizeof(int) * display_->GetWidth());
   }

   DisplayText("SugarPi", 450, 47, false);
   unsigned int i = 0;

   while (menu[i + index_base_].label_ != nullptr && i < MAX_ITEM_PER_PAGE)
   {
      // Display menu bitmap
      DisplayButton(&menu[i + index_base_], 250, i * 20 + 70, selected_ == i+ index_base_);
    
      // log
      i++;
   }
   display_->VSync();
}

void ScreenMenu::Handle()
{
   display_->SetFullResolution(true);

   // Display menu
   DisplayMenu(current_menu_);


   // Reinit actions
   resume_ = false;
   keyboard_->ClearBuffer();

   // Handle it
   while (resume_ == false)
   {
      // Any key pressed ?
      if (keyboard_->IsDown())
      {
         logger_->Write("Menu", LogNotice, "Down");
         Down();
      }
      if (keyboard_->IsUp())
      {
         logger_->Write("Menu", LogNotice, "Up");
         Up();
      }
      if (keyboard_->IsAction())
      {
         logger_->Write("Menu", LogNotice, "Select");
         Select();
      }

   }
   logger_->Write("Menu", LogNotice, "MENU EXITING !");
   display_->SetFullResolution(false);
}

void ScreenMenu::Down()
{
   if (current_menu_[selected_ + 1].label_ != nullptr)
   {
      selected_++;
      if (selected_ > MOVE_BASE)
         index_base_++;
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Up()
{
   if (selected_ > 0)
   {
      selected_--;
      if (selected_ >= MOVE_BASE && index_base_ > 0)
         index_base_--;

   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Select()
{
   (this->*(current_menu_[selected_].function))();
   if (!resume_)
      DisplayMenu(current_menu_);
}

