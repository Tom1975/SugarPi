//
#include "KeyboardHardwareImplemetationWin.h"

#include <Windows.h>

KeyboardHardwareImplemetationWin::KeyboardHardwareImplemetationWin(KeyboardPi* keyboard) : keyboard_(keyboard)
{
   for (unsigned i = 0; i < MAX_GAMEPADS; i++)
   {
      gamepad_active_[i] = nullptr;
   }

   GamepadDef* def = new GamepadDef(keyboard_lines_);
   gamepad_list_.push_back(def);
   gamepad_active_[0] = gamepad_list_[0];

}

KeyboardHardwareImplemetationWin::~KeyboardHardwareImplemetationWin()
{

}

void KeyboardHardwareImplemetationWin::Initialize()
{

}

void KeyboardHardwareImplemetationWin::UpdatePlugnPlay()
{

}

#define action(y) keyboard_->action_buttons_  = activated ? (keyboard_->action_buttons_ |y):(keyboard_->action_buttons_&=~y)
void KeyboardHardwareImplemetationWin::CodeAction(long keycode, bool activated)
{
   // todo : hardcoded values
   switch (keycode)
   {
   case VK_TAB: keyboard_->select_ = activated; action(GamePadButtonSelect); break;
   case VK_SCROLL: gamepad_active_[0]->game_pad_button_start.UpdateMap(0, activated); action(GamePadButtonStart); break;
   case VK_UP: gamepad_active_[0]->game_pad_button_up.UpdateMap(0, activated); action(GamePadButtonUp);  break;
   case VK_DOWN: gamepad_active_[0]->game_pad_button_down.UpdateMap(0, activated); action(GamePadButtonDown); break;
   case VK_LEFT: gamepad_active_[0]->game_pad_button_left.UpdateMap(0, activated); action(GamePadButtonLeft); break;
   case VK_RIGHT: gamepad_active_[0]->game_pad_button_right.UpdateMap(0, activated); action(GamePadButtonRight); break;
      //case VK_SHIFT:gamepad_active_[0]->game_pad_button_A.UpdateMap(0, activated); action(GamePadButtonA);break;
   case VK_CONTROL:gamepad_active_[0]->game_pad_button_X.UpdateMap(0, activated); action(GamePadButtonX); break;
   default:
      if (activated)
         keyboard_->PressKey(keycode);
      else
         keyboard_->UnpressKey(keycode);

            
      /*if (activated)handler_.CharPressed(keycode);
      else handler_.CharReleased(keycode);
      memcpy(keyboard_lines_, handler_.GetKeyboardState(), sizeof(keyboard_lines_));*/

      break;
   }
}


void KeyboardHardwareImplemetationWin::Presskey(long keyCode)
{
   CodeAction(keyCode, true);
}

void KeyboardHardwareImplemetationWin::Unpresskey(long keyCode)
{
   CodeAction(keyCode, false);
}