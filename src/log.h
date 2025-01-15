#pragma once

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif



#include "CPCCore/CPCCoreEmu/ILog.h"

class Log : public ILog
{
public:
   Log();
   virtual ~Log();
   void SetLogger(CLogger* logger);
   virtual void WriteLog(const char* log);
   virtual void WriteLogByte(unsigned char number);
   virtual void WriteLogShort(unsigned short number);
   virtual void WriteLog(unsigned int number);
   virtual void EndOfLine();

protected:
   CLogger* logger_;
};
