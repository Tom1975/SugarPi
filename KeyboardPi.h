#pragma once

//
#include <circle/usb/dwhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/devicenameservice.h>

#include "CPCCore/CPCCoreEmu/IKeyboard.h"


class KeyboardPi : public IKeyboardHandler
{
public:
   KeyboardPi(CLogger* logger, CDWHCIDevice* dwhci_device, CDeviceNameService* device_name_service);
   virtual ~KeyboardPi();

   virtual bool Initialize();
   virtual unsigned char GetKeyboardMap(int index);
   virtual void Init(bool* register_replaced);
   virtual bool IsSelect();
   virtual void ReinitSelect();
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);

protected:
   CLogger*          logger_;
   CDeviceNameService* device_name_service_;
   CDWHCIDevice		*dwhci_device_;
   CUSBGamePadDevice* gamepad_;
   TGamePadState	    gamepad_state_;

   static KeyboardPi* this_ptr_;

   bool              select_;
};