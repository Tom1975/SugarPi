//
#include "KeyboardHardwareImplemetationPi.h"

#include <memory.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>


unsigned shift_l_modifier_ = 0x02;
unsigned shift_r_modifier_ = 0x20;
unsigned ctrl_modifier_ = 0x01;
unsigned copy_modifier_ = 0x04;

static CSpinLock   mutex_;
static void Lock() { mutex_.Acquire(); }
static void Unlock() { mutex_.Release(); }

KeyboardHardwareImplemetationPi::keyboardPi_ = nullptr;

KeyboardHardwareImplemetationPi::KeyboardHardwareImplemetationPi(KeyboardPi* keyboard, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service):
   device_name_service_(device_name_service),
   dwhci_device_(dwhci_device)
{
   keyboardPi_ = keyboard;
   select_ = keyboardPi_->GetSelect();
   action_buttons_ = keyboardPi_->GetActionButtons();
   keyboard_lines_ = keyboardPi_->GetKeyboardLine();
   gamepad_state_ = keyboardPi_->GetGamepadState();
   gamepad_active_ = keyboardPi_->GetGamepadActive();

	for (unsigned i = 0; i < MAX_GAMEPADS; i++)
	{
		gamepad_[i] = 0;
	}


}

KeyboardHardwareImplemetationPi::~KeyboardHardwareImplemetationPi()
{

}


void KeyboardHardwareImplemetationPi::Initialize()
{
   if (dwhci_device_->Initialize() == false)
   {
      CLogger::Get ()->Write("Keyboard", LogPanic, "Initialize failed !");
   }
   CLogger::Get ()->Write("Keyboard", LogNotice, "Initialize done.");

}

void KeyboardHardwareImplemetationPi::UpdatePlugnPlay()
{
   boolean bUpdated = dwhci_device_->UpdatePlugAndPlay();

   if (bUpdated)
   {
      // Gamepad
      for (unsigned nDevice = 1; (nDevice <= MAX_GAMEPADS); nDevice++)
      {
         if (gamepad_[nDevice - 1] != 0)
         {
            continue;
         }

         gamepad_[nDevice - 1] = (CUSBGamePadDevice*)device_name_service_->GetDevice("upad", nDevice, FALSE);
         if (gamepad_[nDevice - 1] == 0)
         {
            continue;
         }

         // Get gamepad names
         CString* gamepad_name = gamepad_[nDevice - 1]->GetDevice()->GetNames();
         const TUSBDeviceDescriptor* descriptor = gamepad_[nDevice - 1]->GetDevice()->GetDeviceDescriptor();

         CLogger::Get ()->Write("Keyboard", LogNotice, "Gamepad : %s - VID=%X; PID=%X; bcdDevice = %X", (const char*)(*gamepad_name), descriptor->idVendor,
            descriptor->idProduct, descriptor->bcdDevice);
         delete gamepad_name;
         const TGamePadState* pState = gamepad_[nDevice - 1]->GetInitialState();
         assert(pState != 0);

         memcpy(&gamepad_state_[nDevice - 1], pState, sizeof(TGamePadState));
         CLogger::Get ()->Write("Keyboard", LogNotice, "Gamepad %u: %d Button(s) %d Hat(s)",
            nDevice, pState->nbuttons, pState->nhats);

         gamepad_[nDevice - 1]->RegisterRemovedHandler(KeyboardHardwareImplemetationPi::GamePadRemovedHandler, this);
         gamepad_[nDevice - 1]->RegisterStatusHandler(KeyboardHardwareImplemetationPi::GamePadStatusHandler);
         gamepad_active_[nDevice - 1] = LookForDevice(descriptor);

         CLogger::Get ()->Write("Keyboard", LogNotice, "Use your gamepad controls!");
      }

      // Keyboard
      if (keyboard_ == 0)
      {
         keyboard_ = (CUSBKeyboardDevice*)device_name_service_->GetDevice("ukbd1", FALSE);
         if (keyboard_ != 0)
         {
            keyboard_->RegisterRemovedHandler(KeyboardRemovedHandler);
            keyboard_->RegisterKeyStatusHandlerRaw(KeyStatusHandlerRaw);

            CLogger::Get ()->Write("Keyboard", LogNotice, "Just type something!");
         }
      }
   }
}


