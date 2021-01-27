//
#include "KeyboardPi.h"

#include <memory.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#define DEVICE_INDEX	1		// "upad1"

//   SET_KEYBOARD(0x59, 1, 5);              // FN 1
//   SET_KEYBOARD(0x5A, 1, 6);              // FN 2
//   SET_KEYBOARD(0x14, 8, 5);              // A 


unsigned shift_l_modifier_ = 0x02;
unsigned shift_r_modifier_ = 0x20;
unsigned ctrl_modifier_ = 0x01;
unsigned copy_modifier_ = 0x04;

unsigned char default_raw_map[10][8] = 
{
   {0x52, 0x4F, 0x51, 0x61, 0x5E, 0x5B, 0x58, 0x63, },   // Cur_up Cur_right Cur_down F9 F6 F3 Enter F.
   {0x50, 0xE2, 0x5F, 0x60, 0x5D, 0x59, 0x5A, 0x62, },   // cur_left Copy f7 f8 f5 f1 f2 f0
   {0x4C, 0x30, 0x28, 0x32, 0x5C, 0xE5, 0x38, 0xE0, },   // Clr {[ Return }] F4 Shift `\ Ctrl
   {0x2E, 0x2D, 0x2F, 0x13, 0x34, 0x33, 0x2E, 0x37, },   // ^£ =- |@ P +; *: ?/ >,
   {0x27, 0x26, 0x12, 0x0C, 0x0F, 0x0E, 0x10, 0x36, },   // _0 )9 O I L K M <.
   {0x25, 0x24, 0x18, 0x1C, 0x0B, 0x0D, 0x11, 0x2C, },   // (8 '7 U Y H J N Space
   {0x23, 0x22, 0x15, 0x17, 0x0A, 0x09, 0x05, 0x19, },   // &,6,Joy1_Up %,5,Joy1_down, R,Joy1_Left T,Joy1_Right G,Joy1Fire2 F,Joy1Fire1 B V
   {0x21, 0x20, 0x08, 0x1A, 0x16, 0x07, 0x06, 0x1B, },   // $4 #3 E W S D C X
   {0x1E, 0x1F, 0x29, 0x14, 0x2B, 0x04, 0x39, 0x1D, },   // !1 "2 Esc Q Tab A CapsLock Z
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, }    // Joy0up Joy0down Joy0left Joy0right Joy0F1 Joy0F2 unused Del
};

GamepadActionHandler::GamepadActionHandler (unsigned char* line, unsigned int index, unsigned char* line2, unsigned int index2) : handler_(nullptr)
{
   line_[0] = line;
   line_[1] = line2;

   index_[0] = index;
   index_[1] = index2;
}

GamepadActionHandler::~GamepadActionHandler()
{
   // Delete handlers
   while (handler_ != nullptr)
   {
      Handler* next_handler = handler_->next_handler;
      delete handler_;
      handler_ =  next_handler;
   }
}

void GamepadActionHandler::AddHandler(IGamepadPressed* handler)
{
   Handler* next_handler = handler_;
   if ( handler_ == nullptr)
   {
      handler_ = new Handler;
      handler_->action_handler = handler;
      handler_->next_handler = nullptr;
   }
   else
   {
      while (next_handler->next_handler != nullptr)
      {
         next_handler = next_handler->next_handler;
      }
      next_handler->next_handler = new Handler;
      next_handler->next_handler->next_handler = nullptr;
      next_handler->next_handler->action_handler = handler;
   }
}

bool GamepadActionHandler::IsPressed(TGamePadState* state)
{
   Handler* current_handler = handler_;
   while ( current_handler != nullptr)
   {
      if (  current_handler->action_handler->IsPressed(state))
         return true;
      current_handler = current_handler->next_handler;
   }
   return false;
}

