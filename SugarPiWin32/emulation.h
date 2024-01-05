#pragma once

#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "CPCCore/CPCCoreEmu/ILog.h"

#include "DisplayPiDesktop.h"
#include "KeyboardPiDesktop.h"
#include "SoundPiDesktop.h"
#include "SugarPiSetupDesktop.h"
#include "ScreenMenu.h"
#include "log.h"

class Emulation
{
public:
   Emulation(CLogger* log);
   ~Emulation(void);

   boolean Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard);
   void Run(unsigned nCore);
   void RunMainLoop();

   void ForceStop();

protected:
   
   CLogger*          logger_;
   SugarPiSetup*     setup_;
   Motherboard*      motherboard_;
   DisplayPi*        display_;
   KeyboardPi*       keyboard_;
   SoundPi*          sound_;
   SoundMixer*       sound_mixer_;
   Log               log_;

   ScreenMenu* menu;

   bool sound_is_ready;
   bool sound_run_;
   bool run_;

   std::mutex sound_mutex_;

};