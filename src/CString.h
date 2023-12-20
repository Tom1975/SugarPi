#pragma once

#include <string>

class CString
{
public:
	CString(void);
	CString(const char* pString);
	CString(const CString& rString);
	CString(CString&& rrString);
	virtual ~CString(void);

	operator const char* (void) const
	{
		return inner_string_.c_str();
	}

	const char* operator = (const char* pString)
	{
		inner_string_ = pString;
		return inner_string_.c_str();
	}

	CString& operator = (const CString& rString)
	{
		inner_string_ = rString.inner_string_;
		return *this;
	}

	CString& operator = (CString&& rrString)
	{
		inner_string_ = rrString.inner_string_;
		return *this;
	}

	size_t GetLength(void) const
	{
		return inner_string_.size();
	}

	void Append(const char* pString);
	int Compare(const char* pString) const;
	int Find(char chChar) const;			// returns index or -1 if not found

	int Replace(const char* pOld, const char* pNew); // returns number of occurrences

	void Format(const char* pFormat, ...);		// supports only a small subset of printf(3)
	void FormatV(const char* pFormat, va_list Args);

private:
	void PutChar(char chChar, size_t nCount = 1);
	void PutString(const char* pString);
	void ReserveSpace(size_t nSpace);


private:
	std::string inner_string_;
};