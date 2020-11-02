#pragma once

#include <circle/logger.h>

#include "DisplayPi.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "ConfigurationManager.h"

class SugarPiSetup
{
public :
   SugarPiSetup ( CLogger* log);
   virtual ~SugarPiSetup();

   void Init(DisplayPi* display, SoundMixer* sound);

   void Load();
   void Save();

   // Access each values
   enum SYNC_TYPE{
      SYNC_FRAME,
      SYNC_SOUND
   } ;
   void SetSync (SYNC_TYPE sync);
   SYNC_TYPE GetSync ();

protected:
  
   CLogger* log_;
   DisplayPi* display_;
   SoundMixer* sound_;

   SYNC_TYPE sync_;
   ConfigurationManager config_;
};