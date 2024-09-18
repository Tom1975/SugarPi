#pragma once

//
#include "KeyboardDefine.h"
#include <string>


class IGamepadPressed
{
public:
   virtual bool IsPressed(TGamePadState*) = 0;
};

class GamepadActionHandler
{
public:
   GamepadActionHandler(unsigned char* line, unsigned int index, unsigned char* line2, unsigned int index2);
   virtual ~GamepadActionHandler();

   void AddHandler(IGamepadPressed*);
   bool IsPressed(TGamePadState*);
   void UpdateMap(unsigned int nDeviceIndex, bool pressed);

protected:
   struct Handler
   {
      IGamepadPressed* action_handler;
      Handler* next_handler;
   };
   Handler* handler_;
   unsigned char* line_[2];
   unsigned int index_[2];
};

class GamepadDef
{
public:
   GamepadDef(unsigned char* keymap);
   virtual ~GamepadDef()
   {
   }

   unsigned int SetValue(const char* key, const char* value);
   IGamepadPressed* CreateFunction(const char* value, bool min = true);

   // Attributes
   std::string name;

   unsigned int supported_controls_;
   unsigned int vid;
   unsigned int pid;
   unsigned int version;

   // Values for buttons
   bool IsPressed(TGamePadState*);

   GamepadActionHandler game_pad_button_X;
   GamepadActionHandler game_pad_button_A;
   GamepadActionHandler game_pad_button_up;
   GamepadActionHandler game_pad_button_down;
   GamepadActionHandler game_pad_button_left;
   GamepadActionHandler game_pad_button_right;
   GamepadActionHandler game_pad_button_start;
   GamepadActionHandler game_pad_button_select;

};


class KeyboardHardwareImplemetation
{
   public:
      virtual void Initialize() = 0;
      virtual void UpdatePlugnPlay() = 0;
};
