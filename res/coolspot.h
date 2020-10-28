#pragma once

#include <circle/logger.h>

class CoolspotFont
{
public:
   CoolspotFont(CLogger* logger);
   virtual ~CoolspotFont();

   void CopyLetter(char c, int line, int* buffer);
   int GetLetterLength(char c);
   int GetLetterHeight(char c);

//protected:
   int char_position_[256];
   CLogger*    logger_;
};