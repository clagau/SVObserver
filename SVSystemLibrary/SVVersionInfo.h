//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVersionInfo
//* .File Name       : $Workfile:   SVVersionInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:04:00  $
//******************************************************************************
#pragma once
#include <string>
namespace SeidenaderVision
{
	class SVVersionInfo
	{
	public:
		static std::string GetVersion();
		static unsigned long GetLongVersion();
		static std::string GetTitleVersion();
		static std::string GetShortTitleVersion();

	private:
		SVVersionInfo();
		~SVVersionInfo();
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVVersionInfo.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:04:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2012 18:10:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added additional string representation of the version number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jul 2012 12:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new functionality to use the resource version information instead of data found in SVObserver.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/