#pragma once
//
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/devicenameservice.h>

#include "KeyboardPi.h"

//
class KeyboardHardwareImplemetationPi : public KeyboardPi
{
public:
   KeyboardHardwareImplemetationPi(CLogger* logger, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service);
   virtual ~KeyboardHardwareImplemetationPi();

   virtual bool Initialize();
   virtual void UpdatePlugnPlay();

   static void GamePadRemovedHandler(CDevice* pDevice, void* pContext);
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);
   static void KeyboardRemovedHandler(CDevice* pDevice, void* pContext);
   static void KeyStatusHandlerRaw(unsigned char ucModifiers, const unsigned char RawKeys[6]);

   static KeyboardPi*       keyboardPi_;

protected:

   bool*             select_;
   unsigned int*     action_buttons_;
   unsigned char*    keyboard_lines_;
   GamepadDef**      gamepad_active_;
   TGamePadState*    gamepad_state_buffered_;
   TGamePadState*	   gamepad_state_;

   unsigned char old_modifier_;
   unsigned char old_raw_keys_[6];

   GamepadDef* LookForDevice (const TUSBDeviceDescriptor* descriptor);

   CDeviceNameService* device_name_service_;
   CUSBHCIDevice* dwhci_device_;
   CUSBGamePadDevice* gamepad_[MAX_GAMEPADS];
   static CUSBKeyboardDevice* keyboard_;

   CSpinLock         mutex_;
};
