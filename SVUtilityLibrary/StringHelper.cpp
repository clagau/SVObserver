//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file StringHelper.cpp
//*****************************************************************************
// Helper functions for std::string
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <locale>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <boost\algorithm\string\case_conv.hpp>
#include "StringHelper.h"
#include "LoadDll.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

constexpr unsigned long cBufferSize = 2000;

namespace SvUl
{
	std::string createStdString( const wchar_t* pString )
	{
		std::string retVal(_T(""));
		_bstr_t l_String( pString );

		if( l_String.length() )
		{
			retVal.assign( l_String );
		}

		return retVal;
	}

	std::string createStdString( const _bstr_t& rString )
	{
		std::string retVal(_T(""));

		if( rString.length() )
		{
			retVal.assign( rString );
		}

		return retVal;
	}

	std::string createStdString( const _variant_t& rVariant )
	{
		std::string Result(_T(""));
		_bstr_t Value( rVariant );

		if( 0 < Value.length() )
		{
			Result.assign( Value );
		}

		return Result;
	}

	int CompareNoCase(const std::string& rStringValue, const std::string & rStringValue2)
	{
		std::string i_left(rStringValue), i_right(rStringValue2);
		return MakeLower(i_left).compare(MakeLower(i_right));
	}

	std::string& MakeLower(std::string& rStringValue)
	{
		boost::to_lower(rStringValue);
		return rStringValue;
	}

	std::string& MakeUpper(std::string& rStringValue)
	{
		boost::to_upper(rStringValue);
		return rStringValue;
	}

	std::string& Trim(std::string& rStringValue, LPCTSTR pTrimChar)
	{
		return TrimLeft(TrimRight(rStringValue, pTrimChar), pTrimChar);
	}

	std::string& TrimLeft(std::string& rStringValue, LPCTSTR pTrimChar)
	{
		rStringValue.erase(0, rStringValue.find_first_not_of(pTrimChar));
		return rStringValue;
	}

	std::string& TrimRight(std::string& rStringValue, LPCTSTR pTrimChar)
	{
		rStringValue.erase(rStringValue.find_last_not_of(pTrimChar) + 1);
		return rStringValue;
	}

	std::string Left( const std::string& rStringValue, size_t count )
	{
		return rStringValue.substr(0, count);
	}

	std::string Mid( const std::string& rStringValue, size_t count ) 
	{
		std::string l_String;

		if( count < rStringValue.size() )
		{
			l_String = rStringValue.substr(count, rStringValue.size() - count);
		}

		return l_String;
	}

	std::string Right( const std::string& rStringValue, size_t count )
	{
		size_t size = rStringValue.size();
		if (size && size >= count)
		{
			return rStringValue.substr(size - count, count);
		}

		return std::string();
	}

	void Remove(std::string& rStringValue, std::string::traits_type::char_type ch)
	{
		rStringValue.erase(std::remove ( rStringValue.begin(), rStringValue.end(), ch ), rStringValue.end());
	}

	void RemoveCharacters(std::string& rStringValue, LPCTSTR ExcludeCharacters)
	{
		for(unsigned int i=0; i < strlen(ExcludeCharacters); i++)
		{
			Remove( rStringValue, ExcludeCharacters[i] );
		}
	}

	std::string ValidateString( const std::string& rStringValue, LPCTSTR ValidCharacters )
	{
		std::string Result( rStringValue );

		size_t Pos( Result.find_first_not_of( ValidCharacters ) );

		while( std::string::npos != Pos )
		{
			Result.erase(Pos, 1);
			Pos = Result.find_first_not_of( ValidCharacters );
		}

		return Result;
	}

	bool isSubmatch(const std::string& rStringValue, const std::string& rSearchString, size_t offsetSource, size_t offsetSearch )
	{
		bool retVal = false;
		size_t wildcardPos = rSearchString.find_first_of('*', offsetSearch);
		if (std::string::npos == wildcardPos)
		{
			retVal = (std::string::npos != rStringValue.find(rSearchString.c_str()+offsetSearch, offsetSource));
		}
		else
		{
			std::string firstSearchString = rSearchString.substr(offsetSearch, wildcardPos-offsetSearch);
			size_t firstPos = rStringValue.find(firstSearchString.c_str(), offsetSource);
			if (std::string::npos != firstPos)
			{
				retVal = isSubmatch(rStringValue, rSearchString, firstPos + firstSearchString.size(), wildcardPos+1);
			}
		}
		return retVal;
	}

