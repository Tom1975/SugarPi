#include "log.h"

Log::Log() :logger_(nullptr)
{

}
Log::~Log()
{

}
void Log::SetLogger(CLogger* logger)
{
   logger_ = logger;
}

void Log::WriteLog(const char* log)
{
   logger_->Write("LOG", LogNotice, log);
}
void Log::WriteLogByte(unsigned char number)
{
   logger_->Write("LOG", LogNotice, "%2.2X", number);
}
void Log::WriteLogShort(unsigned short number)
{
   logger_->Write("LOG", LogNotice, "%4.4X", number);
}
void Log::WriteLog(unsigned int number)
{
   logger_->Write("LOG", LogNotice, "%i", number);
}
void Log::EndOfLine()
{
   logger_->Write("LOG", LogNotice, "\n");
}

