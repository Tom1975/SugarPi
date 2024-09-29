#pragma once

#include <string>
#include <vector>

#include "CLogger.h"
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

   unsigned int      action_buttons_;
   bool              select_;

   //static void GamePadRemovedHandler (CDevice *pDevice, void *pContext);
   static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
   //static void KeyboardRemovedHandler (CDevice *pDevice, void *pContext);


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
   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];
   
   
   // Keyboard definition
   unsigned char keyboard_lines_ [10];

   static KeyboardPi* this_ptr_;

   std::vector<GamepadDef*> gamepad_list_;
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];

   struct RawToCPC
   {
      unsigned char* line_index;
      unsigned char bit;
   };
   RawToCPC raw_to_cpc_map_[0x100];
   unsigned char old_raw_keys_[6];
   unsigned char old_modifier_;
};