	std::string& searchAndReplace( std::string& rStringValue, const std::string::traits_type::_Elem* pFromStr, const std::string::traits_type::_Elem* pToStr, size_t beginPos)
	{
		if (nullptr != pFromStr && nullptr != pToStr && 0 < strlen(pFromStr))
		{
			size_t pos = rStringValue.find(pFromStr, beginPos);

			while( pos != std::string::npos )
			{
				rStringValue.replace(pos, strlen(pFromStr), pToStr);

				pos = rStringValue.find( pFromStr, pos + _tcslen( pToStr ) );
			}
		}

		return rStringValue;
	}

	std::string Format( LPCSTR pFormat, ...)
	{
		std::string result;
		assert(pFormat);

		if (nullptr != pFormat)
		{
			va_list argList;
			va_start(argList, pFormat);

			char Text[cBufferSize];
			vsprintf_s(Text, cBufferSize, pFormat, argList);
			result = Text;
			va_end(argList);
		}
		return result;
	}

	// formatting (using wsprintf style formatting)
	std::string Format(LPCWSTR pFormat, ...)
	{
		std::string result;
		assert(pFormat);

		if (nullptr != pFormat)
		{
			va_list argList;
			va_start(argList, pFormat);

			wchar_t Text[cBufferSize];
			vswprintf_s(Text, cBufferSize, pFormat, argList);
			result = _bstr_t(Text);
			va_end(argList);
		}
		return result;
	}

	template<typename T>
	bool Convert2Number(const std::string& rStringValue, T& Value, bool failIfLeftoverChars)
	{
		std::string text = rStringValue;
		Trim(text);
		std::istringstream stream(text.c_str());
		char c;
		if (!(stream >> Value) || (failIfLeftoverChars && stream.get(c)))
		{
			return false;
		}
		return true;
	}
	
	std::string LoadStdString( UINT ResourceID )
	{
		std::string Result;

		HINSTANCE ResourceInstance( nullptr );
		LoadDll::Instance().getDll( SVOResourceDll, ResourceInstance );
		if( nullptr != ResourceInstance )
		{
			TCHAR ResourceString[cBufferSize];
			if( 0 < ::LoadString(ResourceInstance, ResourceID, ResourceString, cBufferSize ) )
			{
				Result = ResourceString;
			}
		}

		return Result;
	}

	std::string to_utf8(const _bstr_t& rUtf16)
	{
		if (rUtf16.length())
		{
			const wchar_t* const pUtf16 = static_cast<const wchar_t* const> (rUtf16);
			int size = ::WideCharToMultiByte(CP_UTF8, 0, pUtf16, -1, nullptr, 0, nullptr, nullptr) + 1;
			std::vector<char> buff;
			buff.resize(size, '\0');
			::WideCharToMultiByte(CP_UTF8, 0, pUtf16, -1, &buff.at(0), size, nullptr, nullptr);
			return std::string(&buff.at(0));
		}
		return std::string();
	}

	std::string to_utf8(const std::wstring& rUtf16)
	{
		int size = ::WideCharToMultiByte(CP_UTF8, 0, rUtf16.c_str(), -1, nullptr, 0, nullptr, nullptr) + 1;
		std::vector<char> buff;
		buff.resize(size, '\0');
		::WideCharToMultiByte(CP_UTF8, 0, rUtf16.c_str(), -1, &buff.at(0), size, nullptr, nullptr);
		return std::string(&buff.at(0));
	}

	std::string to_utf8(const std::string& rAnsi)
	{
		return SvUl::to_utf8(SvUl::to_utf16(rAnsi, CP_ACP));
	}

