#pragma once

#include "DisplayPi.h"
#include "SoundPi.h"


class SugarPiSetup
{
public :
   SugarPiSetup ();
   virtual ~SugarPiSetup();

   void Init(DisplayPi* display, SoundPi* sound);

   void Load();
   void Save();

protected:
   DisplayPi* display_;
   SoundPi* sound_;
};