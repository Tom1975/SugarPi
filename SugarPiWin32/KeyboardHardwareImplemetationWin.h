#pragma once
//
#include <vector>

#include "KeyboardDefine.h"
#include "KeyboardHandler.h"
#include "KeyboardPi.h"

//
class KeyboardHardwareImplemetationWin : public KeyboardPi
{
   public:
      KeyboardHardwareImplemetationWin(CLogger* logger);
      virtual ~KeyboardHardwareImplemetationWin();

      virtual void Initialize();
      virtual void UpdatePlugnPlay();

      void Presskey(long keyCode);
      void Unpresskey(long keyCode);

      void CodeActionSpecial(long keycode, bool activated);
      
protected:
      void CodeAction(long keycode, bool activated);

      bool*             select_;
      unsigned int*     action_buttons_;

      GamepadDef** gamepad_active_;
};