	std::wstring to_utf16(const std::string& rString, int cp)
	{
		int size = ::MultiByteToWideChar(cp, 0, rString.c_str(), -1, nullptr, 0) + 1;
		std::vector<wchar_t> buff;
		buff.resize(size, '\0');
		::MultiByteToWideChar(cp, 0, rString.c_str(), -1, &buff.at(0), size);
		return std::wstring(&buff.at(0));
	}

	std::string to_ansi(const std::string& rUtf8)
	{
		return createStdString(to_utf16(rUtf8).c_str());
	}

	typedef std::pair<std::string, std::string> StringPair;
	typedef std::map<int, StringPair > IntStringPairMap;

	bool AddEscapeSpecialCharacters(std::string& rString, bool bConvertCtrl)
	{
		bool bAdded = false;

		IntStringPairMap FoundStrings;

		//// Find Special Characters
		for (int index = 0; static_cast<int> (rString.size()) > index; index++)
		{
			// find the single character....
			TBYTE l_ch = rString[index];
			if (l_ch == _T('\\'))
			{
				FoundStrings[index] = StringPair(_T("\\\\"), _T("\\"));
				bAdded = true;
			}
			else if (l_ch == _T('"'))
			{
				FoundStrings[index] = StringPair(_T("\\\""), _T("\""));
				bAdded = true;
			}
			else if (bConvertCtrl && iscntrl(l_ch))
			{
				std::string first;
				std::string second;
				first = SvUl::Format(_T("\\%03o"), l_ch);
				second = SvUl::Format(_T("%c"), l_ch);
				FoundStrings[index] = StringPair(first, second);
				bAdded = true;
			}
		}

		// Process List
		if (bAdded)
		{
			std::string NewString;
			size_t start = 0;
			size_t count;

			for (IntStringPairMap::iterator it = FoundStrings.begin(); it != FoundStrings.end(); ++it)
			{
				count = it->first - start;
				if (count > 0)
				{
					NewString += rString.substr(start, count); // copy up to the found character.
				}
				// Expanded character added...
				NewString += it->second.first;

				start = it->first + it->second.second.size();
			}
			NewString += SvUl::Mid(rString, start);
			rString = NewString;
		}
		return bAdded;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Removes Escaped Double Quotes in Strings.
	/////////////////////////////////////////////////////////////////////////////
	bool RemoveEscapedSpecialCharacters(std::string& rString, bool bConvertCtrl)
	{
		bool bRemoved = false;

		IntStringPairMap FoundStrings;

		//// Find Special Characters
		size_t len = rString.size() - 1;
		size_t index = 0;

		while (std::string::npos != (index = rString.find(_T('\\'), index)))
		{
			if (rString[index + 1] == _T('\\'))
			{
				FoundStrings[static_cast<int> (index)] = StringPair(_T("\\\\"), _T("\\"));
				index++;
			}
			else if (rString[index + 1] == _T('"'))
			{
				FoundStrings[static_cast<int> (index)] = StringPair(_T("\\\""), _T("\""));
				index++;
			}
			else if (bConvertCtrl && index < len - 2 &&
				isdigit(rString[index + 1]) &&
				isdigit(rString[index + 2]) &&
				isdigit(rString[index + 3]))
			{
				std::string first;
				std::string second;
				int ctrlValue = (rString[index + 1] - _T('0')) * 64 + (rString[index + 2] - _T('0')) * 8 + (rString[index + 3] - _T('0'));
				first = SvUl::Format(_T("\\%03o"), ctrlValue);
				second = SvUl::Format(_T("%c"), ctrlValue);
				FoundStrings[static_cast<int> (index)] = StringPair(first, second);
				index += 3;
			}
			index++;
		}

		// Process Found list.
		if (FoundStrings.size() > 0)
		{
			std::string NewString;

			size_t start = 0;
			for (IntStringPairMap::iterator it = FoundStrings.begin(); it != FoundStrings.end(); ++it)
			{
				int count = it->first - static_cast<int> (start);
				NewString += rString.substr(start, count);
				// Insert "Single Character"
				NewString += it->second.second;
				start = it->first + it->second.first.size();
			}
			NewString += SvUl::Mid(rString, start);
			rString = NewString;
			bRemoved = true;
		}
		return bRemoved;
	}
} // namespace SvUl
