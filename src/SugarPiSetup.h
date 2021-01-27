#pragma once

#include <circle/logger.h>

#include "DisplayPi.h"
#include "KeyboardPi.h"
#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "ConfigurationManager.h"

class SugarPiSetup
{
public :
   SugarPiSetup ( CLogger* log);
   virtual ~SugarPiSetup();

   void Init(DisplayPi* display, SoundMixer* sound, Motherboard *motherboard, KeyboardPi* keyboard);

   void Load();
   void Save();

   // Access each values
   enum SYNC_TYPE{
      SYNC_FRAME,
      SYNC_SOUND
   } ;
   void SetSync (SYNC_TYPE sync);
   SYNC_TYPE GetSync ();

   void LoadCartridge (const char* path);

protected:
  
   int LoadCprFromBuffer(unsigned char* buffer, int size);


   CLogger* log_;
   DisplayPi* display_;
   SoundMixer* sound_;
   Motherboard* motherboard_;
   KeyboardPi* keyboard_;

   ConfigurationManager* config_;

   SYNC_TYPE sync_;
   std::string cart_path_;
};