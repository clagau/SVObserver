//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVString
//* .File Name       : $Workfile:   SVString.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Dec 2014 18:20:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <locale>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <boost\algorithm\string\case_conv.hpp>
#include "SVString.h"
#pragma endregion Includes


namespace Seidenader { namespace SVUtilityLibrary { namespace StringFunctions
{
	SVString createSVString( const wchar_t* pString )
	{
		SVString retVal(_T(""));
		_bstr_t l_String( pString );

		if( l_String.length() )
		{
			retVal.assign( l_String );
		}

		return retVal;
	}

	SVString createSVString( const _bstr_t& rString )
	{
		SVString retVal(_T(""));

		if( rString.length() )
		{
			retVal.assign( rString );
		}

		return retVal;
	}

	SVString createSVString( const _variant_t& rVariant )
	{
		SVString Result(_T(""));
		_bstr_t Value( rVariant );

		if( 0 < Value.length() )
		{
			Result.assign( Value );
		}

		return Result;
	}

	int CompareNoCase(const SVString& rStringValue, const SVString & rStringValue2)
	{
		SVString i_left(rStringValue), i_right(rStringValue2);
		return MakeLower(i_left).compare(MakeLower(i_right));
	}

	SVString& MakeLower(SVString& rStringValue)
	{
		boost::to_lower(rStringValue);
		return rStringValue;
	}

	SVString& MakeUpper(SVString& rStringValue)
	{
		boost::to_upper(rStringValue);
		return rStringValue;
	}

	SVString& Trim(SVString& rStringValue, LPCTSTR pTrimChar)
	{
		return TrimLeft(TrimRight(rStringValue, pTrimChar), pTrimChar);
	}

	SVString& TrimLeft(SVString& rStringValue, LPCTSTR pTrimChar)
	{
		rStringValue.erase(0, rStringValue.find_first_not_of(pTrimChar));
		return rStringValue;
	}

	SVString& TrimRight(SVString& rStringValue, LPCTSTR pTrimChar)
	{
		rStringValue.erase(rStringValue.find_last_not_of(pTrimChar) + 1);
		return rStringValue;
	}

	SVString Mid( const SVString& rStringValue, size_t count ) 
	{
		SVString l_String;

		if( count < rStringValue.size() )
		{
			l_String = rStringValue.substr(count, rStringValue.size() - count);
		}

		return l_String;
	}

	SVString Right( const SVString& rStringValue, size_t count )
	{
		size_t size = rStringValue.size();
		if (size && size >= count)
		{
			return rStringValue.substr(size - count, count);
		}

		return SVString();
	}

	void Remove(SVString& rStringValue, SVString::traits_type::_Elem ch)
	{
		rStringValue.erase(std::remove ( rStringValue.begin(), rStringValue.end(), ch ), rStringValue.end());
	}

	void RemoveCharacters(SVString& rStringValue, LPCTSTR ExcludeCharacters)
	{
		for(unsigned int i=0; i < strlen(ExcludeCharacters); i++)
		{
			Remove( rStringValue, ExcludeCharacters[i] );
		}
	}

	bool isSubmatch(const SVString& rStringValue, const SVString& rSearchString, size_t offsetSource, size_t offsetSearch )
	{
		bool retVal = false;
		size_t wildcardPos = rSearchString.find_first_of('*', offsetSearch);
		if (SVString::npos == wildcardPos)
		{
			retVal = (SVString::npos != rStringValue.find(rSearchString.c_str()+offsetSearch, offsetSource));
		}
		else
		{
			SVString firstSearchString = rSearchString.substr(offsetSearch, wildcardPos-offsetSearch);
			size_t firstPos = rStringValue.find(firstSearchString.c_str(), offsetSource);
			if (SVString::npos != firstPos)
			{
				retVal = isSubmatch(rStringValue, rSearchString, firstPos + firstSearchString.size(), wildcardPos+1);
			}
		}
		return retVal;
	}

	SVString& searchAndReplace( SVString& rStringValue, const SVString::traits_type::_Elem* pFromStr, const SVString::traits_type::_Elem* pToStr )
	{
		if (nullptr != pFromStr && nullptr != pToStr)
		{
			size_t pos = rStringValue.find(pFromStr);

			while( pos != SVString::npos )
			{
				rStringValue.replace(pos, strlen(pFromStr), pToStr);

				pos = rStringValue.find( pFromStr, pos + _tcslen( pToStr ) );
			}
		}

		return rStringValue;
	}

	SVString Format( LPCTSTR pFormat, ...)
	{
		SVString stringValue(_T(""));
		assert(pFormat);

		if (nullptr != pFormat)
		{
			va_list argList;
			va_start(argList, pFormat);

			size_t nMaxLen = _vsctprintf( pFormat, argList ) + 1;

			TCHAR* pData = new TCHAR[nMaxLen];
			vsprintf(pData, pFormat, argList);
			stringValue = pData;
			delete [] pData;

			va_end(argList);
		}
		return stringValue;
	}

	// formatting (using wsprintf style formatting)
	SVString Format( const wchar_t* pFormat, ...)
	{
		SVString stringValue(_T(""));
		assert(pFormat);

		if (nullptr != pFormat)
		{
			va_list argList;
			va_start(argList, pFormat);

			size_t nMaxLen = _vscwprintf( pFormat, argList ) + 1;

			wchar_t* pData = new wchar_t[nMaxLen];
			vswprintf(pData, pFormat, argList);
			stringValue = _bstr_t( pData );
			delete [] pData;

			va_end(argList);
		}
		return stringValue;
	}

	template<typename T>
	bool Convert2Number(const SVString& rStringValue, T& Value, bool failIfLeftoverChars)
	{
		SVString text = rStringValue;
		Trim(text);
		std::istringstream stream(text.c_str());
		char c;
		if (!(stream >> Value) || (failIfLeftoverChars && stream.get(c)))
		{
			return false;
		}
		return true;
	}
}}}

