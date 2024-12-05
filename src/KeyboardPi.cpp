//
#include "KeyboardPi.h"

#include <memory.h>
#include <filesystem>

#ifdef __circle__
   #define KEYBOARD_SCANCODES_FILE "101_keyboard"
#elif  _WIN32
   #define KEYBOARD_SCANCODES_FILE "101_keyboard_win"
#elif __linux__ 
   #define KEYBOARD_SCANCODES_FILE "101_keyboard_linux"
#else
   #define KEYBOARD_SCANCODES_FILE "101_keyboard_linux"
   #pragma error "TODO : Generate a keyboard map for your OS !" 
#endif


#ifdef  __circle__
   #include <circle/spinlock.h>
   static CSpinLock   mutex_;
   static void Lock() { mutex_.Acquire(); }
   static void Unlock() { mutex_.Release(); }
#else
   #include <mutex>
   static std::mutex mutex_;
   static void Lock() { mutex_.lock(); }
   static void Unlock() { mutex_.unlock(); }
#endif

#define DEVICE_INDEX	1		// "upad1"


typedef char t_id[9];

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
   else if (strcmp(key, "dpright") == 0)
   {
      game_pad_button_right.AddHandler(CreateFunction(value));
      supported_controls_ |= GamePadButtonRight;
   }
   else if (strcmp(key, "dpup") == 0)
   {
      game_pad_button_up.AddHandler(CreateFunction(value));
      supported_controls_ |= GamePadButtonUp;
   }
   else if (strcmp(key, "start") == 0)
   {
      game_pad_button_start.AddHandler(CreateFunction(value));
      supported_controls_ |= GamePadButtonStart;
   }
   else if (strcmp(key, "righttrigger") == 0)
   {
      game_pad_button_start.AddHandler(CreateFunction(value));
      supported_controls_ |= GamePadButtonStart;
   }
   else if (strcmp(key, "back") == 0)
   {
      game_pad_button_select.AddHandler(CreateFunction(value));
      supported_controls_ |= GamePadButtonSelect;
   }
   if (strcmp(key, "lefttrigger") == 0)
   {
      game_pad_button_select.AddHandler(CreateFunction(value));
      supported_controls_ |= GamePadButtonSelect;
   }
   else if (strcmp(key, "leftx") == 0)
   {
      game_pad_button_left.AddHandler(CreateFunction(value, true));
      game_pad_button_right.AddHandler(CreateFunction(value, false));
      supported_controls_ |= GamePadButtonLeft;
      supported_controls_ |= GamePadButtonRight;
   }
   else if (strcmp(key, "lefty") == 0)
   {
      game_pad_button_down.AddHandler(CreateFunction(value, false));
      game_pad_button_up.AddHandler(CreateFunction(value, true));
      supported_controls_ |= GamePadButtonUp;
      supported_controls_ |= GamePadButtonDown;
   }
   return true;
}



unsigned int getFirstLine(const char* buffer, int size, std::string& out)
{
   if (size == 0)
   {
      return 0;
   }

   // looking for /n
   int offset = 0;
   while (buffer[offset] != 0x0A && buffer[offset] != 0x0D && offset < size)
   {
      offset++;
   }

   char* line = new char[offset + 1];
   memcpy(line, buffer, offset);
   line[offset] = '\0';
   out = std::string(line);
   delete[]line;
   return (offset == size) ? offset : offset + 1;
}

KeyboardPi::KeyboardPi(CLogger* logger) :
   logger_(logger),
   action_buttons_(0),
   select_(false)
{
   for (unsigned i = 0; i < MAX_GAMEPADS; i++)
   {
      gamepad_active_[i] = nullptr;
   }
   
   memset(&gamepad_state_buffered_, 0, sizeof(gamepad_state_buffered_));
   memset(&gamepad_state_, 0, sizeof(gamepad_state_));
}

KeyboardPi::~KeyboardPi()
{

}

void KeyboardPi::UnpressKey(unsigned int scancode)
{
    logger_->Write("KeyboardPi", LogNotice, "PressKey %X - line : %i, bit : %X", scancode, handler_.raw_to_cpc_map_[scancode & 0xFF].line_number, handler_.raw_to_cpc_map_[scancode & 0xFF].bit);
   if (handler_.raw_to_cpc_map_[scancode & 0xFF].bit != 0)
   {
      *handler_.raw_to_cpc_map_[scancode].line_index |= (handler_.raw_to_cpc_map_[scancode].bit);
   }
}

