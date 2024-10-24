//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#ifndef _kernel_h
#define _kernel_h

#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/types.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/cputhrottle.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/exceptionhandler.h>
#include <circle/sched/scheduler.h>

#include <SDCard/emmc.h>
#include <vc4/vchiq/vchiqdevice.h>
#include <vc4/sound/vchiqsounddevice.h>
#include <fatfs/ff.h>

#include <CPCCore/CPCCoreEmu/Motherboard.h>
#include <CPCCore/CPCCoreEmu/SoundMixer.h>

#include "DisplayPi.h"
#include "DisplayPiImp.h"
#include "KeyboardPi.h"
#include "KeyboardHardwareImplemetationPi.h"
#include "SoundPi.h"
#include "emulation.h"


enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};


class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);
   int LoadCprFromBuffer(unsigned char* buffer, int size);

	TShutdownMode Run (void);

protected:
   //void GetFolderCart();

private:
	// do not change this order
	CMemorySystem		m_Memory;
	CActLED				m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
   CSerialDevice		m_Serial;
   CTimer			   m_Timer;
   CLogger			   m_Logger;
   CInterruptSystem  m_Interrupt;
   CCPUThrottle      *cpu_throttle_;
   CEMMCDevice		   m_EMMC;
   CUSBHCIDevice		      dwhci_device_;
   CExceptionHandler * exception_handler_;
   FATFS			      m_FileSystem;

   CScheduler		   scheduler_;
   CVCHIQDevice		vchiq_;
   //SoundMixer        *sound_mixer_;
	//Motherboard       *motherboard_emulation_;
	DisplayPiImp      *display_;
   KeyboardPi        *keyboard_;
   KeyboardHardwareImplemetationPi* keyboard_imp_;
   SoundPi*          sound_;

   Emulation         emulation_;

};

#endif
