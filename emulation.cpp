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
   sound_is_ready(false)
{
   sound_mixer_ = new SoundMixer();
}

Emulation::~Emulation(void)
{
   delete motherboard_;
}

boolean Emulation::Initialize(DisplayPi* display, SoundPi* sound, KeyboardPi* keyboard)
{
   log_.SetLogger(logger_);
   sound_ = sound;
   display_ = display;
   keyboard_ = keyboard;

   sound_mixer_->Init(sound_, nullptr);
   motherboard_ = new Motherboard(sound_mixer_, keyboard_);

   sound_mixer_->SetLog(&log_);
   motherboard_->SetLog(&log_);

   // Create 
   motherboard_->SetPlus(true);
   motherboard_->InitMotherbard(nullptr, nullptr, display_, nullptr, nullptr, nullptr);
   motherboard_->GetPSG()->SetLog(&log_);
   motherboard_->GetPSG()->InitSound(sound_);

   motherboard_->OnOff();
   motherboard_->GetMem()->InitMemory();
   motherboard_->GetMem()->SetRam(1);
   motherboard_->GetCRTC()->DefinirTypeCRTC(CRTC::AMS40226);
   motherboard_->GetVGA()->SetPAL(true);

   // Load a cartridge
   if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK)
   {
      logger_->Write("Kernel", LogPanic, "Cannot mount drive: %s", DRIVE);
   }

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

   //LoadCprFromBuffer(AmstradPLUS_FR, sizeof(AmstradPLUS_FR));

   motherboard_->GetPSG()->Reset();
   motherboard_->GetSig()->Reset();
   motherboard_->InitStartOptimizedPlus();
   motherboard_->OnOff();

   
#ifdef ARM_ALLOW_MULTI_CORE
   return CMultiCoreSupport::Initialize();
#else
   return TRUE;
#endif
}

void Emulation::Run(unsigned nCore)
{
   switch (nCore)
   {
   case 0:
      // Run sound loop
      sound_is_ready = true;
      sound_mixer_->PrepareBufferThread();
      break;
   case 1:
      // Run sound loop 
      while (!sound_is_ready)
      {
         // Checkin for sound is ready
         CTimer::Get ()->MsDelay (50);
         logger_->Write("CORE", LogNotice, "Waiting to start....");
      }

      logger_->Write("CORE", LogNotice, "Main loop");
      RunMainLoop();
      logger_->Write("CORE", LogNotice, "Exiting...");
      break;
   default:
      break;
   }

}


int Emulation::LoadCprFromBuffer(unsigned char* buffer, int size)
{
   // Check RIFF chunk
   int index = 0;
   if (size >= 12
      && (memcmp(&buffer[0], "RIFF", 4) == 0)
      && (memcmp(&buffer[8], "AMS!", 4) == 0)
      )
   {
      // Reinit Cartridge
      motherboard_->EjectCartridge();

      // Ok, it's correct.
      index += 4;
      // Check the whole size

      int chunk_size = buffer[index]
         + (buffer[index + 1] << 8)
         + (buffer[index + 2] << 16)
         + (buffer[index + 3] << 24);

      index += 8;

      // 'fmt ' chunk ? skip it
      if (index + 8 < size && (memcmp(&buffer[index], "fmt ", 4) == 0))
      {
         index += 8;
      }

      // Good.
      // Now we are at the first cbxx
      while (index + 8 < size)
      {
         if (buffer[index] == 'c' && buffer[index + 1] == 'b')
         {
            index += 2;
            char buffer_block_number[3] = { 0 };
            memcpy(buffer_block_number, &buffer[index], 2);
            int block_number = (buffer_block_number[0] - '0') * 10 + (buffer_block_number[1] - '0');
            index += 2;

            // Read size
            int block_size = buffer[index]
               + (buffer[index + 1] << 8)
               + (buffer[index + 2] << 16)
               + (buffer[index + 3] << 24);
            index += 4;

            if (block_size <= size && block_number < 256)
            {
               // Copy datas to proper ROM
               unsigned char* rom = motherboard_->GetCartridge(block_number);
               memset(rom, 0, 0x1000);
               memcpy(rom, &buffer[index], block_size);
               index += block_size;
            }
            else
            {
               return -1;
            }
         }
         else
         {
            return -1;
         }
      }
   }
   else
   {
      // Incorrect headers
      return -1;
   }

   return 0;
}

void Emulation::RunMainLoop()
{

   unsigned nCelsiusOldTmp = 0;
   int count = 0;
   unsigned lasttick = timer_->GetClockTicks();
   while (1)
   {

      // run for 1/10th of second ( 5 frame) in 10 sequences of 1/100th second (10 ms)
      // us
#define TIME_SLOT  10000
      unsigned new_tick;
      //for (unsigned int i = 0; i < 10; i++)
      {
         motherboard_->StartOptimizedPlus<true, false, false>(4 * TIME_SLOT*100);

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
         // do it !
         CCPUThrottle::Get()->SetSpeed(CPUSpeedLow);

         ScreenMenu menu(&log_ ,logger_, display_, keyboard_, motherboard_);
         menu.Handle();

         keyboard_->ReinitSelect();
         CCPUThrottle::Get()->SetSpeed(CPUSpeedMaximum);
      }
      else
      {
         if (count == 0)
         {
            // Temperature
            unsigned nCelsius = CCPUThrottle::Get()->GetTemperature();
            if (nCelsiusOldTmp != nCelsius)
            {
               //logger_->Write("Kernel", LogNotice, "Temperature = %i", nCelsius);
               nCelsiusOldTmp = nCelsius;
            }


            count = 0;

            // Timing computation 
            static unsigned old = 0;
            unsigned elapsed = timer_->GetTicks();

            //logger_->Write("Kernel", LogNotice, "1s => %i ticks", elapsed - old);
            old = elapsed;
         }
         else
         {
            count++;
         }

      }
   }
}