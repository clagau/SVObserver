//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#pragma endregion Includes

namespace Seidenader { namespace SVLibrary
{
	typedef std::pair<CString, CString> StringPair;
	typedef std::map<int, StringPair > IntStringPairMap;

	/////////////////////////////////////////////////////////////////////////////
	// Escapes Special Characters in Strings.
	/////////////////////////////////////////////////////////////////////////////
	bool AddEscapeSpecialCharacters( CString& rString, bool bConvertCtrl )
	{
		bool bAdded = false;

		IntStringPairMap FoundStrings;

		//// Find Special Characters
		int len = rString.GetLength();
		for( int index = 0; index < len; index++ )
		{
			// find the single character....
			TBYTE l_ch = rString[index]; 
			if( l_ch == _T('\\'))
			{
				FoundStrings[index] = StringPair(_T("\\\\"), _T("\\"));
				bAdded = true;
			}
			else if( l_ch == _T('"'))
			{
				FoundStrings[index] = StringPair(_T("\\\""), _T("\""));
				bAdded = true;
			}
			else if( bConvertCtrl && iscntrl(l_ch) )
			{
				CString first;
				CString second;
				first.Format( _T("\\%03o"), l_ch );
				second.Format( _T("%c"), l_ch );
				FoundStrings[index] = StringPair(first, second);
				bAdded = true;
			}
		}

		// Process List
		if( bAdded )
		{
			CString NewString;
			int start = 0;
			int count;

			for( IntStringPairMap::iterator it = FoundStrings.begin(); it != FoundStrings.end(); ++it)
			{
				count = it->first - start;
				if( count > 0 )
				{
					NewString += rString.Mid( start, count); // copy up to the found character.
				}
				// Expanded character added...
				NewString += it->second.first;

				start = it->first + it->second.second.GetLength();
			}
			NewString += rString.Mid( start );
			rString = NewString;
		}
		return bAdded;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Removes Escaped Double Quotes in Strings.
	/////////////////////////////////////////////////////////////////////////////
	bool RemoveEscapedSpecialCharacters( CString& rString, bool bConvertCtrl )
	{
		bool bRemoved = false;

		IntStringPairMap FoundStrings;

		//// Find Special Characters
		int len = rString.GetLength()-1;
		int index = 0;

		while( -1 != ( index = rString.Find(_T('\\'), index )) )
		{
			if( rString[index + 1] == _T('\\') )
			{
				FoundStrings[index] = StringPair(_T("\\\\"), _T("\\"));
				index++;
			}
			else if( rString[index + 1] == _T('"'))
			{
				FoundStrings[index] = StringPair(_T("\\\""), _T("\""));
				index++;
			}
			else if( bConvertCtrl && index < len - 2 && 
					isdigit(rString[index + 1]) &&
					isdigit(rString[index + 2]) &&
					isdigit(rString[index + 3]) )
			{
				CString first;
				CString second;
				int ctrlValue = (rString[index + 1] - _T('0')) * 64 + (rString[index + 2] - _T('0')) * 8 + (rString[index + 3] - _T('0'));
				first.Format( _T("\\%03o"), ctrlValue);
				second.Format(_T("%c"), ctrlValue);
				FoundStrings[index] = StringPair(first, second);
				index += 3;
			}
			index++;
		}

		// Process Found list.
		if( FoundStrings.size() > 0 )
		{
			CString NewString;
		
			int start = 0;
			for( IntStringPairMap::iterator it = FoundStrings.begin(); it != FoundStrings.end(); ++it )
			{
				int count = it->first - start;
				NewString += rString.Mid( start, count );
				// Insert "Single Character"
				NewString += it->second.second;	
				start = it->first + it->second.first.GetLength();
			}
			NewString += rString.Mid( start );
			rString = NewString;
			bRemoved = true;
		}
		return bRemoved;
	}
} /*SVLibrary*/ } /*Seidenader*/