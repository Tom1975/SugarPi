#pragma once
//
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/devicenameservice.h>

#include "KeyboardHardwareImplemetation.h"

//
class KeyboardHardwareImplemetationPi : public KeyboardHardwareImplemetation
{
public:
   KeyboardHardwareImplemetationPi(CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service);
   virtual ~KeyboardHardwareImplemetationPi();

   virtual bool Initialize();
   virtual void UpdatePlugnPlay();

   static void GamePadRemovedHandler(CDevice* pDevice, void* pContext);
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);
   static void KeyboardRemovedHandler(CDevice* pDevice, void* pContext);

protected:

   static void KeyStatusHandlerRaw(unsigned char ucModifiers, const unsigned char RawKeys[6]);
   GamepadDef* LookForDevice (const TUSBDeviceDescriptor* descriptor);

   CDeviceNameService* device_name_service_;
   CUSBHCIDevice* dwhci_device_;
   CUSBGamePadDevice* gamepad_[MAX_GAMEPADS];
   CUSBKeyboardDevice* keyboard_;

   CSpinLock         mutex_;

   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];
};
