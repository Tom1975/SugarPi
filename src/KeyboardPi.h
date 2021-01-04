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

class GamepadDef
{
   public:
      GamepadDef():game_pad_button_X(nullptr),game_pad_button_A(nullptr),game_pad_button_up(nullptr),game_pad_button_down(nullptr),
         game_pad_button_left(nullptr),game_pad_button_right(nullptr),game_pad_button_start(nullptr),game_pad_button_select(nullptr)
      {
      }
      virtual ~GamepadDef()
      {
         delete game_pad_button_X;
         delete game_pad_button_A;
         delete game_pad_button_up;
         delete game_pad_button_down;
         delete game_pad_button_left;
         delete game_pad_button_right;
         delete game_pad_button_start;
         delete game_pad_button_select;
      }

      bool SetValue(const char* key, const char* value);
      IGamepadPressed* CreateFunction(const char* value);

      // Attributes
      std::string name;

      unsigned int vid;
      unsigned int pid;
      unsigned int version;

      // Values for buttons
      bool IsPressed ( TGamePadState* );

      IGamepadPressed* game_pad_button_X;
      IGamepadPressed* game_pad_button_A;
      IGamepadPressed* game_pad_button_up;
      IGamepadPressed* game_pad_button_down;
      IGamepadPressed* game_pad_button_left;
      IGamepadPressed* game_pad_button_right;
      IGamepadPressed* game_pad_button_start;
      IGamepadPressed* game_pad_button_select;

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

   bool AddAction (IGamepadPressed* action, unsigned nDeviceIndex);
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

   TGamePadState	    gamepad_state_[MAX_GAMEPADS];
   TGamePadState	    gamepad_state_buffered_[MAX_GAMEPADS];
   unsigned           action_buttons_;

   static KeyboardPi* this_ptr_;

   std::vector<GamepadDef*> gamepad_list_;
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];
   bool              select_;
};