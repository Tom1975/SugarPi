#include "emulation.h"
#include "ScreenMenu.h"


#define DRIVE		"SD:"

Emulation::Emulation(CMemorySystem* pMemorySystem, CLogger* log, CTimer* timer)
   : Engine(log),
#ifdef ARM_ALLOW_MULTI_CORE
   CMultiCoreSupport(pMemorySystem),
#endif
   timer_(timer),
   sound_mutex_(IRQ_LEVEL),
   sound_is_ready(false),
   sound_run_(true)
   
{
   setup_ = new SugarPiSetup(log);
}

Emulation::~Emulation(void)
{
   delete motherboard_;
}

const char* Emulation::GetBaseDirectory()
{
   return DRIVE;
}

boolean Emulation::Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard, CScheduler	*scheduler)
{
   scheduler_ = scheduler;

   Engine::Initialize(display, sound, keyboard);

#ifdef ARM_ALLOW_MULTI_CORE
   logger_->Write("Kernel", LogNotice, "CMultiCoreSupport is going to initialize");
   return CMultiCoreSupport::Initialize();
#else
   logger_->Write("Kernel", LogNotice, "End of Emulation init.");
   return TRUE;
#endif
}

void Emulation::Run(unsigned nCore)
{
#ifdef ARM_ALLOW_MULTI_CORE
   switch (nCore)
   {
   case 0:
      // Run sound loop
      //sound_mutex_.Acquire();
      //sound_mutex_.Release();
      logger_->Write("Sound", LogNotice, "SoundMixer Started");

      for (int i = 0; i < 10; i++)
         display_->Draw();

      sound_is_ready = true;

      while(sound_run_)
      {
         sound_mixer_->PrepareBufferThread();
         keyboard_->UpdatePlugnPlay();
         scheduler_->Yield();
         display_->Loop();
      }
      
      logger_->Write("Sound", LogNotice, "SoundMixer Ended");
      break;
   case 1:
      // Run Main loop
      {
         bool exit_loop = false;
         while (!exit_loop )
         {
            sound_mutex_.Acquire();
            exit_loop = sound_is_ready;
            sound_mutex_.Release();

            // Checkin for sound is ready
            CTimer::Get ()->MsDelay (50);
            logger_->Write("CORE", LogNotice, "Waiting to start....");
         }
#endif
         logger_->Write("CORE", LogNotice, "Main loop");
         RunMainLoop();
         sound_run_ = false;
         CTimer::Get ()->MsDelay (1000);
         logger_->Write("CORE", LogNotice, "Exiting...");
#ifdef ARM_ALLOW_MULTI_CORE
         break;
      }
   case 2:
      // Delayed initialisation
      logger_->Write("CORE", LogNotice, "Delayed init...");
      SugarboxLogo::Load();
      logger_->Write("CORE", LogNotice, "Delayed init done !");
      break;

   default:
      break;
   }
#endif
}



void Emulation::RunMainLoop()
{
   unsigned nCelsiusOldTmp = 0;
   int count = 0;
   bool finished = false;
   while (!finished )
   {
#define TIME_SLOT  10000
      //logger_->Write("Kernel", LogNotice, "StartOptimizedPlus... !");
      motherboard_->StartOptimizedPlus<true, true, false>(4 * TIME_SLOT*10);
      //logger_->Write("Kernel", LogNotice, "Done !");
      // Menu launched ?
      if (keyboard_->IsSelect())
      {
         logger_->Write("Kernel", LogNotice, "Select...");
         
         ScreenMenu menu(this, &log_ ,logger_, display_, sound_mixer_, keyboard_, motherboard_, setup_);
         CCPUThrottle::Get()->SetSpeed(CPUSpeedLow);
         // todo : find a smart way to signal exit
         /*finished = */(menu.Handle()/* == IAction::Action_Shutdown*/);
         keyboard_->ReinitSelect();
         CCPUThrottle::Get()->SetSpeed(CPUSpeedMaximum);
      }
      else
      {
         if (count == 10)
         {
            // Temperature
            logger_->Write("Kernel", LogNotice, "GetTemperature...");
            unsigned nCelsius = CCPUThrottle::Get()->GetTemperature();
            if (nCelsiusOldTmp != nCelsius)
            {
               logger_->Write("Kernel", LogNotice, "Temperature = %i", nCelsius);
               nCelsiusOldTmp = nCelsius;
            }


            count = 0;

            // Timing computation 
            //static unsigned old = 0;
            /*unsigned elapsed = timer_->GetTicks();

            logger_->Write("Kernel", LogNotice, "1s => %i ticks", (elapsed - old));
            old = elapsed;*/
         }
         else
         {
            count++;
         }

      }
   }
}