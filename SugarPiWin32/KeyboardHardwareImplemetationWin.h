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

      
protected:
      void CodeAction(long keycode, bool activated);

      KeyboardPi*       keyboard_;
      KeyboardHandler   handler_;

      bool*             select_;
      unsigned int*     action_buttons_;
      unsigned char* keyboard_lines_;

      GamepadDef** gamepad_active_;
};
