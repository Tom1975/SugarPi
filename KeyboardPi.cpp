//
#include <memory.h>

#include "KeyboardPi.h"

#define DEVICE_INDEX	1		// "upad1"


KeyboardPi* KeyboardPi::this_ptr_ = 0;


KeyboardPi::KeyboardPi(CLogger* logger, CDWHCIDevice* dwhci_device, CDeviceNameService* device_name_service) :
   logger_(logger),
   dwhci_device_(dwhci_device),
   device_name_service_(device_name_service),
   select_(false)
{
   this_ptr_ = this;
}

KeyboardPi::~KeyboardPi()
{
   
}

bool KeyboardPi::Initialize()
{
   if (dwhci_device_->Initialize() == false)
   {
      logger_->Write("Keyboard", LogPanic, "Initialize failed !");
   }
   logger_->Write("Keyboard", LogNotice, "Initialize done.");

   // Search for plugged USB
   gamepad_ = (CUSBGamePadDevice*)device_name_service_->GetDevice("upad", DEVICE_INDEX, FALSE);
   if (gamepad_ == 0)
   {
      logger_->Write("Keyboard", LogPanic, "Gamepad not found");
   }
   else
   {
      logger_->Write("Keyboard", LogNotice, "Gamepad found");
   }

   if (!(gamepad_->GetProperties() & GamePadPropertyIsKnown))
   {
      logger_->Write("Keyboard", LogPanic, "Gamepad mapping is not known");
   }

   // get initial state from gamepad and register status handler
   const TGamePadState* pState = gamepad_->GetInitialState();
   assert(pState != 0);

   memcpy(&gamepad_state_, pState, sizeof (TGamePadState));

   gamepad_->RegisterStatusHandler(GamePadStatusHandler);
   logger_->Write("Keyboard", LogNotice, "Initialize done !");
   return true;
}

unsigned char KeyboardPi::GetKeyboardMap(int index)
{
   // Check : 
   if (index == 5)
   {
      unsigned char result = 0xFF;
      // button 1
      if (gamepad_state_.buttons& GamePadButtonStart)result &= ~0x80;
      return result;
   }
   if (index == 9 )
   {
      unsigned char result = 0xFF;
      // button 1
      if (gamepad_state_.buttons & GamePadButtonX) result &= ~0x10;
      // button 2
      if (gamepad_state_.buttons & GamePadButtonA) result &= ~0x20;
      // buttons up
      if (gamepad_state_.buttons & GamePadButtonUp) result &= ~0x1;
      // buttons down
      if (gamepad_state_.buttons & GamePadButtonDown) result &= ~0x2;
      // buttons left
      if (gamepad_state_.buttons & GamePadButtonLeft) result &= ~0x4;
      // buttons right
      if (gamepad_state_.buttons & GamePadButtonRight) result &= ~0x8;
      return result;
   }

   return 0xFF;
}

void KeyboardPi::Init(bool* register_replaced)
{

}

bool KeyboardPi::IsSelect()
{
   return select_;
}
void KeyboardPi::ReinitSelect()
{
   select_ = false;
}

void KeyboardPi::GamePadStatusHandler(unsigned nDeviceIndex, const TGamePadState* pState)
{
   //this_ptr_->logger_->Write("Keyboard", LogNotice, "GamePadStatus handler : %i; button state : %i", nDeviceIndex, pState->buttons);

   if (nDeviceIndex != DEVICE_INDEX - 1)
   {
      return;
   }

   assert(this_ptr_ != 0);
   assert(pState != 0);
   memcpy(&this_ptr_->gamepad_state_, pState, sizeof * pState);

   // Select : Open menu
   if (pState->buttons & GamePadButtonSelect)
   {
      // Do something 
      this_ptr_->select_ = true;
      this_ptr_->logger_->Write("Keyboard", LogNotice, "SELECT : Open menu");
   }
}