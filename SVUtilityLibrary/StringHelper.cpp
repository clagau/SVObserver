//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file StringHelper.cpp
//*****************************************************************************
// Helper functions for std::string
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
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
	std::string makeNameUniqueQuasiExplorerStyle(const std::string& rName, const std::vector<std::string>& rNamesToAvoid);
	std::string makeNameUniqueByNumbering(const std::string& rName, const std::vector<std::string>& rNamesToAvoid);

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

	std::string createStdString(const _bstr_t& rString)
	{
		std::string result;

		if(0 < rString.length())
		{
			result.assign(rString);
		}

		return result;
	}

	std::string createStdString(const _variant_t& rVariant)
	{
		std::string result;
		if (VT_BSTR == rVariant.vt && nullptr != rVariant.bstrVal)
		{
			_bstr_t value;
			//Only attach and detach BSTR for conversion
			value.Attach(rVariant.bstrVal);
			if (0 < value.length())
			{
				result.assign(value);
			}
			value.Detach();
		}
		return result;
	}

	int CompareNoCase(const std::string& rLhs, const std::string& rRhs)
	{
		return MakeLower(rLhs.c_str()).compare(MakeLower(rRhs.c_str()));
	}

	std::string MakeLower(LPCTSTR pString)
	{
		std::string result{pString};
		boost::to_lower(result);
		return result;
	}

	std::string MakeUpper(LPCTSTR pString)
	{
		std::string result {pString};
		boost::to_upper(result);
		return result;
	}

	std::string Trim(LPCTSTR pString, LPCTSTR pTrimChar)
	{
		return TrimLeft(TrimRight(pString, pTrimChar).c_str(), pTrimChar);
	}

	std::string TrimLeft(LPCTSTR pString, LPCTSTR pTrimChar)
	{
		std::string result(pString);
		result.erase(0, result.find_first_not_of(pTrimChar));
		return result;
	}

	std::string TrimRight(LPCTSTR pString, LPCTSTR pTrimChar)
	{
		std::string temp(pString);
		temp.erase(temp.find_last_not_of(pTrimChar) + 1);
		return temp;
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

	std::string& searchAndReplace( std::string& rStringValue, const std::string::traits_type::char_type* pFromStr, const std::string::traits_type::char_type* pToStr, size_t beginPos)
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
		std::string text = Trim(rStringValue.c_str());
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
		return to_utf8(to_utf16(rAnsi, CP_ACP));
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
				first = Format(_T("\\%03o"), l_ch);
				second = Format(_T("%c"), l_ch);
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
			NewString += Mid(rString, start);
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
				first = Format(_T("\\%03o"), ctrlValue);
				second = Format(_T("%c"), ctrlValue);
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
			NewString += Mid(rString, start);
			rString = NewString;
			bRemoved = true;
		}
		return bRemoved;
	}

	//provides "copied Name" strings similar to way the Windows File Explorer uses
	std::string copiedName(const std::string& rOriginalName, uint16_t copyIndex)
	{
		if (0 == copyIndex)
		{
			return rOriginalName;
		}

		if (1 == copyIndex)
		{
			return rOriginalName + _T("_Copy");
		}

		return rOriginalName + _T(Format("_Copy%u", copyIndex));
	}


	std::string makeNameUnique(const std::string& rOriginalName, const std::vector<std::string>& rNamesToAvoid, bool useExplorerStyle)
	{
		if (false == isStringInList(rOriginalName, rNamesToAvoid))
		{
			return rOriginalName;
		}
		if (useExplorerStyle)
		{
			// we want to find a unique name by getting a "copied name" similar to the style of Windows Explorer
			return makeNameUniqueQuasiExplorerStyle(rOriginalName, rNamesToAvoid);
		}

		// we want to find a unique name by adapting the last number in the name
		return makeNameUniqueByNumbering(rOriginalName, rNamesToAvoid);
	}


	std::string makeNameUniqueByNumbering(const std::string& rInitialName, const std::vector<std::string>& rNamesToAvoid)
	{
		//This strips any numbers at the end of the name
		auto pos = rInitialName.find_last_not_of(_T("0123456789"));

		pos += (pos == std::string::npos) ? 0 : 1;

		std::string ToolNameCore = rInitialName.substr(0, pos);

		int ToolIndex(0);

		for (const auto& rName : rNamesToAvoid)
		{
			if (std::string::npos != rName.find(ToolNameCore.c_str()))
			{
				// see if the name ends in a number
				int lastNum {0};
				bool digit {false};

				for (int i = static_cast<int> (rName.size()) - 1; i >= 0; i--)
				{
					if (isdigit(rName[i]))
					{
						digit = true;
					}
					else // found a non-digit - stop looking for a digit
					{
						// if any digits were found - convert to a number
						if (digit)
						{
							// convert to a number
							std::string numStr = Right(rName, (rName.size() - 1) - i);
							lastNum = atoi(numStr.c_str());
						}
						break;
					}
				}

				if (digit)
				{
					ToolIndex = std::max(ToolIndex, lastNum + 1);
				}
				else
				{
					ToolIndex = std::max(ToolIndex, 1);
				}
			}
		}

		if (0 != ToolIndex)
		{
			return Format(_T("%s%d"), ToolNameCore.c_str(), ToolIndex);
		}

		return ToolNameCore;
	}


	std::string makeNameUniqueQuasiExplorerStyle(const std::string& rName, const std::vector<std::string>& rNamesToAvoid)
	{
		constexpr uint16_t c_arbitraryMaxCopyIndex = 1000;

		uint16_t copyIndex = 0;

		do
		{
			auto newName = copiedName(rName, copyIndex);

			if (false == isStringInList(newName, rNamesToAvoid))
			{
				return newName;
			}
		} while (copyIndex++ < c_arbitraryMaxCopyIndex);

		return _T("<invalid copied name>");
	}


	bool isStringInList(const std::string& rToBeFound, const std::vector<std::string>& rStringsToBeSearched, const std::string& rExclude)
	{
		auto found = std::any_of(rStringsToBeSearched.begin(), rStringsToBeSearched.end(),
			[rToBeFound, rExclude](const std::string& rCurrentName)->bool
		{
			if (_T("") == rCurrentName)
			{
				return false;
			}
			if (_T("") != rExclude)
			{
				if (0 == _stricmp(rExclude.c_str(), rCurrentName.c_str()))
				{
					return  false;
				}
			}
			auto equal = (0 == _stricmp(rToBeFound.c_str(), rCurrentName.c_str()));
			return equal;
		});

		return found;
	}


	std::string getExtension(const std::string& rFilepath)
	{ 
		size_t pos = rFilepath.rfind(_T("."));

		if (std::string::npos == pos)
		{
			return "";
		}

		return rFilepath.substr(pos);
	}


} // namespace SvUl
