#pragma once

#include <string>
#include <vector>

#ifdef  __circle__
   #include <circle/logger.h>
#else
   #include "CLogger.h"
#endif

#include "CPCCore/CPCCoreEmu/IKeyboard.h"
#include "KeyboardHardwareImplemetation.h"


#define MAX_GAMEPADS	2


class KeyboardPi : public IKeyboardHandler
{
public:
   KeyboardPi(CLogger* logger);
   virtual ~KeyboardPi();

   void SetHard(KeyboardHardwareImplemetation* hard_imp);
    bool Initialize();
    void InitKeyboard (unsigned char key_map[10][8]);
    void LoadKeyboard(const char* path);

    virtual unsigned char GetKeyboardMap(int index);
    void UpdatePlugnPlay();
    void Init(bool* register_replaced);
    void ForceKeyboardState(unsigned char key_states[10]) {};

   bool AddAction (GamepadActionHandler* action, unsigned nDeviceIndex, bool update_map = false);
   void CheckActions(unsigned nDeviceIndex) ;

   void PressKey(unsigned int scancode);
   void UnpressKey(unsigned int scancode);

   void ClearBuffer();
   bool IsSelect();
   bool IsButton(TGamePadButton button);
   bool IsAction();
   void ReinitSelect();

   void LoadGameControllerDB();

   //static void GamePadRemovedHandler (CDevice *pDevice, void *pContext);
   static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
   //static void KeyboardRemovedHandler (CDevice *pDevice, void *pContext);

   unsigned int *GetActionButtons() {return &action_buttons_;   }
   bool* GetSelect() { return &select_; }
   TGamePadState* GetGamepadState() {return gamepad_state_;}
   TGamePadState* GetGamepadStateBuffered() { return gamepad_state_buffered_; }
   unsigned char* GetKeyboardLine() { return keyboard_lines_; }
   GamepadDef** GetGamepadActive() {
      return gamepad_active_;
   }

protected:

   KeyboardHardwareImplemetation* hard_imlementation_;
   //void UpdateKeyboardMap();

   CLogger*          logger_;
   /*CDeviceNameService* device_name_service_;
   CUSBHCIDevice		*dwhci_device_;
   CUSBGamePadDevice* gamepad_[MAX_GAMEPADS];
   CUSBKeyboardDevice* keyboard_;

   CSpinLock         mutex_;
   */
   unsigned int      action_buttons_;
   bool              select_;

   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];
   
   
   // Keyboard definition
   unsigned char keyboard_lines_ [10];

   static KeyboardPi* this_ptr_;

   std::vector<GamepadDef*> gamepad_list_;
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];

   struct RawToCPC
   {
      int line_number;
      unsigned char* line_index;
      unsigned char bit;
   };
   RawToCPC raw_to_cpc_map_[0x100];
   unsigned char old_raw_keys_[6];
   unsigned char old_modifier_;
};