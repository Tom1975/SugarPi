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

CKernel::CKernel (void) 
:
#if AARCH != 64			// non-MMU mode currently does not work with AArch64
	m_Memory (FALSE),	// set this to TRUE to enable MMU and to boost performance
#endif
	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
   m_Timer(&m_Interrupt),
   sound_mixer_(),
   m_Logger(m_Options.GetLogLevel(), &m_Timer),
   motherboard_emulation_ (&sound_mixer_)
{
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
   boolean bOK = TRUE;

   if (bOK)
   {
      bOK = m_Screen.Initialize();
   }
   if (bOK)
   {
      bOK = m_Serial.Initialize(115200);
   }

   if (bOK)
   {
      CDevice* pTarget = m_DeviceNameService.GetDevice(m_Options.GetLogDevice(), FALSE);
      if (pTarget == 0)
      {
         pTarget = &m_Screen;
      }
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

   m_Logger.Write("Kernel", LogNotice, "Initialisation done.");
   return bOK;
}

TShutdownMode CKernel::Run (void)
{
   m_Logger.Write("Kernel", LogNotice, "Entering running mode...");
	// draw rectangle on screen
	for (unsigned nPosX = 0; nPosX < m_Screen.GetWidth (); nPosX++)
	{
		m_Screen.SetPixel (nPosX, 0, NORMAL_COLOR);
		m_Screen.SetPixel (nPosX, m_Screen.GetHeight ()-1, NORMAL_COLOR);
	}
	for (unsigned nPosY = 0; nPosY < m_Screen.GetHeight (); nPosY++)
	{
		m_Screen.SetPixel (0, nPosY, NORMAL_COLOR);
		m_Screen.SetPixel (m_Screen.GetWidth ()-1, nPosY, NORMAL_COLOR);
	}

	// draw cross on screen
	for (unsigned nPosX = 0; nPosX < m_Screen.GetWidth (); nPosX++)
	{
		unsigned nPosY = nPosX * m_Screen.GetHeight () / m_Screen.GetWidth ();

		m_Screen.SetPixel (nPosX, nPosY, NORMAL_COLOR);
		m_Screen.SetPixel (m_Screen.GetWidth ()-nPosX-1, nPosY, NORMAL_COLOR);
	}

   // Init motherboard
   m_Logger.Write("Kernel", LogNotice, "Init motherboard...");
   motherboard_emulation_.InitMotherbard( nullptr, nullptr, &display_, nullptr, nullptr, nullptr);
   m_Logger.Write("Kernel", LogNotice, "Done !");
   // Set configuration
   m_Logger.Write("Kernel", LogNotice, "On/Off...");
   motherboard_emulation_.OnOff();
   m_Logger.Write("Kernel", LogNotice, "Done !");

	// check the blink frequency without and with MMU (see option in constructor above)
	while (1)
	{
		m_ActLED.On ();
		for (volatile unsigned i = 1; i <= 500000000; i++)
		{
			// just wait
		}

		m_ActLED.Off ();
		for (volatile unsigned i = 1; i <= 10000000; i++)
		{
			// just wait
		}
	}

	return ShutdownHalt;
}
