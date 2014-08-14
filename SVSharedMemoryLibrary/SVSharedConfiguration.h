//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedConfiguration
//* .File Name       : $Workfile:   SVSharedConfiguration.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:40  $
//******************************************************************************
#pragma once

#include <string>
#include <fstream>

namespace SeidenaderVision
{
	class SVSharedConfiguration
	{
	public:
		static const std::string GetSharedDrive();
		static const std::string GetShareName();
		static const std::string GetLastInspectedName();
		static const std::string GetRejectsName();
		static const std::string GetPPQName();
		static const std::string GetPPQRejectsName();
		static const std::string GetMonitorListName();

		static const std::string GetSharedMemoryDirectoryName();
		static const std::string GetImageDirectoryName();
		static const std::string GetRejectImageDirectoryName();
		static void Log(const std::string & msg);

		static HRESULT SharedResourcesOk();
		static void EnsureShareDirectoryExists();

	private:
		SVSharedConfiguration();
		~SVSharedConfiguration();
		static inline const std::string fname() 
		{ 
			char buf[10] = {0};
			return std::string(GetSharedDrive() + "\\Temp\\") + _itoa(::GetCurrentProcessId(), buf, 16) + "log.txt";
		}
		static bool SharedDriveExists();
		static bool SharedDriveSizeOk();
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedConfiguration.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:07:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:39:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
