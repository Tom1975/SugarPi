#pragma once

//
#include "CPCCore/CPCCoreEmu/IKeyboard.h"


class KeyboardPi : public IKeyboardHandler
{
public:
   KeyboardPi();
   virtual ~KeyboardPi();

   virtual unsigned char GetKeyboardMap(int index);
   virtual void Init(bool* register_replaced);

protected:
};