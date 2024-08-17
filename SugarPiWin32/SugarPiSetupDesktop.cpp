//
#include "SugarPiSetupDesktop.h"

#define SECTION_SETUP      "SETUP"
#define KEY_SYNC           "sync"
#define KEY_CART           "cart"
#define KEY_LAYOUT         "layout"

#define KEY_SYNC_SOUND     "sound"
#define KEY_SYNC_FRAME     "frame"


#define DEFAULT_CART "CART/crtc3_projo.cpr"
#define DEFAULT_LAYOUT "LAYOUT/101_keyboard"


SugarPiSetup::SugarPiSetup( CLogger* log) : log_(log), display_(nullptr), sound_(nullptr), motherboard_(nullptr), keyboard_(nullptr)
{
   config_ = new ConfigurationManager(log);
}

SugarPiSetup::~SugarPiSetup()
{
   delete config_;  
}

void  SugarPiSetup::Init(DisplayPi* display, SoundMixer* sound, Motherboard *motherboard, KeyboardPi* keyboard)
{
   display_ = display;
   sound_ = sound;
   motherboard_ = motherboard;
   keyboard_ = keyboard;
}

void SugarPiSetup::Load()
{
   config_->OpenFile("Config/config");

   // Syncronisation
   #define SIZE_OF_BUFFER 256
   char buffer[SIZE_OF_BUFFER];
   if (config_->GetConfiguration (SECTION_SETUP, KEY_SYNC, KEY_SYNC_FRAME, buffer, SIZE_OF_BUFFER ))
   {
      if (strcmp ( buffer, KEY_SYNC_SOUND) == 0)
      {
         SetSync(SYNC_SOUND);
      }
      else if (strcmp ( buffer, KEY_SYNC_FRAME) == 0)
      {
         SetSync(SYNC_FRAME);
      }
   }

   // Keyboard layout (if any)
   if (config_->GetConfiguration (SECTION_SETUP, KEY_LAYOUT, DEFAULT_LAYOUT, buffer, SIZE_OF_BUFFER ))
   {
      keyboard_->LoadKeyboard (buffer);
   }
   // todo
   
   // Hardware configuration

   // Current cartridge
   if (config_->GetConfiguration (SECTION_SETUP, KEY_CART, DEFAULT_CART, buffer, SIZE_OF_BUFFER ))
   {
      LoadCartridge(buffer);
   }   
}

void SugarPiSetup::Save()
{
   // Hardware configuration
   // to add

   // Syncronisation
   config_->SetConfiguration (SECTION_SETUP, KEY_SYNC, (sync_==SYNC_SOUND)?KEY_SYNC_SOUND:KEY_SYNC_FRAME);

   // Current cartridge
   config_->SetConfiguration (SECTION_SETUP, KEY_CART, cart_path_.c_str());

   config_->CloseFile();
}

void  SugarPiSetup::SetSync (SYNC_TYPE sync)
{
   sync_ = sync;
   if (sync==SYNC_SOUND)
   {
      display_->SyncWithFrame(false);
      sound_->SyncOnSound(true);      
   }
   else
   {
      display_->SyncWithFrame(true);
      sound_->SyncOnSound(false);
   }
}

SugarPiSetup::SYNC_TYPE  SugarPiSetup::GetSync ()
{
   return sync_;
}

void SugarPiSetup::LoadSetup(const char* path)
{
   MachineSettings* settings = MachineSettings::CreateSettings(config_, path);
   // motherboard_->ChangeSettings(settings);
   // TODO : load current settings
}



void SugarPiSetup::LoadCartridge (const char* path)
{
   FILE* f;

   if (fopen_s(&f, path, "rb") == 0)
   {
      fseek(f, 0, SEEK_END);
      int buffer_size = ftell(f);
      rewind(f);
      unsigned char* buffer = new unsigned char[buffer_size];

      fread(buffer, buffer_size, 1, f);
      fclose(f);
      int ret = LoadCprFromBuffer(buffer, buffer_size);
      delete[]buffer;
   }
}

int SugarPiSetup::LoadCprFromBuffer(unsigned char* buffer, int size)
{
   // Check RIFF chunk
   int index = 0;
   if (size >= 12
      && (memcmp(&buffer[0], "RIFF", 4) == 0)
      && (memcmp(&buffer[8], "AMS!", 4) == 0)
      )
   {
      // Reinit Cartridge
      motherboard_->EjectCartridge();

      // Ok, it's correct.
      index += 4;
      // Check the whole size ?
      index += 8;

      // 'fmt ' chunk ? skip it
      if (index + 8 < size && (memcmp(&buffer[index], "fmt ", 4) == 0))
      {
         index += 8;
      }

      // Good.
      // Now we are at the first cbxx
      while (index + 8 < size)
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

            if (block_size <= size && block_number < 256)
            {
               // Copy datas to proper ROM
               unsigned char* rom = motherboard_->GetCartridge(block_number);
               memset(rom, 0, 0x1000);
               memcpy(rom, &buffer[index], block_size);
               index += block_size;
            }
            else
            {
               return -1;
            }
         }
         else
         {
            return -1;
         }
      }
   }
   else
   {
      // Incorrect headers
      return -1;
   }

   return 0;
}

