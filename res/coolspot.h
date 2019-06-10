#pragma once

#include <circle/logger.h>

class CoolspotFont
{
public:
   CoolspotFont();
   virtual ~CoolspotFont();

   void CopyLetter(char c, int line, int* buffer , CLogger* logger);
   int GetLetterLength(char c);
   int GetLetterHeight(char c);

//protected:
   int char_position_[256];

};