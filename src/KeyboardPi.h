#pragma once

//
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/devicenameservice.h>

#include "CPCCore/CPCCoreEmu/IKeyboard.h"
#include "CPCCore/CPCCoreEmu/simple_string.h"
#include "CPCCore/CPCCoreEmu/simple_vector.hpp"

#define MAX_GAMEPADS	2


class IGamepadPressed
{
   public:
      virtual bool IsPressed(TGamePadState*) = 0;
};

class GamepadActionHandler
{
   public:
      GamepadActionHandler ();
      virtual ~GamepadActionHandler();

      virtual void AddHandler(IGamepadPressed*);
      virtual bool IsPressed(TGamePadState*);

   protected:
      struct Handler
      {
         IGamepadPressed* action_handler;
         Handler* next_handler;
      };
      Handler* handler_;
};

class GamepadDef
{
   public:
      GamepadDef() : supported_controls_(0), vid(0), pid(0), version(0)
      {
      }
      virtual ~GamepadDef()
      {
      }

      unsigned int SetValue(const char* key, const char* value);
      IGamepadPressed* CreateFunction(const char* value, bool min = true);

      // Attributes
      std::string name;

      unsigned int supported_controls_;
      unsigned int vid;
      unsigned int pid;
      unsigned int version;

      // Values for buttons
      bool IsPressed ( TGamePadState* );

      GamepadActionHandler game_pad_button_X;
      GamepadActionHandler game_pad_button_A;
      GamepadActionHandler game_pad_button_up;
      GamepadActionHandler game_pad_button_down;
      GamepadActionHandler game_pad_button_left;
      GamepadActionHandler game_pad_button_right;
      GamepadActionHandler game_pad_button_start;
      GamepadActionHandler game_pad_button_select;

};

class KeyboardPi : public IKeyboardHandler
{
public:
   KeyboardPi(CLogger* logger, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service);
   virtual ~KeyboardPi();

   virtual bool Initialize();
   virtual unsigned char GetKeyboardMap(int index);
   virtual void UpdatePlugnPlay();
   virtual void Init(bool* register_replaced);
   virtual void ForceKeyboardState(unsigned char key_states[10]) {};

   bool AddAction (GamepadActionHandler* action, unsigned nDeviceIndex);
   void CheckActions(unsigned nDeviceIndex) ;

   virtual void ClearBuffer();
   virtual bool IsSelect();
   virtual bool IsDown();
   virtual bool IsUp();
   virtual bool IsAction();
   virtual void ReinitSelect();

   static void GamePadRemovedHandler (CDevice *pDevice, void *pContext);
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);

protected:
   void LoadGameControllerDB();
   GamepadDef* LookForDevice (const TUSBDeviceDescriptor* descriptor);

   CLogger*          logger_;
   CDeviceNameService* device_name_service_;
   CUSBHCIDevice		*dwhci_device_;
   CUSBGamePadDevice* gamepad_[MAX_GAMEPADS];

   CSpinLock         mutex_;
   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];
   unsigned          action_buttons_;

   static KeyboardPi* this_ptr_;

   std::vector<GamepadDef*> gamepad_list_;
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];
   bool              select_;
};