void KeyboardPi::PressKey(unsigned int scancode)
{
   logger_->Write("KeyboardPi", LogNotice, "PressKey %X - line : %i, bit : %X", scancode, handler_.raw_to_cpc_map_[scancode & 0xFF].line_number, handler_.raw_to_cpc_map_[scancode & 0xFF].bit);
   if (handler_.raw_to_cpc_map_[scancode & 0xFF].bit != 0)
   {
      
      *handler_.raw_to_cpc_map_[scancode & 0xFF].line_index &= ~(handler_.raw_to_cpc_map_[scancode & 0xFF].bit);
   }   
}

bool KeyboardPi::Initialize()
{
   // Load gamecontrollerdb.txt
   LoadGameControllerDB();

   return true;
}

void KeyboardPi::UpdatePlugnPlay()
{
}

unsigned char KeyboardPi::GetKeyboardMap(int index)
{
   unsigned char result = 0xFF;
   Lock();

   result = handler_.GetKeyboardMap(index);

   Unlock();
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
   Lock();
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_X, nDeviceIndex, true)?GamePadButtonX:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_A, nDeviceIndex, true)?GamePadButtonA:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_up, nDeviceIndex, true)?GamePadButtonUp:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_down, nDeviceIndex, true)?GamePadButtonDown:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_left, nDeviceIndex, true)?GamePadButtonLeft:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_right, nDeviceIndex, true)?GamePadButtonRight:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_start, nDeviceIndex, true)?GamePadButtonStart:0;
   action_buttons_ |= AddAction(&gamepad_active_[nDeviceIndex]->game_pad_button_select, nDeviceIndex, true)?GamePadButtonSelect:0;
   Unlock();
}

void KeyboardPi::Init(bool* register_replaced)
{
   std::filesystem::path exe_path("SD:");
   exe_path /= "Keyboards";
   exe_path /= KEYBOARD_SCANCODES_FILE;   
   handler_.InitKeyboard (exe_path.string().c_str());
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

bool KeyboardPi::IsButton(TGamePadButton button)
{
   if (action_buttons_ & (button))
   {
      Lock();
      action_buttons_ &= ~(button);
      Unlock();
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
      Lock();
      action_buttons_ &= ~(GamePadButtonA | GamePadButtonX);
      Unlock();
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


#define GAMECONTROLLERDB_FILE "SD:/Config/gamecontrollerdb.txt"

void KeyboardPi::LoadGameControllerDB()
{
   logger_->Write("KeyboardPi", LogNotice, "Loading game controller db...");

   // Open file
   FILE * f;
   f = fopen(GAMECONTROLLERDB_FILE, "r");
   //FRESULT Result = fopen(&File, GAMECONTROLLERDB_FILE, FA_READ | FA_OPEN_EXISTING);
   if (f == NULL)
   {
      CLogger::Get ()->Write("ConfigurationManager", LogNotice, "Cannot open %s file", GAMECONTROLLERDB_FILE);
      return;
   }

   // Load every known gamepad to internal structure
   fseek(f, 0, SEEK_END);
   unsigned int buffer_size_ = ftell(f);
   rewind(f);

   unsigned char* buff = new unsigned char[buffer_size_];
   unsigned nBytesRead;

   nBytesRead = fread(buff, 1, buffer_size_, f);
   if (buffer_size_ != nBytesRead)
   {
      // ERROR
      fclose(f);
      logger_->Write("KeyboardPi", LogNotice, "Error reading gamecontrollerdb  ");
      return;
   }

   // get next line
   gamepad_list_.clear();
   const char* ptr_buffer = (char*)buff;
   unsigned int offset = 0;
   unsigned int end_line;
   std::string s;
   while ((end_line = getFirstLine(&ptr_buffer[offset], nBytesRead, s)) > 0)
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


      GamepadDef * def = new GamepadDef(handler_.GetKeyboardState());

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
   fclose(f);

   logger_->Write("KeyboardPi", LogNotice, "Loading game controller db... Done !");

}
