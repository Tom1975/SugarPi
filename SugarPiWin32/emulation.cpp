#include "emulation.h"
#include "ScreenMenu.h"




Emulation::Emulation( CLogger* log)
   :
   logger_(log),
   setup_(nullptr),
   motherboard_(nullptr),
   display_(nullptr),
   keyboard_(nullptr),
   sound_(nullptr),
   sound_mixer_(nullptr),
   sound_is_ready(false),
   sound_run_(true),
   menu(nullptr)
   
{
   setup_ = new SugarPiSetup(log);
   sound_mixer_ = new SoundMixer();
}

Emulation::~Emulation(void)
{
   delete menu;
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

   menu = new ScreenMenu(&log_, logger_, display_, sound_mixer_, keyboard_, motherboard_, setup_);

   
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