void GamepadActionHandler::UpdateMap(unsigned int nDeviceIndex, bool pressed)
{
   if (pressed)
   {
      *line_[nDeviceIndex] &= ~(1<<index_[nDeviceIndex]);
   }
   else
   {
      *line_[nDeviceIndex] |= (1<<index_[nDeviceIndex]);
   }
}

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
         return (state->hats[hat_index_] != 0xF) && (state->hats[hat_index_] & value_) == value_;
      }
   protected:
      unsigned int value_;
      unsigned int hat_index_;
};

class GamepadAxisPressed : public IGamepadPressed
{
   public:
      GamepadAxisPressed (unsigned int axis_index, bool axis_min) : axis_index_(axis_index), axis_min_(axis_min)
      {
      }

      virtual bool IsPressed(TGamePadState* state)
      {
         if ( axis_min_)
         {
            return state->axes[axis_index_].value == state->axes[axis_index_].minimum;
         }
         else
         {
            return state->axes[axis_index_].value == state->axes[axis_index_].maximum;
         }
         
      }
   protected:
      unsigned int axis_index_;
      bool axis_min_;
};

   //////////////////////////////////////   
   // Helper 
GamepadDef::GamepadDef(unsigned char* keymap) : supported_controls_(0), vid(0), pid(0), version(0),
game_pad_button_X(&keymap[9], 4, &keymap[9], 4),
game_pad_button_A(&keymap[9], 5, &keymap[9], 5),
game_pad_button_up(&keymap[9], 0, &keymap[9], 0),
game_pad_button_down(&keymap[9], 1, &keymap[9], 1),
game_pad_button_left(&keymap[9], 2, &keymap[9], 2),
game_pad_button_right(&keymap[9], 3, &keymap[9], 3),
game_pad_button_start(&keymap[5], 7, &keymap[5], 7),
game_pad_button_select(0, 0, 0, 0)
{

}   

