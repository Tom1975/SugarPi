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
#include "ScreenMenu.h"
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
   cpu_throttle_(nullptr),
   m_EMMC(&m_Interrupt, &m_Timer, &m_ActLED),
   dwhci_device_(&m_Interrupt, &m_Timer, TRUE),
   vchiq_(&m_Memory, &m_Interrupt),
   display_(nullptr),
   keyboard_(nullptr),
   sound_(nullptr),
   emulation_(&m_Memory, &m_Logger, &m_Timer)
   
{
   display_ = new DisplayPi(&m_Logger, &m_Timer);
   keyboard_ = new KeyboardPi(&m_Logger, &dwhci_device_, &m_DeviceNameService);
   cpu_throttle_ = new CCPUThrottle();
   exception_handler_ = new CExceptionHandler;
}

CKernel::~CKernel (void)
{
   delete exception_handler_;
   delete cpu_throttle_;
   delete keyboard_;
   delete display_;
   delete sound_;
}

boolean CKernel::Initialize (void)
{

   boolean bOK = TRUE;

   if (bOK)
   {
      bOK = display_->Initialization();
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
         pTarget = display_->GetScreenDevice();
      }*/
      bOK = m_Logger.Initialize(pTarget);
      m_Logger.Write("Kernel", LogNotice, "Initialisation done for log.");
      
   }
   
   if (bOK)
   {
      bOK = m_Interrupt.Initialize();
      m_Logger.Write("Kernel", LogNotice, "Interrupt initialization done : %i", bOK);
   }

   if (bOK)
   {
      bOK = m_Timer.Initialize();
      m_Logger.Write("Kernel", LogNotice, "Timer initialization done : %i", bOK);
   }

   if (bOK)
   {
      bOK = m_EMMC.Initialize();
      m_Logger.Write("Kernel", LogNotice, "EMMC initialization done : %i", bOK);
   }

#ifndef USE_QEMU_SUGARPI
   m_Logger.Write("Kernel", LogNotice, "Initialisation of VCHIQ.....");
   if (bOK)
   {
      bOK = vchiq_.Initialize();
   }
   m_Logger.Write("Kernel", LogNotice, "Initialisationfoe Done : %i", bOK);

   if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK)
   {
      m_Logger.Write("Kernel", LogPanic, "Cannot mount drive: %s", DRIVE);
   }

#ifdef USE_VCHIQ_SOUND   
   sound_ = new SoundPi(&m_Logger, &vchiq_, &scheduler_);
#else
   sound_ = new SoundPi(&m_Logger, &m_Interrupt, &scheduler_);
#endif

   m_Logger.Write("Kernel", LogNotice, "Creating SoundPI");
   sound_->Initialize();
   m_Logger.Write("Kernel", LogNotice, "SoundPI Initialized !");

   if (bOK)
   {
      bOK = keyboard_->Initialize();
   }
#endif
   if (bOK)
   {
      m_Logger.Write("Kernel", LogNotice, "Initialisation emulation.....");
      bOK = emulation_.Initialize(display_, sound_, keyboard_, &scheduler_);	// must be initialized at last
      m_Logger.Write("Kernel", LogNotice, "Initialisation done done !");
   }

   m_Logger.Write("Kernel", LogNotice, "EDID...");
   display_->ListEDID();
   m_Logger.Write("Kernel", LogNotice, "EDID Done !");

   m_Logger.Write("Kernel", LogNotice, "Initialisation done. Waiting for CPUThrottle %i", bOK ? 1 : 0);

   CCPUThrottle::Get()->SetSpeed(CPUSpeedMaximum);

   
   m_Logger.Write("Kernel", LogNotice, "Initialisation done. Result = %i - CPU Speed max value : %i", bOK?1:0, CCPUThrottle::Get()->GetMaxClockRate());
   return bOK;
}

TShutdownMode CKernel::Run (void)
{

   m_Logger.Write("Kernel", LogNotice, "Entering running mode...");

   //while (1)
   {
      emulation_.Run(0);
      //scheduler_.Yield();
   }



   /*unsigned nCelsiusOldTmp = 0;
	while (1)
	{
      
      // 200ms 
      motherboard_emulation_->StartOptimizedPlus(4000*50*20);

      // Temperature
      unsigned nCelsius = CCPUThrottle::Get()->GetTemperature();
      if (nCelsiusOldTmp != nCelsius)
      {
         m_Logger.Write("Kernel", LogNotice, "Temperature = %i", nCelsius);
         nCelsiusOldTmp = nCelsius;
      }
     
      // Menu launched ?
      if (keyboard_->IsSelect())
      {
         // do it !
         CCPUThrottle::Get()->SetSpeed(CPUSpeedLow);

         ScreenMenu menu(&m_Logger, display_, keyboard_, motherboard_emulation_);
         menu.Handle();

         keyboard_->ReinitSelect();
         CCPUThrottle::Get()->SetSpeed(CPUSpeedMaximum);
      }
      else
      {
         // Timing computation 
         static unsigned old = 0;
         unsigned elapsed = m_Timer.GetTicks();

         m_Logger.Write("Kernel", LogNotice, "Time for 1s emulation : %i ticks -> %i ms", elapsed - old, (elapsed - old));
         old = elapsed;

      }
   }
   */
  CTimer::Get ()->MsDelay (2000);
  m_Logger.Write("Kernel", LogNotice, "Exiting : Halt");
	return ShutdownHalt;
}
