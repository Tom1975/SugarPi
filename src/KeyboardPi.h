#pragma once

#include <string>
#include <vector>

#ifdef  __circle__
   #include <circle/logger.h>
#else
   #include "CLogger.h"
#endif

#include "CPCCore/CPCCoreEmu/IKeyboard.h"
#include "CPCCore/CPCCoreEmu/KeyboardHandler.h"
#include "KeyboardHardwareImplemetation.h"


#define MAX_GAMEPADS	2


class KeyboardPi : public IKeyboardHandler
{
public:
   KeyboardPi(CLogger* logger);
   virtual ~KeyboardPi();

    bool Initialize();

    virtual unsigned char GetKeyboardMap(int index);
    virtual void UpdatePlugnPlay();
    virtual void Init(bool* register_replaced);
    virtual void ForceKeyboardState(unsigned char key_states[10]) {};

   virtual bool AddAction (GamepadActionHandler* action, unsigned nDeviceIndex, bool update_map = false);
   virtual void CheckActions(unsigned nDeviceIndex) ;

   virtual void PressKey(unsigned int scancode);
   virtual void UnpressKey(unsigned int scancode);

      virtual void ValidateKeyboardMap() {
         handler_.ValidateKeyboardMap();
      }
   virtual void ClearBuffer();
   virtual bool IsSelect();
   virtual bool IsButton(TGamePadButton button);
   virtual bool IsAction();
   virtual bool IsFunctionKey(unsigned int function_number);
   virtual unsigned int GetFunctionKey() {
      return function_keys_;
   }
   virtual void ReinitSelect();

   KeyboardHandler* GetHandler() {
      return &handler_;
   }
   virtual void LoadGameControllerDB();

   std::vector<GamepadDef*> gamepad_list_;

protected:
   KeyboardHandler   handler_;

   CLogger*          logger_;
   unsigned int      action_buttons_;
   bool              select_;
   unsigned int      function_keys_;
   unsigned char*    keyboard_lines_;
   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];

   // Keyboard definition
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];

};