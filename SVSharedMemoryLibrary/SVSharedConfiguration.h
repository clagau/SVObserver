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

#pragma region Includes
//Moved to precompiled header: #include <fstream>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	class SVSharedConfiguration
	{
	public:
		static const double SharedDriveMinSize;// = 0.5;
		static const SVString GetSharedDrive(); //<return "V:";
		static const SVString GetShareName(); //<return "SVSharedMemory";
		static const SVString GetMonitorListName(); //< return "monitor_list";
		static const SVString GetSharedMemoryDirectoryName();
		static void Log(const SVString& msg);
		static HRESULT SharedResourcesOk();
		static void EnsureShareDirectoryExists();
		static bool SharedDriveExists();
	private:
		SVSharedConfiguration() =delete;
		static inline const SVString LogFilename() 
		{ 
			char buf[40] = {0};
			return SVString(GetSharedDrive() + "\\Temp\\") + _itoa(::GetCurrentProcessId(), buf, 16) + "log.txt";
		}
		static bool SharedDriveSizeOk();
	};
} //namespace SvSml
