//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:   SVStringConversions.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 13:27:10  $
//******************************************************************************

#ifndef SVSTRINGCONVERSIONS_H
#define SVSTRINGCONVERSIONS_H

#include <string>
#include <boost/static_assert.hpp>

namespace SVStringConversions
{
	template<typename T>
	inline std::string to_utf8(const T& utf16)
	{
		// do static assert
		// Only allowed types are _bstr_t and std::wstring
		//BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Use of non allowed type, allowed types are _bstr_t and std::wstring");
		BOOST_STATIC_ASSERT(sizeof(T) == 0);
	}

	std::wstring to_utf16(const std::string & utf8, int cp = CP_UTF8);
}

#include "SVStringConversions.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVStringConversions.h_v  $
 * 
 *    Rev 1.1   30 Jul 2013 13:27:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added code page flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:25:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 13:45:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  787
 * SCR Title:  Fix problem when Discrete I/O does not change when forced attribute is set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in for new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
