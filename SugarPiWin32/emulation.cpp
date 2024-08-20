#include "emulation.h"
#include "ScreenMenu.h"




Emulation::Emulation( CLogger* log)
   : Engine (log),
   sound_is_ready(false),
   sound_run_(true)
   
{
   setup_ = new SugarPiSetup(log);

}

Emulation::~Emulation(void)
{
   delete menu;
   delete motherboard_;
}


void Emulation::Run(unsigned nCore)
{
#ifdef ARM_ALLOW_MULTI_CORE
   switch (nCore)
   {
   case 0:
      // Run sound loop
      sound_mutex_.lock();
      sound_is_ready = true;
      sound_mutex_.unlock();
      logger_->Write("Sound", LogNotice, "SoundMixer Started");
      sound_mixer_->PrepareBufferThread();

      while(sound_run_)
      {
         sound_mixer_->Tick();
         keyboard_->UpdatePlugnPlay();
         // scheduler call
         display_->Loop();
         WAIT(1);
      }
      
      logger_->Write("Sound", LogNotice, "SoundMixer Ended");
      break;
   case 1:
      // Run Main loop
      {
         bool exit_loop = false;
         while (!exit_loop )
         {
            sound_mutex_.lock();
            exit_loop = sound_is_ready;
            sound_mutex_.unlock();

            // Checkin for sound is ready
            WAIT(50);
            logger_->Write("CORE", LogNotice, "Waiting to start....");
         }
#endif
         logger_->Write("CORE", LogNotice, "Main loop");
         RunMainLoop();
         sound_run_ = false;
         display_->StopLoop();
         logger_->Write("CORE", LogNotice, "Exiting...");
#ifdef ARM_ALLOW_MULTI_CORE
         break;
      }
   case 2:
      // Display loop
      logger_->Write("CORE", LogNotice, "Display Loop started");
      logger_->Write("CORE", LogNotice, "Display Loop Ended");

   default:
      break;
   }
#endif
}

const char* Emulation::GetBaseDirectory()
{
   return ".";
}

void Emulation::ForceStop()
{
   run_ = false;
   if (menu)menu->ForceStop();
}

void Emulation::RunMainLoop()
{
   run_ = true;
   unsigned nCelsiusOldTmp = 0;
   int count = 0;
   while (run_)
   {
#define TIME_SLOT  10000
      motherboard_->StartOptimizedPlus<true, true, false>(4 * TIME_SLOT*10);
            
      // Menu launched ?
      if (keyboard_->IsSelect())
      {
         menu->Handle();
         keyboard_->ReinitSelect();
      }
   }
}