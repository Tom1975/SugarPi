//
#include <windows.h>

#include "CLogger.h"
#include "CString.h"


CLogger* CLogger::pThis = nullptr;

CLogger::CLogger(unsigned nLogLevel)
{
   if (pThis == nullptr)
      pThis = this;
}

CLogger::~CLogger()
{
}

void CLogger::Write(const char* pSource, TLogSeverity Severity, const char* pMessage, ...)
{
   CString str;

   va_list var;
   va_start(var, pMessage);
   str.FormatV(pMessage, var);
   va_end(var);

   str.Append("\r\n");
   CString str2 = "[";
   str2.Append(pSource);
   str2.Append("] ");
   str2.Append(str);

   ::OutputDebugString(str2);
}

void CLogger::WriteV(const char* pSource, TLogSeverity Severity, const char* pMessage, va_list Args)
{
   
}

CLogger* CLogger::Get(void)
{
   return pThis;
}
