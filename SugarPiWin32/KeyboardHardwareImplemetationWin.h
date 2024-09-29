#pragma once
//
#include <vector>

#include "KeyboardHardwareImplemetation.h"
#include "KeyboardDefine.h"

#include "KeyboardHandler.h"
#include "KeyboardPi.h"

//
class KeyboardHardwareImplemetationWin : public KeyboardHardwareImplemetation
{
   public:
      KeyboardHardwareImplemetationWin(KeyboardPi* keyboard);
      virtual ~KeyboardHardwareImplemetationWin();

      virtual void Initialize();
      virtual void UpdatePlugnPlay();

      void Presskey(long keyCode);
      void Unpresskey(long keyCode);

      //bool              select_;
protected:
      void CodeAction(long keycode, bool activated);

      KeyboardPi*    keyboard_;
      KeyboardHandler   handler_;
      unsigned char keyboard_lines_[10];

      std::vector<GamepadDef*> gamepad_list_;
      GamepadDef* gamepad_active_[MAX_GAMEPADS];

      //unsigned int      action_buttons_;

};
