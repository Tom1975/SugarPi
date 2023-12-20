#pragma once
#include <cstdarg>

//
enum TLogSeverity
{
   LogPanic,	// Halt the system after processing this message
   LogError,	// Severe error in this component, system may continue to work
   LogWarning,	// Non-severe problem, component continues to work
   LogNotice,	// Informative message, which is interesting for the system user
   LogDebug	// Message, which is only interesting for debugging this component
};


class CLogger
{
public:
   CLogger(unsigned nLogLevel);
   virtual ~CLogger();

   void Write(const char* pSource, TLogSeverity Severity, const char* pMessage, ...);
   void WriteV(const char* pSource, TLogSeverity Severity, const char* pMessage, va_list Args);

   static CLogger* Get(void);

private:
   static CLogger *pThis ;

};