IGamepadPressed* GamepadDef::CreateFunction(const char* value, bool min)
{
   if (strlen (value) < 2) return nullptr;

   switch (value[0])
   {
      case 'a':
      {
         unsigned int axis = atoi(&value [1]);
         return new GamepadAxisPressed(axis, min);
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

unsigned int GamepadDef::SetValue(const char* key, const char* value)
{
   if ( strcmp(key, "a") == 0) 
   {
      game_pad_button_A.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonA;
   }
   else if ( strcmp(key, "x") == 0) 
   {
      game_pad_button_X.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonA;
   }
   else if ( strcmp(key, "dpdown") == 0) 
   {
      game_pad_button_down.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonDown;
   }      
   else if ( strcmp(key, "dpleft") == 0) 
   {
      game_pad_button_left.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonLeft;
   }      
   else if ( strcmp(key, "dpright") == 0) 
   {
      game_pad_button_right.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonRight;
   }          
   else if ( strcmp(key, "dpup") == 0) 
   {
      game_pad_button_up.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonUp;
   }
   else if ( strcmp(key, "start") == 0) 
   {
      game_pad_button_start.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonStart;
   }      
   else if ( strcmp(key, "righttrigger") == 0) 
   {
      game_pad_button_start.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonStart;
   }      
   else if ( strcmp(key, "back") == 0) 
   {
      game_pad_button_select.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonSelect;
   }
   if ( strcmp(key, "lefttrigger") == 0) 
   {
      game_pad_button_select.AddHandler ( CreateFunction(value) );
      supported_controls_ |= GamePadButtonSelect;
   }
   else if ( strcmp(key, "leftx") == 0) 
   {
      game_pad_button_left.AddHandler ( CreateFunction(value, true) );
      game_pad_button_right.AddHandler ( CreateFunction(value, false) );
      supported_controls_ |= GamePadButtonLeft;
      supported_controls_ |= GamePadButtonRight;
   }
   else if ( strcmp(key, "lefty") == 0) 
   {
      game_pad_button_down.AddHandler ( CreateFunction(value, false) );
      game_pad_button_up.AddHandler ( CreateFunction(value, true) );
      supported_controls_ |= GamePadButtonUp;
      supported_controls_ |= GamePadButtonDown;
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
   memset ( keyboard_lines_, 0xff, sizeof (keyboard_lines_));

   InitKeyboard (default_raw_map);
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

#define SET_KEYBOARD(raw,line,b)\
   raw_to_cpc_map_[raw].line_index = &keyboard_lines_[line];\
   raw_to_cpc_map_[raw].bit = 1<<b;


 void KeyboardPi::InitKeyboard (unsigned char key_map[10][8])
{
   memset ( raw_to_cpc_map_, 0, sizeof raw_to_cpc_map_);
   memset ( old_raw_keys_, 0, sizeof old_raw_keys_);

   for (int line = 0; line < 10; line++)
   {
      for (int bit = 0; bit < 8; bit++)
      {
         unsigned char raw_key = key_map[line][bit];
         raw_to_cpc_map_[raw_key].line_index = &keyboard_lines_[line];
         raw_to_cpc_map_[raw_key].bit = 1<<bit;
      }
   }

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

   if (bUpdated)
   {
      // Gamepad
      for (unsigned nDevice = 1; (nDevice <= MAX_GAMEPADS); nDevice++)
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

         logger_->Write ("Keyboard", LogNotice, "Gamepad : %s - VID=%X; PID=%X; bcdDevice = %X", (const char*) (*gamepad_name), descriptor->idVendor, 
            descriptor->idProduct, descriptor->bcdDevice );
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

      // Keyboard
      if (keyboard_ == 0)
      {
         keyboard_ = (CUSBKeyboardDevice *) device_name_service_->GetDevice ("ukbd1", FALSE);
			if (keyboard_ != 0)
			{
				keyboard_->RegisterRemovedHandler (KeyboardRemovedHandler);
				keyboard_->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);

				logger_->Write ("Keyboard", LogNotice, "Just type something!");
			}
		}
   }
}

unsigned char KeyboardPi::GetKeyboardMap(int index)
{
   unsigned char result = 0xFF;
   mutex_.Acquire();

   result = keyboard_lines_[index];

   mutex_.Release();
   return result;
}

bool KeyboardPi::AddAction (GamepadActionHandler* action, unsigned nDeviceIndex, bool update_map)
{
   if ( action == nullptr ) return false;
   bool x = action->IsPressed(&gamepad_state_[nDeviceIndex]);

   if (update_map)
   {
      action->UpdateMap(nDeviceIndex, x);
   }

   bool buff_x = action->IsPressed(&gamepad_state_buffered_[nDeviceIndex]);
   return ( (buff_x & x)^x);
}

void KeyboardPi::CheckActions (unsigned nDeviceIndex)
{
   if ( gamepad_active_[nDeviceIndex] == nullptr) return;
   mutex_.Acquire();
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_X, nDeviceIndex, true)?GamePadButtonX:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_A, nDeviceIndex, true)?GamePadButtonA:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_up, nDeviceIndex, true)?GamePadButtonUp:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_down, nDeviceIndex, true)?GamePadButtonDown:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_left, nDeviceIndex, true)?GamePadButtonLeft:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_right, nDeviceIndex, true)?GamePadButtonRight:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_start, nDeviceIndex, true)?GamePadButtonStart:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_select, nDeviceIndex, true)?GamePadButtonSelect:0;
   mutex_.Release();
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
      mutex_.Acquire();
      action_buttons_ &= ~(GamePadButtonDown);
      mutex_.Release();
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
      mutex_.Acquire();
      action_buttons_ &= ~(GamePadButtonUp);
      mutex_.Release();
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
      mutex_.Acquire();
      action_buttons_ &= ~(GamePadButtonA | GamePadButtonX);
      mutex_.Release();
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

