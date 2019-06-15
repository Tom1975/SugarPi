//
#include "ScreenMenu.h"


#include <memory.h>

#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#include "res\button_1.h"
#include "res\coolspot.h"


#define DRIVE		"SD:"

#define PATH_CARTIRDGE "SD:/CART"
static CoolspotFont font;

ScreenMenu::MenuItem base_menu[] =
{
   { "Resume",             &ScreenMenu::Resume},
   { "Insert Cartridge",   &ScreenMenu::InsertCartridge},
   { "Hardware Setup",     &ScreenMenu::HardwareSetup},
   { "Reset",              &ScreenMenu::Reset},
   { "Shutdown",           &ScreenMenu::ShutDown},
   { nullptr, nullptr}
};

ScreenMenu::ScreenMenu(CLogger* logger, DisplayPi* display, KeyboardPi* keyboard, Motherboard* motherboard) :
   logger_(logger),
   display_(display),
   keyboard_(keyboard),
   //BaseMenu(logger),
   current_menu_(base_menu),
   selected_(0),
   motherboard_(motherboard)
{
}

ScreenMenu::~ScreenMenu()
{
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

   ScreenMenu::MenuItem* submenu = new ScreenMenu::MenuItem[cartridge_list.size()+1];
   for (int i = 0; i < cartridge_list.size(); i++)
   {
      logger_->Write("Menu", LogNotice, "Added next ");
      submenu[i].function= nullptr;
      submenu[i].label_ = cartridge_list[i]->fname;
      logger_->Write("Menu", LogNotice, "Added %s", cartridge_list[i]->fname);
   }
   submenu[cartridge_list.size()].function = nullptr;
   submenu[cartridge_list.size()].label_= nullptr;

   // Display menu !
   MenuItem* old_menu = current_menu_;
   selected_ = 0;

   current_menu_ = submenu;
   DisplayMenu(current_menu_);

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
            LoadCprFromBuffer(fullpath);
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
      

   current_menu_ = old_menu;
   selected_ = 0;
   return 0;
}

int ScreenMenu::HardwareSetup()
{
   logger_->Write("Menu", LogNotice, "ACTION : InsertCartridge");
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
      unsigned char c = buff[i];

      // Look for proper bitmap position (on first line only)
      for (int display_y = 0; display_y < font.GetLetterHeight(c); display_y++)
      {
         int* line = display_->GetVideoBuffer(display_y + y);
         font.CopyLetter(c, display_y, &line[x + x_offset_output], logger_);
      }
      x_offset_output += font.GetLetterLength(c);

      i++;
      
   }
}

void ScreenMenu::DisplayButton(MenuItem* menu, int x, int y, bool selected)
{
   int index_bmp = 0;
   /*for (int j = y; j < y + Button_1_h; j++)
   {
      int* line = display_->GetVideoBuffer(j);
      for (int i = x; i < x + Button_1_w; i++)
      {
         if (selected)
         {
            line[i] = Button_1[index_bmp] << 8
               | Button_1[index_bmp + 1] << 16
               | Button_1[index_bmp + 2] ;
         }
         else
         {
            line[i] = Button_1[index_bmp] << 8
               | Button_1[index_bmp + 1]
               | Button_1[index_bmp + 2] << 16;
         }
         index_bmp += 3;
      }
   }*/
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
      for (int x = 0; x < display_->GetWidth(); x++)
      {
         line[x] = ((i << 6)&0xFF00) | ((x << 14)&0xFF0000);
      }
   }

   DisplayText("SugarPi", 450, 47, false);

   unsigned int i = 0;

   //for (unsigned int i = 0; i < menu->items.size(); i++)
   while (menu[i].label_ != nullptr)
   {
      logger_->Write("Menu", LogNotice, "Insert : %s", menu[i].label_);

      // Display menu bitmap
      DisplayButton(&menu[i], 250, i * 20 + 70, selected_ == i);
    
      // log
      i++;
   }
}

void ScreenMenu::Handle()
{
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
         Down();
      }
      if (keyboard_->IsUp())
      {
         Up();
      }
      if (keyboard_->IsAction())
      {
         Select();
      }

   }
   logger_->Write("Menu", LogNotice, "MENU EXITING !");
}

void ScreenMenu::Down()
{
   if (current_menu_[selected_ + 1].label_ != nullptr)
   {
      selected_++;
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Up()
{
   if (selected_ > 0)
   {
      selected_--;
   }
   DisplayMenu(current_menu_);
}

void ScreenMenu::Select()
{
   (this->*(current_menu_[selected_].function))();
   if (!resume_)
      DisplayMenu(current_menu_);
}



int ScreenMenu::LoadCprFromBuffer(const char* filepath)
{
   FIL File;
   FRESULT Result = f_open(&File, filepath, FA_READ | FA_OPEN_EXISTING);
   if (Result != FR_OK)
   {
      logger_->Write("Cartridge", LogPanic, "Cannot open file: %s", filepath);
   }
   else
   {
      logger_->Write("Cartridge", LogNotice, "File opened correctly");
   }

   FILINFO file_info;
   f_stat(filepath, &file_info);
   logger_->Write("Cartridge", LogNotice, "File size : %i", file_info.fsize);
   unsigned char* buffer = new unsigned char[file_info.fsize];
   unsigned nBytesRead;

   logger_->Write("Cartridge", LogNotice, "buffer allocated");
   f_read(&File, buffer, file_info.fsize, &nBytesRead);
   if (file_info.fsize != nBytesRead)
   {
      logger_->Write("Cartridge", LogPanic, "Read incorrect %i instead of ", nBytesRead, file_info.fsize);
   }
   else
   {
      logger_->Write("Cartridge", LogNotice, "file read");
   }

   // Check RIFF chunk
   int index = 0;
   if (file_info.fsize >= 12
      && (memcmp(&buffer[0], "RIFF", 4) == 0)
      && (memcmp(&buffer[8], "AMS!", 4) == 0)
      )
   {
      // Reinit Cartridge
      motherboard_->EjectCartridge();

      // Ok, it's correct.
      index += 4;
      // Check the whole size

      int chunk_size = buffer[index]
         + (buffer[index + 1] << 8)
         + (buffer[index + 2] << 16)
         + (buffer[index + 3] << 24);

      index += 8;

      // 'fmt ' chunk ? skip it
      if (index + 8 < file_info.fsize && (memcmp(&buffer[index], "fmt ", 4) == 0))
      {
         index += 8;
      }

      // Good.
      // Now we are at the first cbxx
      while (index + 8 < file_info.fsize)
      {
         if (buffer[index] == 'c' && buffer[index + 1] == 'b')
         {
            index += 2;
            char buffer_block_number[3] = { 0 };
            memcpy(buffer_block_number, &buffer[index], 2);
            int block_number = (buffer_block_number[0] - '0') * 10 + (buffer_block_number[1] - '0');
            index += 2;

            // Read size
            int block_size = buffer[index]
               + (buffer[index + 1] << 8)
               + (buffer[index + 2] << 16)
               + (buffer[index + 3] << 24);
            index += 4;

            if (block_size <= file_info.fsize && block_number < 256)
            {
               // Copy datas to proper ROM
               unsigned char* rom = motherboard_->GetCartridge(block_number);
               memset(rom, 0, 0x1000);
               memcpy(rom, &buffer[index], block_size);
               index += block_size;
            }
            else
            {
               delete[]buffer;
               return -1;
            }
         }
         else
         {
            delete[]buffer;
            return -1;
         }
      }
   }
   else
   {
      // Incorrect headers
      delete[]buffer;
      return -1;
   }
   delete[]buffer;
   return 0;
}