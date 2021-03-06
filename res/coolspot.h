#pragma once

#include <circle/logger.h>

class CoolspotFont
{
public:
   CoolspotFont(CLogger* logger);
   virtual ~CoolspotFont();

   void CopyLetter(unsigned char c, int line, int* buffer);
   int GetLetterLength(unsigned char c);
   int GetLetterHeight(unsigned char c);

//protected:
   int char_position_[256];
   CLogger*    logger_;
};