void KeyboardPi::KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6])
{
	assert (this_ptr_ != 0);

	CString Message;
	Message.Format ("Key status (modifiers %02X)", (unsigned) ucModifiers);

   this_ptr_->mutex_.Acquire();

   // Modifier
   if (this_ptr_->old_modifier_ & shift_l_modifier_) this_ptr_->keyboard_lines_[2] |= 0x20;
   if (this_ptr_->old_modifier_ & shift_r_modifier_) this_ptr_->keyboard_lines_[2] |= 0x20;
   if (this_ptr_->old_modifier_ & ctrl_modifier_) this_ptr_->keyboard_lines_[2] |= 0x80;
   if (this_ptr_->old_modifier_ & copy_modifier_) this_ptr_->keyboard_lines_[1] |= 0x02;

   // Unpress the previous keys
   for (unsigned i = 0; i < 6; i++)
   {
		if (this_ptr_->old_raw_keys_[i] != 0)
		{
         if (this_ptr_->raw_to_cpc_map_[this_ptr_->old_raw_keys_[i]].bit != 0)
         {
            *this_ptr_->raw_to_cpc_map_[this_ptr_->old_raw_keys_[i]].line_index |= (this_ptr_->raw_to_cpc_map_[this_ptr_->old_raw_keys_[i]].bit);
         }
      }
   }
   
   // Press the new ones
   if (ucModifiers & shift_l_modifier_) this_ptr_->keyboard_lines_[2] &= ~0x20;
   if (ucModifiers & shift_r_modifier_) this_ptr_->keyboard_lines_[2] &= ~0x20;
   if (ucModifiers & ctrl_modifier_) this_ptr_->keyboard_lines_[2] &= ~0x80;
   if (ucModifiers & copy_modifier_) this_ptr_->keyboard_lines_[1] &= ~0x02;

	for (unsigned i = 0; i < 6; i++)
	{
		if (RawKeys[i] != 0)
		{  
         if (this_ptr_->raw_to_cpc_map_[RawKeys[i]].bit != 0)
         {
            *this_ptr_->raw_to_cpc_map_[RawKeys[i]].line_index &= ~(this_ptr_->raw_to_cpc_map_[RawKeys[i]].bit);
         }

			CString KeyCode;
			KeyCode.Format (" %02X", (unsigned) RawKeys[i]);

			Message.Append (KeyCode);
		}
	}
   this_ptr_->old_modifier_ = ucModifiers;
   memcpy( this_ptr_->old_raw_keys_, RawKeys, sizeof (this_ptr_->old_raw_keys_));
   this_ptr_->mutex_.Release();

	CLogger::Get ()->Write ("Keyboard", LogNotice, Message);
}

void KeyboardPi::KeyboardRemovedHandler (CDevice *pDevice, void *pContext)
{
	KeyboardPi *pThis = (KeyboardPi *) pContext;
	assert (pThis != 0);

	CLogger::Get ()->Write ("Keyboard", LogDebug, "Keyboard removed");

	pThis->keyboard_ = 0;
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
   if (( this_ptr_->gamepad_active_[nDeviceIndex] != nullptr) && this_ptr_->AddAction(&this_ptr_->gamepad_active_[nDeviceIndex]->game_pad_button_select, nDeviceIndex))
   {
      this_ptr_->select_ = true;
   }

   this_ptr_->gamepad_state_buffered_[nDeviceIndex] = this_ptr_->gamepad_state_[nDeviceIndex];
}


 #define GAMECONTROLLERDB_FILE "SD:/Config/gamecontrollerdb.txt"
