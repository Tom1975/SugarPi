#pragma once

#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "CPCCore/CPCCoreEmu/ILog.h"

#ifdef _WIN32
   #include "SoundPiDesktop.h"
   #include "SugarPiSetupDesktop.h"
   #include "KeyboardPiDesktop.h"
   #include "DisplayPiDesktop.h"
#else
   #include "SoundPi.h"
   #include "SugarPiSetup.h"
   #include "KeyboardPi.h"
   #include "DisplayPi.h"

#endif

#include "ScreenMenu.h"
#include "log.h"


class Engine : public IEngine
{
public:
   Engine(CLogger* log);
   virtual ~Engine();
      
   virtual boolean Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard);
   virtual void UpdateComputer(bool no_cart_reload);
   virtual void LoadConfiguration(const char* config_name_file);
   virtual void Reset();

   // Path access
   virtual const char* GetBaseDirectory() = 0;
   void SetFDCPlugged(bool bFDCPlugged) { motherboard_->GetSig()->fdc_present_ = bFDCPlugged; motherboard_->GetPPI()->SetExpSignal(bFDCPlugged); };

protected:
   void LoadRom(int rom_number, const char* path);

   CLogger* logger_;
   SugarPiSetup* setup_;
   Motherboard* motherboard_;
   DisplayPi* display_;
   KeyboardPi* keyboard_;
   SoundPi* sound_;
   SoundMixer* sound_mixer_;
   Log               log_;

   MachineSettings* current_settings_;

   ScreenMenu* menu;

};