void KeyboardHardwareImplemetationPi::KeyStatusHandlerRaw(unsigned char ucModifiers, const unsigned char RawKeys[6])
{
   assert(keyboardPi_ != 0);

   CString Message;
   Message.Format("Key status (modifiers %02X)", (unsigned)ucModifiers);

   Lock();

   // Modifier
   if (old_modifier_ & shift_l_modifier_) keyboard_lines_[2] |= 0x20;
   if (old_modifier_ & shift_r_modifier_) keyboard_lines_[2] |= 0x20;
   if (old_modifier_ & ctrl_modifier_) keyboard_lines_[2] |= 0x80;
   if (old_modifier_ & copy_modifier_) keyboard_lines_[1] |= 0x02;

   // Unpress the previous keys
   for (unsigned i = 0; i < 6; i++)
   {
      //if (keyboardPi_->old_raw_keys_[i] != 0)
      {
         keyboardPi_->UnpressKey(keyboardPi_->old_raw_keys_[i]);

      }
   }

   // Press the new ones
   if (ucModifiers & shift_l_modifier_) keyboard_lines_[2] &= ~0x20;
   if (ucModifiers & shift_r_modifier_) keyboard_lines_[2] &= ~0x20;
   if (ucModifiers & ctrl_modifier_) keyboard_lines_[2] &= ~0x80;
   if (ucModifiers & copy_modifier_) keyboard_lines_[1] &= ~0x02;

   for (unsigned i = 0; i < 6; i++)
   {
      if (RawKeys[i] != 0)
      {

         //if (keyboardPi_->raw_to_cpc_map_[RawKeys[i]].bit != 0)
         {
            keyboardPi_->PressKey(RawKeys[i]);
         }

         CString KeyCode;
         KeyCode.Format(" %02X", (unsigned)RawKeys[i]);

         Message.Append(KeyCode);
      }
   }
   old_modifier_ = ucModifiers;
   memcpy(old_raw_keys_, RawKeys, sizeof(old_raw_keys_));
   Unlock();

   //CLogger::Get ()->Write ("Keyboard", LogNotice, Message);
}

void KeyboardHardwareImplemetationPi::KeyboardRemovedHandler(CDevice* pDevice, void* pContext)
{
   KeyboardHardwareImplemetationPi* pThis = (KeyboaKeyboardHardwareImplemetationPirdPi*)pContext;
   assert(pThis != 0);

   CLogger::Get()->Write("Keyboard", LogDebug, "Keyboard removed");

   keyboard_ = 0;
}


void KeyboardHardwareImplemetationPi::GamePadRemovedHandler(CDevice* pDevice, void* pContext)
{
   KeyboardHardwareImplemetationPi* pThis = (KeyboardHardwareImplemetationPi*)pContext;
   assert(pThis != 0);

   for (unsigned i = 0; i < MAX_GAMEPADS; i++)
   {
      if (pThis->gamepad_[i] == (CUSBGamePadDevice*)pDevice)
      {
         CLogger::Get()->Write("Keyboard", LogDebug, "Gamepad %u removed", i + 1);

         pThis->gamepad_[i] = 0;

         break;
      }
   }
}


void KeyboardHardwareImplemetationPi::GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState)
{
   assert(keyboardPi_ != 0);
   assert(pState != 0);

   memcpy(&keyboardPi_->gamepad_state_[nDeviceIndex], pState, sizeof * pState);
   // Set the new pushed buttons

   keyboardPi_->CheckActions(nDeviceIndex);
   if ((keyboardPi_->gamepad_active_[nDeviceIndex] != nullptr) && keyboardPi_->AddAction(&keyboardPi_->gamepad_active_[nDeviceIndex]->game_pad_button_select, nDeviceIndex))
   {
      keyboardPi_->select_ = true;
   }

   keyboardPi_->gamepad_state_buffered_[nDeviceIndex] = keyboardPi_->gamepad_state_[nDeviceIndex];
}


GamepadDef* KeyboardHardwareImplemetationPi::LookForDevice(const TUSBDeviceDescriptor* descriptor)
{
   GamepadDef* gamepad = nullptr;

   for (unsigned int index = 0; index < gamepad_list_.size(); index++)
   {
      if (gamepad_list_[index]->vid == descriptor->idVendor && gamepad_list_[index]->pid == descriptor->idProduct && gamepad_list_[index]->version == descriptor->bcdDevice)
      {
         CLogger::Get ()->Write("KeyboardPi", LogNotice, "Gamepad found in database !");
         return gamepad_list_[index];
      }
   }

   CLogger::Get ()->Write("KeyboardPi", LogNotice, "Unknown gamepad...");
   return gamepad;
}
