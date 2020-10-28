#pragma once

//
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/devicenameservice.h>

#include "CPCCore/CPCCoreEmu/IKeyboard.h"


class KeyboardPi : public IKeyboardHandler
{
public:
   KeyboardPi(CLogger* logger, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service);
   virtual ~KeyboardPi();

   virtual bool Initialize();
   virtual unsigned char GetKeyboardMap(int index);
   virtual void Init(bool* register_replaced);
   virtual void ForceKeyboardState(unsigned char key_states[10]) {};

   virtual void ClearBuffer();
   virtual bool IsSelect();
   virtual bool IsDown();
   virtual bool IsUp();
   virtual bool IsAction();
   virtual void ReinitSelect();
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);

protected:
   CLogger*          logger_;
   CDeviceNameService* device_name_service_;
   CUSBHCIDevice		*dwhci_device_;
   CUSBGamePadDevice* gamepad_;

   TGamePadState	    gamepad_state_;
   TGamePadState	    gamepad_state_buffered_;
   unsigned           action_buttons_;

   static KeyboardPi* this_ptr_;

   bool              select_;
};