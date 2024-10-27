//
#include "KeyboardHardwareImplemetationWin.h"

#include <Windows.h>

KeyboardHardwareImplemetationWin::KeyboardHardwareImplemetationWin(KeyboardPi* keyboard) : keyboard_(keyboard)
{
   select_ = keyboard_->GetSelect();
   action_buttons_ = keyboard_->GetActionButtons();
   gamepad_active_ = keyboard_->GetGamepadActive();
   keyboard_lines_ = keyboard_->GetKeyboardLine();
   for (unsigned i = 0; i < MAX_GAMEPADS; i++)
   {
      gamepad_active_[i] = nullptr;
   }

   GamepadDef* def = new GamepadDef(keyboard_lines_);
   //gamepad_list_.push_back(def);
   gamepad_active_[0] = def;// gamepad_list_[0];

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

#define action(y) *action_buttons_  = activated ? (*action_buttons_ |y):(*action_buttons_&=~y)


void KeyboardHardwareImplemetationWin::CodeActionSpecial(long keycode, bool activated)
{
   switch (keycode)
   {
   case VK_LWIN: *select_ = activated; action(GamePadButtonSelect); break;
   case VK_SCROLL: if (gamepad_active_[0] != nullptr) { gamepad_active_[0]->game_pad_button_start.UpdateMap(0, activated); action(GamePadButtonStart); } break;
   case VK_UP: if (gamepad_active_[0] != nullptr) {
      gamepad_active_[0]->game_pad_button_up.UpdateMap(0, activated); action(GamePadButtonUp);
   }  break;
   case VK_DOWN: if (gamepad_active_[0] != nullptr) {
      gamepad_active_[0]->game_pad_button_down.UpdateMap(0, activated); action(GamePadButtonDown);
   } break;
   case VK_LEFT: if (gamepad_active_[0] != nullptr) {
      gamepad_active_[0]->game_pad_button_left.UpdateMap(0, activated); action(GamePadButtonLeft);
   } break;
   case VK_RIGHT: if (gamepad_active_[0] != nullptr) {
      gamepad_active_[0]->game_pad_button_right.UpdateMap(0, activated); action(GamePadButtonRight);
   } break;
                //case VK_SHIFT:gamepad_active_[0]->game_pad_button_A.UpdateMap(0, activated); action(GamePadButtonA);break;
   case VK_CONTROL:if (gamepad_active_[0] != nullptr) {
      gamepad_active_[0]->game_pad_button_X.UpdateMap(0, activated); action(GamePadButtonX);
   } break;
   }
}

void KeyboardHardwareImplemetationWin::CodeAction(long keycode, bool activated)
{
   
   if (activated)
      keyboard_->PressKey(keycode);
   else
      keyboard_->UnpressKey(keycode);
}


void KeyboardHardwareImplemetationWin::Presskey(long keyCode)
{
      CodeAction(keyCode, true);
}

void KeyboardHardwareImplemetationWin::Unpresskey(long keyCode)
{
   CodeAction(keyCode, false);
}