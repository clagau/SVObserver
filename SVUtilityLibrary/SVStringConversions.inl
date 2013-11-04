//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:   SVStringConversions.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 Jul 2013 13:27:12  $
//******************************************************************************

#include "stdafx.h"

#include <comdef.h>
#include <boost/config.hpp>
#include <boost/scoped_array.hpp>
#include "SVStringConversions.h"

namespace SVStringConversions
{
	template<>
	inline std::string to_utf8(const _bstr_t & utf16)
	{
		if (utf16.length())
		{
			std::wstring wstr = static_cast<wchar_t*>(utf16);
			int sz = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL)+1;
			boost::scoped_array<char> buff(new char[sz]);
			::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buff.get(), sz, NULL, NULL);
			return std::string(buff.get());
		}
		return std::string();
	}

	template<>
	inline std::string to_utf8(const std::wstring & utf16)
	{
		int sz = ::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL)+1;
		boost::scoped_array<char> buff(new char[sz]);
		::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, buff.get(), sz, NULL, NULL);
		return std::string(buff.get());
	}

	inline std::string to_utf8(const wchar_t * str)
	{
		//::OutputDebugStringW(str);
		int sz = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL)+1;
		boost::scoped_array<char> buff(new char[sz]);
		::WideCharToMultiByte(CP_UTF8, 0, str, -1, buff.get(), sz, NULL, NULL);
		return std::string(buff.get());
	}
	
	inline std::wstring to_utf16(const std::string & utf8, int cp)
	{
		int sz = ::MultiByteToWideChar(cp, 0, utf8.c_str(), -1, NULL, 0)+1;
		boost::scoped_array<wchar_t> buff(new wchar_t[sz]);
		::MultiByteToWideChar(cp, 0, utf8.c_str(), -1, buff.get(), sz);
		return std::wstring(buff.get());
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVStringConversions.inl_v  $
 * 
 *    Rev 1.2   30 Jul 2013 13:27:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added code page flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jun 2013 18:35:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jun 2013 03:41:44   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added overload for wchar_t *
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:25:44   bWalter
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
