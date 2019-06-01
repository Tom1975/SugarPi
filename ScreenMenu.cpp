//
#include "ScreenMenu.h"


ScreenMenu::ScreenMenu(CLogger* logger) : 
   logger_(logger)
{
}

ScreenMenu::~ScreenMenu()
{
   
}

void ScreenMenu::Handle()
{
   logger_->Write("Menu", LogNotice, "Opening menu");
}
