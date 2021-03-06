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

class Emulation
#ifdef ARM_ALLOW_MULTI_CORE
   : public CMultiCoreSupport
#endif
{
public:
   Emulation(CMemorySystem* pMemorySystem, CLogger* log, CTimer* timer);
   ~Emulation(void);

   boolean Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard,CScheduler	*scheduler);
   void Run(unsigned nCore);
   void RunMainLoop();


protected:
   
   CLogger*          logger_;
   CTimer*           timer_;
   CSpinLock         sound_mutex_;
   CScheduler*       scheduler_;

   SugarPiSetup*     setup_;
   Motherboard*      motherboard_;
   DisplayPi*        display_;
   KeyboardPi*       keyboard_;
   SoundPi*          sound_;
   SoundMixer*       sound_mixer_;
   Log               log_;

   bool sound_is_ready;
   bool sound_run_;
};