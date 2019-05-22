//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
//#include "Cartridge.h"

#define DRIVE		"SD:"
//#define DRIVE		"USB:"

CKernel::CKernel(void)
   :
#if AARCH != 64			// non-MMU mode currently does not work with AArch64
   m_Memory(TRUE),	// set this to TRUE to enable MMU and to boost performance
#endif
   m_Timer(&m_Interrupt),
   m_Logger(m_Options.GetLogLevel(), &m_Timer),
   m_EMMC(&m_Interrupt, &m_Timer, &m_ActLED),
   sound_mixer_(nullptr),
   display_(&m_Logger),
   motherboard_emulation_ (nullptr)
{
   sound_mixer_ = new SoundMixer;
   motherboard_emulation_ = new Motherboard(sound_mixer_, &keyboard_);
}

CKernel::~CKernel (void)
{
   delete motherboard_emulation_; 
   delete sound_mixer_;
}

boolean CKernel::Initialize (void)
{

   boolean bOK = TRUE;

   if (bOK)
   {
      bOK = display_.Initialization();
   }
   if (bOK)
   {
      bOK = m_Serial.Initialize(115200);
   }

   if (bOK)
   {
      CDevice* pTarget = &m_Serial; 
      //CDevice* pTarget = m_DeviceNameService.GetDevice(m_Options.GetLogDevice(), FALSE);
      /*if (pTarget == 0)
      {
         pTarget = display_.GetScreenDevice();
      }*/
      bOK = m_Logger.Initialize(pTarget);
   }
   
   if (bOK)
   {
      bOK = m_Interrupt.Initialize();
   }

   if (bOK)
   {
      bOK = m_Timer.Initialize();
   }
   if (bOK)
   {
      bOK = m_EMMC.Initialize();
   }

   CCPUThrottle::Get()->SetSpeed(CPUSpeedMaximum);


   m_Logger.Write("Kernel", LogNotice, "Initialisation done.");
   return bOK;
}


int CKernel::LoadCprFromBuffer(unsigned char* buffer, int size)
{

   // Check RIFF chunk
   int index = 0;
   if (size >= 12
      && (memcmp(&buffer[0], "RIFF", 4) == 0)
      && (memcmp(&buffer[8], "AMS!", 4) == 0)
      )
   {
      // Reinit Cartridge
      motherboard_emulation_->EjectCartridge();

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
            int block_number = (buffer_block_number[0]-'0')*10+ (buffer_block_number[1]-'0');
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
               unsigned char* rom = motherboard_emulation_->GetCartridge(block_number);
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
/*
void CKernel::GetFolderCart()
{
   // Show contents of root directory
   DIR Directory;
   FILINFO FileInfo;
   FRESULT Result = f_findfirst(&Directory, &FileInfo, DRIVE "/CART", "*");
   for (unsigned i = 0; Result == FR_OK && FileInfo.fname[0]; i++)
   {
      if (!(FileInfo.fattrib & (AM_HID | AM_SYS)))
      {
         CString FileName;
         FileName.Format("%-19s", FileInfo.fname);
      }

      Result = f_findnext(&Directory, &FileInfo);
   }
}*/

TShutdownMode CKernel::Run (void)
{
   m_Logger.Write("Kernel", LogNotice, "Entering running mode...");

   // Get proper file to load
   

   // Init motherboard
   m_Logger.Write("Kernel", LogNotice, "Init motherboard...");
   motherboard_emulation_->SetPlus(true);
   motherboard_emulation_->InitMotherbard(nullptr, nullptr, &display_, nullptr, nullptr, nullptr);
   motherboard_emulation_->OnOff();
   motherboard_emulation_->GetMem()->InitMemory();
   motherboard_emulation_->GetMem()->SetRam(1);
   motherboard_emulation_->GetCRTC()->DefinirTypeCRTC(CRTC::AMS40226);
   motherboard_emulation_->GetVGA()->SetPAL(true);

   // Load a cartridge
   if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK)
   {
      m_Logger.Write("Kernel", LogPanic, "Cannot mount drive: %s", DRIVE);
   }
   
   FIL File;
   FRESULT Result = f_open(&File, DRIVE "/CART/crtc3_projo.cpr", FA_READ | FA_OPEN_EXISTING);
   if (Result != FR_OK)
   {
      m_Logger.Write("Kernel", LogPanic, "Cannot open file: %s", DRIVE "/CART/crtc3_projo.cpr");
   }
   else
   {
      m_Logger.Write("Kernel", LogNotice, "File opened correctly");
   }

   FILINFO file_info;
   f_stat(DRIVE "/CART/crtc3_projo.cpr", &file_info);
   m_Logger.Write("Kernel", LogNotice, "File size : %i", file_info.fsize);
   unsigned char* buff = new unsigned char [file_info.fsize];
   unsigned nBytesRead;

   m_Logger.Write("Kernel", LogNotice, "buffer allocated");
   f_read(&File, buff, file_info.fsize, &nBytesRead);
   if (file_info.fsize != nBytesRead)
   {
      m_Logger.Write("Kernel", LogPanic, "Read incorrect %i instead of ", nBytesRead, file_info.fsize);
   }
   else
   {
      m_Logger.Write("Kernel", LogNotice, "file read");
   }
   LoadCprFromBuffer(buff, file_info.fsize);
   m_Logger.Write("Kernel", LogNotice, "CPR read correctly");

   //LoadCprFromBuffer(AmstradPLUS_FR, sizeof(AmstradPLUS_FR));

   motherboard_emulation_->GetPSG()->Reset();
   motherboard_emulation_->GetSig()->Reset();
   motherboard_emulation_->InitStartOptimizedPlus();
   motherboard_emulation_->OnOff();

   m_Logger.Write("Kernel", LogNotice, "Done !");

   
   // check the blink frequency without and with MMU (see option in constructor above)
	while (1)
	{

      motherboard_emulation_->StartOptimizedPlus(4000*10000);
      unsigned nCelsius = CCPUThrottle::Get()->GetTemperature();
      m_Logger.Write("Kernel", LogNotice, "Temperature = %i", nCelsius);
     
	}

	return ShutdownHalt;
}
