#pragma once

//
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbkeyboard.h>
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
      GamepadActionHandler (unsigned char* line, unsigned int index, unsigned char* line2, unsigned int index2);
      virtual ~GamepadActionHandler();

      void AddHandler(IGamepadPressed*);
      bool IsPressed(TGamePadState*);
      void UpdateMap(unsigned int nDeviceIndex, bool pressed);

   protected:
      struct Handler
      {
         IGamepadPressed* action_handler;
         Handler* next_handler;
      };
      Handler* handler_;
      unsigned char* line_[2];
      unsigned int index_[2];
};

class GamepadDef
{
   public:
      GamepadDef(unsigned char* keymap);
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

    bool Initialize();
    void InitKeyboard (unsigned char key_map[10][8]);
    void LoadKeyboard(const char* path);

    virtual unsigned char GetKeyboardMap(int index);
    void UpdatePlugnPlay();
    void Init(bool* register_replaced);
    void ForceKeyboardState(unsigned char key_states[10]) {};

   bool AddAction (GamepadActionHandler* action, unsigned nDeviceIndex, bool update_map = false);
   void CheckActions(unsigned nDeviceIndex) ;

   void ClearBuffer();
   bool IsSelect();
   bool IsButton(TGamePadButton button);
   bool IsAction();
   void ReinitSelect();

   void LoadGameControllerDB();

   static void GamePadRemovedHandler (CDevice *pDevice, void *pContext);
   static void GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState);
   static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
   static void KeyboardRemovedHandler (CDevice *pDevice, void *pContext);
protected:
   GamepadDef* LookForDevice (const TUSBDeviceDescriptor* descriptor);
   //void UpdateKeyboardMap();

   CLogger*          logger_;
   CDeviceNameService* device_name_service_;
   CUSBHCIDevice		*dwhci_device_;
   CUSBGamePadDevice* gamepad_[MAX_GAMEPADS];
   CUSBKeyboardDevice* keyboard_;

   CSpinLock         mutex_;

   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];
   unsigned          action_buttons_;

   // Keyboard definition
   unsigned char keyboard_lines_ [10];

   static KeyboardPi* this_ptr_;

   std::vector<GamepadDef*> gamepad_list_;
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];
   bool              select_;

   struct RawToCPC
   {
      unsigned char* line_index;
      unsigned char bit;
   };
   RawToCPC raw_to_cpc_map_[0x100];
   unsigned char old_raw_keys_[6];
   unsigned char old_modifier_;
};