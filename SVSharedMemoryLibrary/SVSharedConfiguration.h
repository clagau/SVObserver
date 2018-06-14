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
#pragma endregion Includes

namespace SvSml
{
	class SVSharedConfiguration
	{
	public:
		static const double SharedDriveMinSize;// = 0.5;
		static const std::string GetSharedDrive();
		static const std::string GetShareName(); //<return "SVSharedMemory";
		static const std::string GetMonitorListName(); //< return "monitor_list";
		static const std::string GetSharedMemoryDirectoryName();
		static void Log(const std::string& msg);
		static HRESULT SharedResourcesOk();
		static void EnsureShareDirectoryExists();
		static bool SharedDriveExists();
	private:
		SVSharedConfiguration() =delete;
		static inline const std::string LogFilename() 
		{ 
			const int bufsize{ 40 };
			char buf[bufsize] = {0};
			_itoa_s(::GetCurrentProcessId(), buf, bufsize, 16);
			std::string procId(buf);
			return std::string(GetSharedDrive() + "\\Temp\\") + procId + "log.txt";
		}
		static bool SharedDriveSizeOk();
	};
} //namespace SvSml
