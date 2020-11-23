#include "emulation.h"
#include "ScreenMenu.h"


#define DRIVE		"SD:"


Emulation::Emulation(CMemorySystem* pMemorySystem, CLogger* log, CTimer* timer)
   :
#ifdef ARM_ALLOW_MULTI_CORE
   CMultiCoreSupport(pMemorySystem),
#endif
   logger_(log),
   timer_(timer),
   motherboard_(nullptr),
   display_(nullptr),
   sound_mixer_(nullptr),
   sound_(nullptr),
   sound_is_ready(false),
   sound_mutex_(IRQ_LEVEL),
   setup_(nullptr)
   
{
   setup_ = new SugarPiSetup(log);
   sound_mixer_ = new SoundMixer();
}

Emulation::~Emulation(void)
{
   delete motherboard_;
}

boolean Emulation::Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard, CScheduler	*scheduler)
{
   log_.SetLogger(logger_);
   logger_->Write("Kernel", LogNotice, "Emulation::Initialize");

   sound_ = sound;
   display_ = display;
   keyboard_ = keyboard;
   scheduler_ = scheduler;

   sound_mixer_->Init(sound_, nullptr);

   logger_->Write("Kernel", LogNotice, "Creating Motherboard");
   motherboard_ = new Motherboard(sound_mixer_, keyboard_);

   sound_mixer_->SetLog(&log_);
   motherboard_->SetLog(&log_);

   if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK)
   {
      logger_->Write("Kernel", LogPanic, "Cannot mount drive: %s", DRIVE);
   }

   motherboard_->SetPlus(true);
   motherboard_->InitMotherbard(nullptr, nullptr, display_, nullptr, nullptr, nullptr);
   motherboard_->GetPSG()->SetLog(&log_);
   motherboard_->GetPSG()->InitSound(sound_);

   motherboard_->OnOff();
   motherboard_->GetMem()->InitMemory();
   motherboard_->GetMem()->SetRam(1);
   motherboard_->GetCRTC()->DefinirTypeCRTC(CRTC::AMS40226);
   motherboard_->GetVGA()->SetPAL(true);

   // Setup
   setup_->Init(display, sound_mixer_, motherboard_);
   setup_->Load();

/*
   #define CARTOUCHE_BASE "/CART/crtc3_projo.cpr"
//#define CARTOUCHE_BASE "/CART/gnggxfinalalpha.cpr"

   FIL File;
   FRESULT Result = f_open(&File, DRIVE CARTOUCHE_BASE, FA_READ | FA_OPEN_EXISTING);

   if (Result != FR_OK)
   {
      logger_->Write("Kernel", LogPanic, "Cannot open file: %s", DRIVE CARTOUCHE_BASE);
   }
   else
   {
      logger_->Write("Kernel", LogNotice, "File opened correctly");
   }

   FILINFO file_info;
   f_stat(DRIVE CARTOUCHE_BASE, &file_info);
   logger_->Write("Kernel", LogNotice, "File size : %i", file_info.fsize);
   unsigned char* buff = new unsigned char[file_info.fsize];
   unsigned nBytesRead;

   logger_->Write("Kernel", LogNotice, "buffer allocated");
   f_read(&File, buff, file_info.fsize, &nBytesRead);
   if (file_info.fsize != nBytesRead)
   {
      logger_->Write("Kernel", LogPanic, "Read incorrect %i instead of ", nBytesRead, file_info.fsize);
   }
   else
   {
      logger_->Write("Kernel", LogNotice, "file read");
   }
   LoadCprFromBuffer(buff, file_info.fsize);
   logger_->Write("Kernel", LogNotice, "CPR read correctly");
*/
   //LoadCprFromBuffer(AmstradPLUS_FR, sizeof(AmstradPLUS_FR));

   motherboard_->GetPSG()->Reset();
   motherboard_->GetSig()->Reset();
   motherboard_->InitStartOptimizedPlus();
   motherboard_->OnOff();

   
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
      sound_is_ready = true;
      //sound_mutex_.Release();
      logger_->Write("Sound", LogNotice, "SoundMixer Started");
      while(1)
      {
         sound_mixer_->PrepareBufferThread();
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
         sound_mixer_->StopMixer();
         CTimer::Get ()->MsDelay (4000);
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



void Emulation::RunMainLoop()
{
   ScreenMenu menu(&log_ ,logger_, display_, sound_mixer_, keyboard_, motherboard_, setup_);
   unsigned nCelsiusOldTmp = 0;
   int count = 0;
   unsigned lasttick = timer_->GetClockTicks();
   bool finished = false;
   while (!finished )
   {

      // run for 1/10th of second ( 5 frame) in 10 sequences of 1/100th second (10 ms)
      // us
#define TIME_SLOT  10000
      unsigned new_tick;
      //for (unsigned int i = 0; i < 10; i++)
      {
         motherboard_->StartOptimizedPlus<true, false, false>(4 * TIME_SLOT*10);

         new_tick = timer_->GetClockTicks();
         //if (new_tick - lasttick < i * TIME_SLOT)
         {
            //timer_->SimpleusDelay(i*TIME_SLOT - (new_tick - lasttick)-1);
         }
      }
      lasttick = new_tick;
      
      
      // Menu launched ?
      if (keyboard_->IsSelect())
      {
         CCPUThrottle::Get()->SetSpeed(CPUSpeedLow);
         finished = (menu.Handle() == ScreenMenu::Action_Shutdown);
         keyboard_->ReinitSelect();
         CCPUThrottle::Get()->SetSpeed(CPUSpeedMaximum);
      }
      else
      {
         if (count == 10)
         {
            // Temperature
            unsigned nCelsius = CCPUThrottle::Get()->GetTemperature();
            if (nCelsiusOldTmp != nCelsius)
            {
               logger_->Write("Kernel", LogNotice, "Temperature = %i", nCelsius);
               nCelsiusOldTmp = nCelsius;
            }


            count = 0;

            // Timing computation 
            static unsigned old = 0;
            unsigned elapsed = timer_->GetTicks();

            logger_->Write("Kernel", LogNotice, "1s => %i ticks", (elapsed - old));
            old = elapsed;
         }
         else
         {
            count++;
         }

      }
   }
}