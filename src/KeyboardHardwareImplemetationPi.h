#pragma once
//
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/devicenameservice.h>

#include "KeyboardPi.h"
#include "KeyboardHardwareImplemetation.h"

//
class KeyboardHardwareImplemetationPi : public KeyboardHardwareImplemetation
{
public:
   KeyboardHardwareImplemetationPi(KeyboardPi* keyboard, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service);
   virtual ~KeyboardHardwareImplemetationPi();

   virtual void Initialize();
   virtual void UpdatePlugnPlay();

   static void GamePadRemovedHandler(CDevice* pDevice, void* pContext);
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);
   static void KeyboardRemovedHandler(CDevice* pDevice, void* pContext);

protected:

   bool*             select_;
   unsigned int*     action_buttons_;
   unsigned char*    keyboard_lines_;
   GamepadDef**      gamepad_active_;
   TGamePadState**	gamepad_state_;

   static void KeyStatusHandlerRaw(unsigned char ucModifiers, const unsigned char RawKeys[6]);
   GamepadDef* LookForDevice (const TUSBDeviceDescriptor* descriptor);

   CDeviceNameService* device_name_service_;
   CUSBHCIDevice* dwhci_device_;
   CUSBGamePadDevice* gamepad_[MAX_GAMEPADS];
   static CUSBKeyboardDevice* keyboard_;

   CSpinLock         mutex_;
   static KeyboardPi*       keyboardPi_;
};
