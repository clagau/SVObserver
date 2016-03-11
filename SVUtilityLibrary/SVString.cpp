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
#include "SVString.h"
#include <algorithm>
#include <locale>
#include <sstream>
#include <boost\algorithm\string\case_conv.hpp>
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
		//_bstr_t l_String( p_rString );

		if( rString.length() )
		{
			retVal.assign( rString );
		}

		return retVal;
	}

	SVString createSVString( const VARIANT& rVariant )
	{
		SVString retVal(_T(""));
		_bstr_t l_String( rVariant );

		if (l_String.length())
		{
			retVal.assign( l_String );
		}

		return retVal;
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

		if (pFormat != NULL)
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

		if (pFormat != NULL)
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVString.cpp_v  $
 * 
 *    Rev 1.1   16 Dec 2014 18:20:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method isSubmatch to support the Object Selector's Filter Page search.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:24:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Feb 2013 12:43:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVString with a nw method to get a const reference to the internal data type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Sep 2012 13:49:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  787
 * SCR Title:  Fix problem when Discrete I/O does not change when forced attribute is set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Aug 2012 17:26:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update comment on method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Aug 2012 16:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated string class to fix find methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Jun 2012 17:42:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Feb 2011 09:52:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2010 08:22:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2009 09:40:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jun 2009 07:46:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ToString method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
