//
#include "KeyboardPi.h"

#include <memory.h>


#ifdef  __circle__
#include <circle/spinlock.h>
static CSpinLock   mutex_;
void Lock() { Lock(); }
void Unlock() { Unlock(); }
#else
#include <mutex>
static std::mutex mutex_;
static void Lock() { mutex_.lock(); }
static void Unlock() { mutex_.unlock(); }
#endif

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
   {0x1E, 0x1F, 0x29, 0x14, 0x2B, 0x10, 0x39, 0x1D, },   // !1 "2 Esc Q Tab A CapsLock Z
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


typedef char t_id[9];

KeyboardPi* KeyboardPi::this_ptr_ = 0;


unsigned int getline(const char* buffer, int size, std::string& out)
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

//KeyboardPi::KeyboardPi(CLogger* logger, CUSBHCIDevice* dwhci_device, CDeviceNameService* device_name_service) :
KeyboardPi::KeyboardPi(CLogger* logger) :
   logger_(logger),
   action_buttons_(0),
   select_(false)
{
   memset(keyboard_lines_, 0xff, sizeof(keyboard_lines_));

   for (unsigned i = 0; i < MAX_GAMEPADS; i++)
   {
      gamepad_active_[i] = nullptr;
   }

   InitKeyboard(default_raw_map);
   this_ptr_ = this;

   memset(&gamepad_state_buffered_, 0, sizeof(gamepad_state_buffered_));
   memset(&gamepad_state_, 0, sizeof(gamepad_state_));
}

KeyboardPi::~KeyboardPi()
{

}

#define SET_KEYBOARD(raw,line,b)\
   raw_to_cpc_map_[raw].line_index = &keyboard_lines_[line];\
   raw_to_cpc_map_[raw].bit = 1<<b;

void KeyboardPi::SetHard(KeyboardHardwareImplemetation* hard_imp)
{
   hard_imlementation_ = hard_imp;
}

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
         raw_to_cpc_map_[raw_key].line_number = line;
         raw_to_cpc_map_[raw_key].bit = 1<<bit;
      }
   }

}

void KeyboardPi::UnpressKey(unsigned int scancode)

{
   if (raw_to_cpc_map_[scancode & 0xFF].bit != 0)
   {
      logger_->Write("KeyboardPi", LogNotice, "PressKey %X - line : %i, bit : %X", scancode, raw_to_cpc_map_[scancode & 0xFF].line_number, raw_to_cpc_map_[scancode & 0xFF].bit);
      *raw_to_cpc_map_[scancode].line_index |= (raw_to_cpc_map_[scancode].bit);
   }
}

void KeyboardPi::PressKey(unsigned int scancode)
{
   if (raw_to_cpc_map_[scancode & 0xFF].bit != 0)
   {
      logger_->Write("KeyboardPi", LogNotice, "UnpressKey %X - line : %i, bit : %X", scancode, raw_to_cpc_map_[scancode & 0xFF].line_number, raw_to_cpc_map_[scancode & 0xFF].bit);
      *raw_to_cpc_map_[scancode & 0xFF].line_index &= ~(raw_to_cpc_map_[scancode & 0xFF].bit);
   }
   
}


bool KeyboardPi::Initialize()
{
   // Load gamecontrollerdb.txt
   //LoadGameControllerDB();
   hard_imlementation_->Initialize();
   //UpdatePlugnPlay();

   return true;
}
void KeyboardPi::UpdatePlugnPlay()
{
   hard_imlementation_->UpdatePlugnPlay();
}

unsigned char KeyboardPi::GetKeyboardMap(int index)
{
   unsigned char result = 0xFF;
   Lock();

   result = keyboard_lines_[index];

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
/*
bool KeyboardPi::IsDown()
{
   if (action_buttons_ & (GamePadButtonDown))
   {
      Lock();
      action_buttons_ &= ~(GamePadButtonDown);
      Unlock();
      return true;
   }
   else
   {
      return false;
   }
}*/

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
   fclose(f);

   logger_->Write("KeyboardPi", LogNotice, "Loading game controller db... Done !");

}

void KeyboardPi::LoadKeyboard(const char* path)
{
   // Open file
   FILE *f;
   f = fopen(path, "r");
   if (f == NULL)
   {
      CLogger::Get ()->Write("ConfigurationManager", LogNotice, "Cannot open %s layout file", path);
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
         offset += end_line;
         continue;
      }

      // Decode line to buffer
      // Remove spaces
      while (ptr_buffer[offset] == ' ')
      {
         offset++;
      }
      for (unsigned int raw_key = 0; raw_key<8 && (2+raw_key * 3) < end_line; raw_key++)
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
   fclose(f);

   InitKeyboard (default_raw_map);
   
   logger_->Write("KeyboardPi", LogNotice, "Loading keyboard layout... Done !");   
}
