//
#include "KeyboardPi.h"

#include <memory.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#define DEVICE_INDEX	1		// "upad1"


class GamepadButtonPressed : public IGamepadPressed
{
   public:
      GamepadButtonPressed (unsigned int bit_to_test) : bit_to_test_(1<<bit_to_test)
      {
      }

      virtual bool IsPressed(TGamePadState* state)
      {
         return state->buttons & bit_to_test_;
      }
   protected:
      unsigned int bit_to_test_;
};

class GamepadHatPressed : public IGamepadPressed
{
   public:
      GamepadHatPressed (unsigned int hat_index, unsigned int value) : value_(value), hat_index_(hat_index)
      {
      }

      virtual bool IsPressed(TGamePadState* state)
      {
         return (state->hats[hat_index_] & value_) == value_;
      }
   protected:
      unsigned int value_;
      unsigned int hat_index_;
};

class GamepadAxisPressed : public IGamepadPressed
{
   public:
      GamepadAxisPressed (unsigned int axis_index) : axis_index_(axis_index)
      {
      }

      virtual bool IsPressed(TGamePadState* state)
      {
         return state->axes[axis_index_].value > ((state->axes[axis_index_].maximum+state->axes[axis_index_].minimum)/2 );
      }
   protected:
      unsigned int axis_index_;
};

   //////////////////////////////////////   
   // Helper 
IGamepadPressed* GamepadDef::CreateFunction(const char* value)
{
   if (strlen (value) < 2) return nullptr;

   switch (value[0])
   {
      case 'a':
      {
         unsigned int axis = atoi(&value [1]);
         return new GamepadAxisPressed(axis);
         break;
      }
      case 'b':
      {
         unsigned int button = atoi(&value [1]);
         return new GamepadButtonPressed(button);
         break;
      }
      case 'h':
      {
         // hat, value
         std::string str = &value[1];
         size_t pos_middle = str.find ('.');
         if ( pos_middle != std::string::npos)
         {
            std::string str_hat = str.substr(0, pos_middle);
            std::string str_value = str.substr(pos_middle+1);
            unsigned int hat = atoi(str_hat.c_str());
            unsigned int value = atoi(str_value.c_str());
            return new GamepadHatPressed( hat, value );
         }
      }
      break;
   }
   return nullptr;
}

bool GamepadDef::SetValue(const char* key, const char* value)
{
   if ( strcmp(key, "a") == 0) 
   {
      delete game_pad_button_A;
      game_pad_button_A = CreateFunction(value);
   }
   else if ( strcmp(key, "x") == 0) 
   {
      delete game_pad_button_X;
      game_pad_button_X = CreateFunction(value);
   }
   else if ( strcmp(key, "dpdown") == 0) 
   {
      delete game_pad_button_down;
      game_pad_button_down = CreateFunction(value);
   }      
   else if ( strcmp(key, "dpleft") == 0) 
   {
      delete game_pad_button_left;
      game_pad_button_left = CreateFunction(value);
   }      
   else if ( strcmp(key, "dpright") == 0) 
   {
      delete game_pad_button_right;
      game_pad_button_right = CreateFunction(value);
   }          
   else if ( strcmp(key, "dpup") == 0) 
   {
      delete game_pad_button_up;
      game_pad_button_up = CreateFunction(value);
   }      
   else if ( strcmp(key, "start") == 0) 
   {
      delete game_pad_button_start;
      game_pad_button_start = CreateFunction(value);
   }      
   else if ( strcmp(key, "back") == 0) 
   {
      delete game_pad_button_select;
      game_pad_button_select = CreateFunction(value);
   }      
   return true;
}


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
      gamepad_active_ [i] = nullptr;
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
         gamepad_active_[nDevice-1] = LookForDevice (descriptor);

			logger_->Write ("Keyboard", LogNotice, "Use your gamepad controls!");
		}
}

unsigned char KeyboardPi::GetKeyboardMap(int index)
{
   // Check : 
   if ( gamepad_active_[0] != nullptr)
   {
      if (index == 5)
      {
         unsigned char result = 0xFF;
         // button 1
         //if (gamepad_state_[0].buttons& GamePadButtonStart)result &= ~0x80;
         if (gamepad_active_[0]->game_pad_button_start->IsPressed(&gamepad_state_[0]))result &= ~0x80;
         return result;
      }
      if (index == 9 )
      {
         unsigned char result = 0xFF;
         if (gamepad_active_[0]->game_pad_button_X->IsPressed(&gamepad_state_[0]))result &= ~0x10;
         if (gamepad_active_[0]->game_pad_button_A->IsPressed(&gamepad_state_[0]))result &= ~0x20;
         if (gamepad_active_[0]->game_pad_button_up->IsPressed(&gamepad_state_[0]))result &= ~0x1;
         if (gamepad_active_[0]->game_pad_button_down->IsPressed(&gamepad_state_[0]))result &= ~0x2;
         if (gamepad_active_[0]->game_pad_button_left->IsPressed(&gamepad_state_[0]))result &= ~0x4;
         if (gamepad_active_[0]->game_pad_button_right->IsPressed(&gamepad_state_[0]))result &= ~0x8;
            
            /*
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
            */
         return result;
      }
   }
   return 0xFF;
}

