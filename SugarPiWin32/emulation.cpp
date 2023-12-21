#include "emulation.h"
#include "ScreenMenu.h"




Emulation::Emulation( CLogger* log)
   :
#ifdef ARM_ALLOW_MULTI_CORE
   CMultiCoreSupport(pMemorySystem),
#endif
   logger_(log),
   setup_(nullptr),
   motherboard_(nullptr),
   display_(nullptr),
   keyboard_(nullptr),
   sound_(nullptr),
   sound_mixer_(nullptr),
   sound_is_ready(false),
   sound_run_(true)
   
{
   setup_ = new SugarPiSetup(log);
   sound_mixer_ = new SoundMixer();
}

Emulation::~Emulation(void)
{
   delete motherboard_;
}

boolean Emulation::Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard)
{
   log_.SetLogger(logger_);
   logger_->Write("Kernel", LogNotice, "Emulation::Initialize");

   sound_ = sound;
   display_ = display;
   keyboard_ = keyboard;

   sound_mixer_->Init(sound_, nullptr);

   logger_->Write("Kernel", LogNotice, "Creating Motherboard");
   motherboard_ = new Motherboard(sound_mixer_, keyboard_);

   sound_mixer_->SetLog(&log_);
   motherboard_->SetLog(&log_);

   motherboard_->SetPlus(true);
   motherboard_->InitMotherbard(&log_, nullptr, display_, nullptr, nullptr, nullptr);
   motherboard_->GetPSG()->SetLog(&log_);
   motherboard_->GetPSG()->InitSound(sound_);

   motherboard_->OnOff();
   motherboard_->GetMem()->InitMemory();
   motherboard_->GetMem()->SetRam(1);
   motherboard_->GetCRTC()->DefinirTypeCRTC(CRTC::AMS40226);
   motherboard_->GetVGA()->SetPAL(true);
   motherboard_->GetSig()->fdc_present_ = true;
   motherboard_->GetPPI()->SetExpSignal ( true );

   // Setup
   setup_->Init(display, sound_mixer_, motherboard_, keyboard_);
   setup_->Load();

   motherboard_->GetPSG()->Reset();
   motherboard_->GetSig()->Reset();
   motherboard_->InitStartOptimizedPlus();
   motherboard_->OnOff();

   
   logger_->Write("Kernel", LogNotice, "End of Emulation init.");
   return TRUE;
}

void Emulation::Run(unsigned nCore)
{
#ifdef ARM_ALLOW_MULTI_CORE
   switch (nCore)
   {
   case 0:
      // Run sound loop
      //sound_mutex_.Acquire();
      sound_is_ready = true;
      //sound_mutex_.Release();
      logger_->Write("Sound", LogNotice, "SoundMixer Started");
      while(sound_run_)
      {
         sound_mixer_->PrepareBufferThread();
         keyboard_->UpdatePlugnPlay();
         scheduler_->Yield();
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
         WAIT(1000);
         logger_->Write("CORE", LogNotice, "Exiting...");
#ifdef ARM_ALLOW_MULTI_CORE
         break;
      }
   case 2:
      // Display loop
      logger_->Write("CORE", LogNotice, "Display Loop started");
      //display_->Loop();
      logger_->Write("CORE", LogNotice, "Display Loop Ended");

   default:
      break;
   }
#endif
}


void Emulation::ForceStop()
{
   run_ = false;
}

void Emulation::RunMainLoop()
{
   run_ = true;
   ScreenMenu menu(&log_ ,logger_, display_, sound_mixer_, keyboard_, motherboard_, setup_);
   unsigned nCelsiusOldTmp = 0;
   int count = 0;
   bool finished = false;
   while (!finished && run_)
   {
#define TIME_SLOT  10000
      motherboard_->StartOptimizedPlus<true, true, false>(4 * TIME_SLOT*10);
            
      // Menu launched ?
      if (keyboard_->IsSelect())
      {
         /*finished = */(menu.Handle()/* == IAction::Action_Shutdown*/);
         keyboard_->ReinitSelect();
      }
   }
}