//
#include "KeyboardPi.h"

#include <memory.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#include "CPCCore/CPCCoreEmu/simple_string.h"

#define DEVICE_INDEX	1		// "upad1"


class gamepad_def
{
   public:

   // Attributes
   unsigned int vid;
   unsigned int pid;
   unsigned int version;

   // Values for buttons
   
   // Helper 
   bool SetValue(const char* key, const char* value)
   {
      if ( strcmp(key, "") == 0) 
      {

      }
   }

};

typedef char t_id[9];

KeyboardPi* KeyboardPi::this_ptr_ = 0;


unsigned int getline ( const char* buffer, int size, std::string& out)
{
   if ( size == 0)
   {
      return 0;
   }
      
   // looking for /n
   int offset = 0;
   while (buffer[offset] != 0x0A && buffer[offset] != 0x0D && offset < size)
   {
      offset++;
   }

   char* line = new char[offset+1];
   memcpy ( line, buffer, offset);
   line[offset] = '\0';
   out = std::string(line);
   delete []line;
   return (offset == size)?offset:offset+1;
}

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
 #define GAMECONTROLLERDB_FILE "SD:/Config/gamecontrollerdb.txt"
void KeyboardPi::LoadGameControllerDB()
{
   // Open file
   FIL File;
   FRESULT Result = f_open(&File, GAMECONTROLLERDB_FILE, FA_READ | FA_OPEN_EXISTING);
   if (Result != FR_OK)
   {
      CLogger::Get ()->Write("ConfigurationManager", LogNotice, "Cannot open Config/gamecontrollerdb.txt file");
      return;
   }

   // Load every known gamepad to internal structure
FILINFO file_info;
   f_stat(GAMECONTROLLERDB_FILE, &file_info);
   unsigned char* buff = new unsigned char[file_info.fsize];
   unsigned nBytesRead;

   f_read(&File, buff, file_info.fsize, &nBytesRead);
   if (file_info.fsize != nBytesRead)
   {
      // ERROR
      f_close(&File);
      logger_->Write("KeyboardPi", LogNotice, "Error reading gamecontrollerdb  ");
      return;
   }

   // get next line
   const char* ptr_buffer = (char*)buff;
   unsigned int offset = 0;
   unsigned int end_line;
   std::string s;
   while ((end_line = getline(&ptr_buffer[offset], nBytesRead, s)) > 0)
   {
      gamepad_def def;
      // Do not use emty lines, and comment lines
      if (s.size() == 0 ||s[0] == '#')
      {
         continue;
      }

      // read : vid/pid
      t_id num_buffer[4];
      memset (num_buffer, 0, sizeof(num_buffer));
      bool error = false;
      for (int i = 0; i < 4 && !error; i++)
      {
         s.substr
         strncpy(num_buffer[i], s.substr(i*8, 8).c_str(), 9 );
         if (strlen(num_buffer[i]) != 8)
         {
            logger_->Write("KeyboardPi", LogNotice, "Extracting pid/vid informations error : Length incorrect");
            // Error, continue next line
            error = true;
         }
      }

      if (error) continue;
      char* ptr;
      def.vid = strtol(num_buffer[1], &ptr, 16);
      def.pid = strtol(num_buffer[2], &ptr, 16);
      def.version = strtol(num_buffer[3], &ptr, 16);
      // remove ids
      s = s.substr(33);
      // extract name (until next comma)
      std::string::size_type end_name = s.find (',');
      if (end_name == npos) continue;

      def.name = s.substr (0, end_name-1);
      s = substr (end_name+1);

      // extract buttons, axis, etc. Everything has the form : x:y,
      std::string::size_type end_str = s.find (',');
      while (end_str != npos)
      {
         std::string parameter = s.substr (0, end_str-1);
         std::string pos_middle = parameter.find (':');
         if ( pos_middle != npos)
         {
            std::string key = parameter.substr(0, pos_middle-1);
            std::string value = parameter.substr(pos_middle+1);

            // Affect to proper attribute.
            def.SetValue(key.c_str(), value.c_str());
         }

         s = substr (end_str+1);
         end_str = s.find (',');
      }

   }

   delete []buff;
   f_close(&File);

}