GamepadDef* KeyboardPi::LookForDevice (const TUSBDeviceDescriptor* descriptor)
{
   GamepadDef* gamepad = nullptr;

   for (unsigned int index = 0; index < gamepad_list_.size(); index++)
   {
      if ( gamepad_list_[index]->vid == descriptor->idVendor && gamepad_list_[index]->pid == descriptor->idProduct && gamepad_list_[index]->version == descriptor->bcdDevice)
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

      GamepadDef * def = new GamepadDef(keyboard_lines_);

      char* ptr;

      def->vid = strtoul(num_buffer[1], &ptr, 16);
      def->pid = strtoul(num_buffer[2], &ptr, 16);
      def->version = strtoul(num_buffer[3], &ptr, 16);

      // 

      // remove ids
      s = s.substr(33);
      // extract name (until next comma)
      std::string::size_type end_name = s.find (',');
      if (end_name == std::string::npos) continue;

      def->name = s.substr (0, end_name);
      s = s.substr (end_name+1);


      // Do not handle native circle++ handled device:
      if ( ( def->vid == 0x54C && def->pid == 0x268)
         ||( def->vid == 0x54C && def->pid == 0x9cc)
         ||( def->vid == 0x54C && def->pid == 0x5c4)
         ||( def->vid == 0x45e && def->pid == 0x28e)
         ||( def->vid == 0x45e && def->pid == 0x28f)
         ||( def->vid == 0x45e && def->pid == 0x2d1)
         ||( def->vid == 0x45e && def->pid == 0x2dd)
         ||( def->vid == 0x45e && def->pid == 0x2e3)
         ||( def->vid == 0x45e && def->pid == 0x2ea)
         ||( def->vid == 0x57e && def->pid == 0x2009)
         )
      {
         def->game_pad_button_X.AddHandler(new GamepadButtonPressed(10/*GamePadButtonX*/)); 
         def->game_pad_button_A.AddHandler(new GamepadButtonPressed(9/*GamePadButtonA*/)); 
         def->game_pad_button_up.AddHandler(new GamepadButtonPressed(15/*GamePadButtonUp*/)); 
         def->game_pad_button_down.AddHandler(new GamepadButtonPressed(17/*GamePadButtonDown*/)); 
         def->game_pad_button_left.AddHandler(new GamepadButtonPressed(18/*GamePadButtonLeft*/)); 
         def->game_pad_button_right.AddHandler(new GamepadButtonPressed(16/*GamePadButtonRight*/)); 
         def->game_pad_button_select.AddHandler(new GamepadButtonPressed(11/*GamePadButtonSelect*/)); 
         def->game_pad_button_start.AddHandler(new GamepadButtonPressed(14/*GamePadButtonStart*/)); 
      }
      else
      {
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
      }

      // Add to controller database
      gamepad_list_.push_back (def);
   }

   delete []buff;
   f_close(&File);

   logger_->Write("KeyboardPi", LogNotice, "Loading game controller db... Done !");
}

void KeyboardPi::LoadKeyboardLayout (const char* path)
{
   // Open file
   FIL File;
   FRESULT Result = f_open(&File, path, FA_READ | FA_OPEN_EXISTING);
   if (Result != FR_OK)
   {
      CLogger::Get ()->Write("ConfigurationManager", LogNotice, "Cannot open %s layout file", path);
      return;
   }

   // Load every known gamepad to internal structure
FILINFO file_info;
   f_stat(path, &file_info);
   unsigned char* buff = new unsigned char[file_info.fsize];
   unsigned nBytesRead;

   f_read(&File, buff, file_info.fsize, &nBytesRead);
   if (file_info.fsize != nBytesRead)
   {
      // ERROR
      f_close(&File);
      logger_->Write("KeyboardPi", LogNotice, "Error reading keyboard layout file %s ",path );
      return;
   }

   // get next line
   const char* ptr_buffer = (char*)buff;
   unsigned int offset = 0;
   unsigned int end_line;
   std::string s;
   int line_index = 0;
   while ((end_line = getline(&ptr_buffer[offset], nBytesRead, s)) > 0 && line_index < 8)
   {
      nBytesRead -= end_line;

      // Do not use emty lines, and comment lines
      if (s.size() == 0 ||s[0] == '#')
      {
         continue;
      }

      // Decode line to buffer
      for (int raw_key = 0; raw_key<8 && (2+raw_key * 3) < end_line; raw_key++)
      {
         char number [3];
         memcpy ( number, &ptr_buffer[offset+raw_key*3], 2);
         number[2] = '\0';
         unsigned char value = strtoul(number, NULL, 16);
         default_raw_map[line_index][raw_key] = value;
      }
      offset += end_line;
      line_index++;
   }
   delete []buff;
   f_close(&File);

   InitKeyboard (default_raw_map);
   
   logger_->Write("KeyboardPi", LogNotice, "Loading keyboard layout... Done !");   
}
