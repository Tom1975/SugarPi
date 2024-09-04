#pragma once


#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#include "CString.h"
#endif


#include "DisplayPi.h"
#include "KeyboardPi.h"
#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "CPCCore/CPCCoreEmu/MachineSettings.h"
#include "MultiLanguage.h"
#include "ConfigurationManager.h"

class SugarPiSetup
{
public :
   SugarPiSetup ( CLogger* log);
   virtual ~SugarPiSetup();

   void Init(DisplayPi* display, SoundMixer* sound, Motherboard *motherboard, KeyboardPi* keyboard, MultiLanguage* language);

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
   void LoadDisk (const char* path);
   void LoadTape (const char* path);
   MachineSettings* LoadSetup(const char* path);

   ConfigurationManager* GetConfigurationManager(){return config_;}

protected:
  
   int LoadCprFromBuffer(unsigned char* buffer, int size);


   CLogger* log_;
   DisplayPi* display_;
   SoundMixer* sound_;
   Motherboard* motherboard_;
   KeyboardPi* keyboard_;
   unsigned int language_id_;
   MultiLanguage* language_;

   ConfigurationManager* config_;

   SYNC_TYPE sync_;
   std::string cart_path_;
};