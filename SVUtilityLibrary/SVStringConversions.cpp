//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:   SVStringConversions.cpp  $

#pragma region Includes
#include "stdafx.h"
#include "SVStringConversions.h"
#pragma endregion Includes

namespace Seidenader { namespace SVUtilityLibrary
{
	typedef std::map<int, SVStringPair > IntStringPairMap;

	bool AddEscapeSpecialCharacters( SVString& rString, bool bConvertCtrl )
	{
		bool bAdded = false;

		IntStringPairMap FoundStrings;

		//// Find Special Characters
		for( int index = 0; static_cast<int> (rString.size()) > index; index++ )
		{
			// find the single character....
			TBYTE l_ch = rString[index]; 
			if( l_ch == _T('\\'))
			{
				FoundStrings[index] = SVStringPair(_T("\\\\"), _T("\\"));
				bAdded = true;
			}
			else if( l_ch == _T('"'))
			{
				FoundStrings[index] = SVStringPair(_T("\\\""), _T("\""));
				bAdded = true;
			}
			else if( bConvertCtrl && iscntrl(l_ch) )
			{
				SVString first;
				SVString second;
				first = SvUl_SF::Format( _T("\\%03o"), l_ch );
				second = SvUl_SF::Format( _T("%c"), l_ch );
				FoundStrings[index] = SVStringPair(first, second);
				bAdded = true;
			}
		}

		// Process List
		if( bAdded )
		{
			SVString NewString;
			size_t start = 0;
			size_t count;

			for( IntStringPairMap::iterator it = FoundStrings.begin(); it != FoundStrings.end(); ++it)
			{
				count = it->first - start;
				if( count > 0 )
				{
					NewString += rString.substr( start, count); // copy up to the found character.
				}
				// Expanded character added...
				NewString += it->second.first;

				start = it->first + it->second.second.size();
			}
			NewString += SvUl_SF::Mid( rString, start );
			rString = NewString;
		}
		return bAdded;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Removes Escaped Double Quotes in Strings.
	/////////////////////////////////////////////////////////////////////////////
	bool RemoveEscapedSpecialCharacters( SVString& rString, bool bConvertCtrl )
	{
		bool bRemoved = false;

		IntStringPairMap FoundStrings;

		//// Find Special Characters
		size_t len = rString.size() - 1;
		size_t index = 0;

		while( SVString::npos != ( index = rString.find(_T('\\'), index )) )
		{
			if( rString[index + 1] == _T('\\') )
			{
				FoundStrings[static_cast<int> (index)] = SVStringPair(_T("\\\\"), _T("\\"));
				index++;
			}
			else if( rString[index + 1] == _T('"'))
			{
				FoundStrings[static_cast<int> (index)] = SVStringPair(_T("\\\""), _T("\""));
				index++;
			}
			else if( bConvertCtrl && index < len - 2 && 
					isdigit(rString[index + 1]) &&
					isdigit(rString[index + 2]) &&
					isdigit(rString[index + 3]) )
			{
				SVString first;
				SVString second;
				int ctrlValue = (rString[index + 1] - _T('0')) * 64 + (rString[index + 2] - _T('0')) * 8 + (rString[index + 3] - _T('0'));
				first = SvUl_SF::Format( _T("\\%03o"), ctrlValue);
				second = SvUl_SF::Format(_T("%c"), ctrlValue);
				FoundStrings[static_cast<int> (index)] = SVStringPair(first, second);
				index += 3;
			}
			index++;
		}

		// Process Found list.
		if( FoundStrings.size() > 0 )
		{
			SVString NewString;
		
			size_t start = 0;
			for( IntStringPairMap::iterator it = FoundStrings.begin(); it != FoundStrings.end(); ++it )
			{
				int count = it->first - static_cast<int> (start);
				NewString += rString.substr( start, count );
				// Insert "Single Character"
				NewString += it->second.second;	
				start = it->first + it->second.first.size();
			}
			NewString += SvUl_SF::Mid( rString, start );
			rString = NewString;
			bRemoved = true;
		}
		return bRemoved;
	}
} /* namespace SVUtilityLibrary */ } /* namespace Seidenader */
