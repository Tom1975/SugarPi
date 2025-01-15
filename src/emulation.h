#pragma once

#include <circle/multicore.h>
#include <circle/screen.h>
#include <circle/memory.h>
#include <circle/types.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/cputhrottle.h>
#include <circle/sched/scheduler.h>

#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#include "CPCCore/CPCCoreEmu/Motherboard.h"
#include "CPCCore/CPCCoreEmu/SoundMixer.h"
#include "CPCCore/CPCCoreEmu/ILog.h"

#include "DisplayPi.h"
#include "KeyboardPi.h"
#include "SoundPi.h"
#include "SugarPiSetup.h"
#include "log.h"
#include "Engine.h"

class Emulation : public Engine
#ifdef ARM_ALLOW_MULTI_CORE
   ,public CMultiCoreSupport
#endif
{
public:
   Emulation(CMemorySystem* pMemorySystem, CLogger* log, CTimer* timer);
   ~Emulation(void);

   boolean Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard,CScheduler	*scheduler);
   virtual const char* GetBaseDirectory();
   void Run(unsigned nCore);
   void RunMainLoop();


protected:
   CTimer*           timer_;
   CSpinLock         sound_mutex_;
   CScheduler*       scheduler_;

   bool pnp_need_update_;
   bool sound_is_ready;
   bool sound_run_;
};