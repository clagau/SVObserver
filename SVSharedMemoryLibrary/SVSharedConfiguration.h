//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedConfiguration
//* .File Name       : $Workfile:   SVSharedConfiguration.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   20 Aug 2014 17:32:04  $
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
		static const std::string GetControlShareName();
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
		static bool SharedDriveExists();
		static bool ControlFileExits();

	private:
		SVSharedConfiguration();
		~SVSharedConfiguration();
		static inline const std::string LogFilename() 
		{ 
			char buf[40] = {0};
			return std::string(GetSharedDrive() + "\\Temp\\") + _itoa(::GetCurrentProcessId(), buf, 16) + "log.txt";
		}
		static bool SharedDriveSizeOk();
	};
}

