#pragma once
#include <circle/logger.h>

class ScreenMenu 
{
public:
   ScreenMenu(CLogger* logger);
   virtual ~ScreenMenu();

   void Handle();

protected:
   CLogger* logger_;
};
