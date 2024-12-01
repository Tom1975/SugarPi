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

   virtual void ClearBuffer();
   virtual bool IsSelect();
   virtual bool IsButton(TGamePadButton button);
   virtual bool IsAction();
   virtual void ReinitSelect();

   KeyboardHandler* GetHandler() {
      return &handler_;
   }
   virtual void LoadGameControllerDB();

   unsigned int *GetActionButtons() {return &action_buttons_;   }
   bool* GetSelect() { return &select_; }
   TGamePadState* GetGamepadState() {return gamepad_state_;}
   TGamePadState* GetGamepadStateBuffered() { return gamepad_state_buffered_; }
   GamepadDef** GetGamepadActive() {
      return gamepad_active_;
   }

   std::vector<GamepadDef*> gamepad_list_;

protected:
   KeyboardHandler   handler_;

   CLogger*          logger_;
   unsigned int      action_buttons_;
   bool              select_;

   TGamePadState	   gamepad_state_[MAX_GAMEPADS];
   TGamePadState	   gamepad_state_buffered_[MAX_GAMEPADS];
   
   
   // Keyboard definition
   GamepadDef*       gamepad_active_[MAX_GAMEPADS];

};