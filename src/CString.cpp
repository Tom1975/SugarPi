#include <string>
#include <stdarg.h>
#include <stdio.h>

#include "CString.h"

CString::CString()
{

}

CString::CString(const char* pString)
{
	inner_string_ = pString;
}

CString::CString(const CString& rString)
{
	inner_string_ = rString.inner_string_;
}

CString::CString(CString&& rrString)
{
	inner_string_ = rrString.inner_string_;
}

CString::~CString(void)
{

}

void CString::Append(const char* pString)
{
	inner_string_.append( pString);
}

int CString::Compare(const char* pString) const
{
	return strcmp(inner_string_.c_str(), pString);
}

int CString::Find(char chChar) const
{
	return inner_string_.find(chChar);

}

// returns number of occurrences
int CString::Replace(const char* pOld, const char* pNew)
{
	// TODO
	return 0;
}

void CString::Format(const char* pFormat, ...)
{
	va_list var;
	va_start(var, pFormat);

	FormatV(pFormat, var);

	va_end(var);
}

void CString::FormatV(const char* pFormat, va_list Args)
{
	int n = vsnprintf(&inner_string_[0], inner_string_.size(), pFormat, Args);
	if (n > inner_string_.size())
	{
		inner_string_.resize(n + 1);
		vsnprintf(&inner_string_[0], inner_string_.size(), pFormat, Args);
	}
	// remove ending zero
	inner_string_.pop_back();
}

void CString::PutChar(char chChar, size_t nCount)
{
	inner_string_.append(nCount, chChar);
}

void CString::PutString(const char* pString)
{
	inner_string_.append(pString);
}

void CString::ReserveSpace(size_t nSpace)
{
	inner_string_.reserve(inner_string_.size() + nSpace);
}

