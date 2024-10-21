//
#include <CPCCore/CPCCoreEmu/stdafx.h>
#include "Engine.h"

#ifdef  __circle__
#include <circle/string.h>
#else
#include "CString.h"
#endif

#include <filesystem>
Engine::Engine(CLogger* log) :
   in_menu_(false),
   logger_(log),
   setup_(nullptr),
   motherboard_(nullptr),
   display_(nullptr),
   keyboard_(nullptr),
   sound_(nullptr),
   sound_mixer_(nullptr),
   current_settings_(nullptr),
   menu(nullptr),
   language_manager_(log),
   language_(nullptr)
{
   sound_mixer_ = new SoundMixer();
   current_settings_ = new MachineSettings();
}

Engine::~Engine()
{
   delete sound_mixer_;
   delete language_;
}

boolean Engine::Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard)
{
   log_.SetLogger(logger_);
   logger_->Write("Kernel", LogNotice, "Emulation::Initialize");

   if (setup_ != nullptr)
   {
      language_ = new MultiLanguage(&language_manager_);
      language_->Init("RES/labels.ini");
   }

   sound_ = sound;
   display_ = display;
   keyboard_ = keyboard;

   sound_mixer_->Init(sound_, nullptr);
   logger_->Write("Kernel", LogNotice, "Creating Motherboard");
   motherboard_ = new Motherboard(sound_mixer_, keyboard_);

   sound_mixer_->SetLog(&log_);
   motherboard_->SetLog(&log_);

   motherboard_->SetPlus(true);
   motherboard_->InitMotherbard(&log_, nullptr, display_, nullptr, nullptr, nullptr);
   motherboard_->GetPSG()->SetLog(&log_);
   motherboard_->GetPSG()->InitSound(sound_);

   motherboard_->OnOff();
   motherboard_->GetMem()->InitMemory();
   motherboard_->GetMem()->SetRam(1);
   motherboard_->GetCRTC()->DefinirTypeCRTC(CRTC::AMS40226);
   motherboard_->GetVGA()->SetPAL(true);
   motherboard_->GetSig()->fdc_present_ = true;
   motherboard_->GetPPI()->SetExpSignal(true);

   menu = new ScreenMenu(this, &log_, logger_, display_, sound_mixer_, keyboard_, motherboard_, setup_, language_);
   logger_->Write("Kernel", LogNotice, "End of Emulation init.");

   // Setup
   setup_->Init(display, sound_mixer_, motherboard_, keyboard_, language_);
   setup_->Load();

   Reset();

   UpdateComputer(true);

   return TRUE;
}

void Engine::Reset()
{
   motherboard_->GetPSG()->Reset();
   motherboard_->GetSig()->Reset();
   motherboard_->InitStartOptimizedPlus();
   motherboard_->OnOff();
}

void Engine::LoadConfiguration(const char* config_name_file)
{
   delete current_settings_;
   MachineSettings* settings = setup_->LoadSetup(config_name_file);
   current_settings_ = settings;
   current_settings_->Load();

   // Update with init.
   /*if (init != nullptr && !init->_cart_inserted.empty())
   {
      current_settings_->SetDefaultCartridge(init->_cart_inserted.string().c_str());
   }*/

   UpdateComputer(true);
}

void Engine::LoadRom(int rom_number, const char* path)
{
   FILE* file_rom = nullptr;
   if (fopen_s(&file_rom, path, "rb") == 0 && file_rom != nullptr)
   {
      fseek(file_rom, 0, SEEK_END);
      unsigned int lSize = ftell(file_rom);
      fseek(file_rom, 0, SEEK_SET);

      if (lSize > 0xffff)
      {
         logger_->Write("Kernel", LogNotice, "ROM %s too big... KO !!\n", path);
         fclose(file_rom);
      }
      else
      {
         Memory::RamBank rom;
         fread(rom, 1, lSize, file_rom);
         fclose(file_rom);
         if (rom_number == -1)
            motherboard_->GetMem()->LoadLowerROM(rom, sizeof(rom));
         else
            motherboard_->GetMem()->LoadROM(rom_number, rom, sizeof(rom));
      }
   }
}

void Engine::UpdateComputer(bool no_cart_reload)
{
   motherboard_->GetMem()->InitMemory();

   // Update RAM todo
   motherboard_->GetMem()->SetRam(current_settings_->GetRamCfg());

   // Update ROM todo
   std::filesystem::path path ( GetBaseDirectory());
   
   path /= "ROMS";
   std::filesystem::path rom_path = path;
   path /= current_settings_->GetLowerRom();
   LoadRom(-1, path.string().c_str());

   keyboard_->LoadKeyboard(current_settings_->GetKeyboardConfig());

   for (int i = 0; i < 256; i++)
   {
      const char* rom_path_str = current_settings_->GetUpperRom(i);
      if (rom_path_str == nullptr)
      {
         motherboard_->GetMem()->ClearRom(i);
      }
      else
      {
         path = rom_path;
         path /= rom_path_str;
         LoadRom(i, path.string().c_str());
      }
   }

   // Hardware
   motherboard_->GetCRTC()->DefinirTypeCRTC(current_settings_->GetCRTCType());
   motherboard_->GetVGA()->SetPAL(current_settings_->PALPlugged());
   SetFDCPlugged(current_settings_->FDCPlugged());

   // External devices
   motherboard_->UpdateExternalDevices();


   // PLUS Machine ?todo
   unsigned int hardware_type = current_settings_->GetHardwareType();
   //SetMachineType(hardware_type);
   if (hardware_type == MachineSettings::PLUS_6128
      || hardware_type == MachineSettings::PLUS_464)
   {
      motherboard_->SetPlus(true);
      if (no_cart_reload == false)
      {
         setup_->LoadCartridge(current_settings_->GetDefaultCartridge());
      }
   }
   else
   {
      motherboard_->SetPlus(false);
   }
   // reset
   motherboard_->OnOff();
}