bool KeyboardPi::AddAction (IGamepadPressed* action, unsigned nDeviceIndex)
{
   if ( action == nullptr ) return false;
   bool x = action->IsPressed(&gamepad_state_[nDeviceIndex]);
   bool buff_x = action->IsPressed(&gamepad_state_buffered_[nDeviceIndex]);
   return ( (buff_x & x)^x);
}

void KeyboardPi::CheckActions (unsigned nDeviceIndex)
{
   if ( gamepad_active_[nDeviceIndex] == nullptr) return;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_X, nDeviceIndex)?GamePadButtonX:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_A, nDeviceIndex)?GamePadButtonA:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_up, nDeviceIndex)?GamePadButtonUp:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_down, nDeviceIndex)?GamePadButtonDown:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_left, nDeviceIndex)?GamePadButtonLeft:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_right, nDeviceIndex)?GamePadButtonRight:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_start, nDeviceIndex)?GamePadButtonStart:0;
   action_buttons_ |= AddAction(gamepad_active_[nDeviceIndex]->game_pad_button_select, nDeviceIndex)?GamePadButtonSelect:0;
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

   // Set the new pushed buttons
   this_ptr_->CheckActions (nDeviceIndex);
   this_ptr_->gamepad_state_buffered_[nDeviceIndex] = this_ptr_->gamepad_state_[nDeviceIndex];

   // Select : Open menu
   //if (pState->buttons & GamePadButtonSelect)
   if ( this_ptr_->gamepad_active_[nDeviceIndex] != nullptr)
   {
      if (this_ptr_->gamepad_active_[nDeviceIndex]->game_pad_button_select->IsPressed(&this_ptr_->gamepad_state_[nDeviceIndex])) 
      {
         // Do something 
         this_ptr_->select_ = true;
      }
   }
}


 #define GAMECONTROLLERDB_FILE "SD:/Config/gamecontrollerdb.txt"
 GamepadDef* KeyboardPi::LookForDevice (const TUSBDeviceDescriptor* descriptor)
{
   GamepadDef* gamepad = nullptr;

   for (unsigned int index = 0; index < gamepad_list_.size(); index++)
   {
      if ( gamepad_list_[index]->vid == descriptor->idVendor && gamepad_list_[index]->pid == descriptor->idProduct )
      {
         logger_->Write("KeyboardPi", LogNotice, "Gamepad found in database !");
         return gamepad_list_[index];
      }
   }

   logger_->Write("KeyboardPi", LogNotice, "Unknown gamepad...");
   return gamepad;
}

void KeyboardPi::LoadGameControllerDB()
{
   logger_->Write("KeyboardPi", LogNotice, "Loading game controller db...");

   // Open file
   FIL File;
   FRESULT Result = f_open(&File, GAMECONTROLLERDB_FILE, FA_READ | FA_OPEN_EXISTING);
   if (Result != FR_OK)
   {
      CLogger::Get ()->Write("ConfigurationManager", LogNotice, "Cannot open %s file", GAMECONTROLLERDB_FILE);
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
   gamepad_list_.clear();
   const char* ptr_buffer = (char*)buff;
   unsigned int offset = 0;
   unsigned int end_line;
   std::string s;
   while ((end_line = getline(&ptr_buffer[offset], nBytesRead, s)) > 0)
   {
      offset += end_line;
      nBytesRead -= end_line;
      
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
         std::string id = s.substr(i*8, 4);
         strcpy(num_buffer[i], id.substr(2, 2).c_str() );
         strcat(num_buffer[i], id.substr(0, 2).c_str() );
      }

      if (error) continue;

      GamepadDef * def = new GamepadDef;

      char* ptr;

      def->vid = strtoul(num_buffer[1], &ptr, 16);
      def->pid = strtoul(num_buffer[2], &ptr, 16);
      def->version = strtoul(num_buffer[3], &ptr, 16);
      // remove ids
      s = s.substr(33);
      // extract name (until next comma)
      std::string::size_type end_name = s.find (',');
      if (end_name == std::string::npos) continue;

      def->name = s.substr (0, end_name);
      s = s.substr (end_name+1);

      if ( def->vid != 0x79 || def->pid != 6)
         continue;

      // extract buttons, axis, etc. Everything has the form : x:y,
      std::string::size_type end_str = s.find (',');
      while (end_str != std::string::npos)
      {
         std::string parameter = s.substr (0, end_str);
         size_t pos_middle = parameter.find (':');
         if ( pos_middle != std::string::npos)
         {
            std::string key = parameter.substr(0, pos_middle);
            std::string value = parameter.substr(pos_middle+1);

            // Affect to proper attribute.
            def->SetValue(key.c_str(), value.c_str());
         }

         s = s.substr (end_str+1);
         end_str = s.find (',');
      }

      // Add to controller database
      gamepad_list_.push_back (def);
   }

   delete []buff;
   f_close(&File);

   logger_->Write("KeyboardPi", LogNotice, "Loading game controller db... Done !");
}