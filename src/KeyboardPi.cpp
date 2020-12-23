//
#include <memory.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#include "KeyboardPi.h"

#define DEVICE_INDEX	1		// "upad1"


KeyboardPi* KeyboardPi::this_ptr_ = 0;


KeyboardPi::KeyboardPi(CLogger* logger, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service) :
   logger_(logger),
   device_name_service_(device_name_service),
   dwhci_device_(dwhci_device),
   action_buttons_(0),
   select_(false)
{
   this_ptr_ = this;

	for (unsigned i = 0; i < MAX_GAMEPADS; i++)
	{
		gamepad_[i] = 0;
	}

   memset(&gamepad_state_buffered_, 0, sizeof(gamepad_state_buffered_));
   memset(&gamepad_state_, 0, sizeof(gamepad_state_));
}

KeyboardPi::~KeyboardPi()
{
   
}

bool KeyboardPi::Initialize()
{
   // Load gamecontrollerdb.txt
   LoadGameControllerDB();

   if (dwhci_device_->Initialize() == false)
   {
      logger_->Write("Keyboard", LogPanic, "Initialize failed !");
   }
   logger_->Write("Keyboard", LogNotice, "Initialize done.");

   UpdatePlugnPlay();

   return true;
}
void KeyboardPi::UpdatePlugnPlay()
{
   boolean bUpdated = dwhci_device_->UpdatePlugAndPlay ();

   for (unsigned nDevice = 1; bUpdated && (nDevice <= MAX_GAMEPADS); nDevice++)
		{
			if (gamepad_[nDevice-1] != 0)
			{
				continue;
			}

			gamepad_[nDevice-1] = (CUSBGamePadDevice *)device_name_service_->GetDevice ("upad", nDevice, FALSE);
			if (gamepad_[nDevice-1] == 0)
			{
				continue;
			}

         // Get gamepad names
         CString* gamepad_name = gamepad_[nDevice-1]->GetDevice()->GetNames();
         const TUSBDeviceDescriptor* descriptor = gamepad_[nDevice-1]->GetDevice()->GetDeviceDescriptor();

         logger_->Write ("Keyboard", LogNotice, "Gamepad : %s - VID=%X; PID=%X", (const char*) (*gamepad_name), descriptor->idVendor, 
            descriptor->idProduct );
         delete gamepad_name ;
			const TGamePadState *pState = gamepad_[nDevice-1]->GetInitialState ();
			assert (pState != 0);

         memcpy(&gamepad_state_[nDevice-1], pState, sizeof (TGamePadState));
			logger_->Write ("Keyboard", LogNotice, "Gamepad %u: %d Button(s) %d Hat(s)",
					nDevice, pState->nbuttons, pState->nhats);

			gamepad_[nDevice-1]->RegisterRemovedHandler (GamePadRemovedHandler, this);
			gamepad_[nDevice-1]->RegisterStatusHandler (GamePadStatusHandler);

			logger_->Write ("Keyboard", LogNotice, "Use your gamepad controls!");
		}
}

unsigned char KeyboardPi::GetKeyboardMap(int index)
{
   // Check : 
   if (index == 5)
   {
      unsigned char result = 0xFF;
      // button 1
      if (gamepad_state_[0].buttons& GamePadButtonStart)result &= ~0x80;
      return result;
   }
   if (index == 9 )
   {
      unsigned char result = 0xFF;
      // button 1
      if (gamepad_state_[0].buttons & GamePadButtonX) result &= ~0x10;
      // button 2
      if (gamepad_state_[0].buttons & GamePadButtonA) result &= ~0x20;
      // buttons up
      if (gamepad_state_[0].buttons & GamePadButtonUp) result &= ~0x1;
      // buttons down
      if (gamepad_state_[0].buttons & GamePadButtonDown) result &= ~0x2;
      // buttons left
      if (gamepad_state_[0].buttons & GamePadButtonLeft) result &= ~0x4;
      // buttons right
      if (gamepad_state_[0].buttons & GamePadButtonRight) result &= ~0x8;
      return result;
   }

   return 0xFF;
}

void KeyboardPi::Init(bool* register_replaced)
{

}

void KeyboardPi::ClearBuffer()
{
   action_buttons_ = 0;
   select_ = false;
}

bool KeyboardPi::IsSelect()
{
   return select_;
}

bool KeyboardPi::IsDown()
{
   if (action_buttons_ & (GamePadButtonDown))
   {
      action_buttons_ &= ~(GamePadButtonDown);
      return true;
   }
   else
   {
      return false;
   }

}

bool KeyboardPi::IsUp()
{
   if (action_buttons_ & (GamePadButtonUp))
   {
      action_buttons_ &= ~(GamePadButtonUp);
      return true;
   }
   else
   {
      return false;
   }
}

bool KeyboardPi::IsAction()
{
   if (action_buttons_ & (GamePadButtonA|GamePadButtonX))
   {
      action_buttons_ &= ~(GamePadButtonA | GamePadButtonX);
      return true;
   }
   else
   {
      return false;
   }
}


void KeyboardPi::ReinitSelect()
{
   select_ = false;
}

void KeyboardPi::GamePadRemovedHandler (CDevice *pDevice, void *pContext)
{
	KeyboardPi *pThis = (KeyboardPi *) pContext;
	assert (pThis != 0);

	for (unsigned i = 0; i < MAX_GAMEPADS; i++)
	{
		if (pThis->gamepad_[i] == (CUSBGamePadDevice *) pDevice)
		{
			CLogger::Get ()->Write ("Keyboard", LogDebug, "Gamepad %u removed", i+1);

			pThis->gamepad_[i] = 0;

			break;
		}
	}   
}

void KeyboardPi::GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState)
{
   assert(this_ptr_ != 0);
   assert(pState != 0);
   memcpy(&this_ptr_->gamepad_state_[nDeviceIndex], pState, sizeof * pState);

   // Compare with historic state
   // Set the new pushed buttons
   this_ptr_->action_buttons_ |= ((this_ptr_->gamepad_state_buffered_[nDeviceIndex].buttons & this_ptr_->gamepad_state_[nDeviceIndex].buttons) ^ this_ptr_->gamepad_state_[nDeviceIndex].buttons);
   this_ptr_->gamepad_state_buffered_[nDeviceIndex].buttons = this_ptr_->gamepad_state_[nDeviceIndex].buttons;

   // Select : Open menu
   if (pState->buttons & GamePadButtonSelect)
   {
      // Do something 
      this_ptr_->select_ = true;
   }
}

void KeyboardPi::LoadGameControllerDB()
{
   // Open file
   FIL File;
   FRESULT Result = f_open(&File, "SD:/Config/gamecontrollerdb.txt", FA_READ | FA_OPEN_EXISTING);
   if (Result != FR_OK)
   {
      CLogger::Get ()->Write("ConfigurationManager", LogNotice, "Cannot open Config/gamecontrollerdb.txt file");
      return;
   }
   
   // Load every known gamepad to internal structure



   f_close(&File);

}