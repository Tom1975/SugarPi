#pragma once

#include "Engine.h"

#include "log.h"

class Emulation : public Engine
{
public:
   Emulation(CLogger* log);
   ~Emulation(void);

   void Run(unsigned nCore);
   void RunMainLoop();

   void ForceStop();
   virtual const char* GetBaseDirectory();

protected:
   

   bool sound_is_ready;
   bool sound_run_;
   bool run_;

   std::mutex sound_mutex_;

};