//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedConfiguration
//* .File Name       : $Workfile:   SVSharedConfiguration.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 Oct 2014 19:02:52  $
//******************************************************************************

#include "stdafx.h"
#include <shlwapi.h>
#include <winioctl.h>
#include "SVSharedConfiguration.h"
#include "SVShared.h"

#define ENABLE_LOG

using namespace SeidenaderVision;

const std::string SVSharedConfiguration::GetControlShareName()
{
	return "ControlSegment";
}

const std::string SVSharedConfiguration::GetShareName()
{
	return "SVSharedMemory";
}

const std::string SVSharedConfiguration::GetLastInspectedName()
{
	return "last_inspected";
}

const std::string SVSharedConfiguration::GetRejectsName()
{
	return "rejects";
}

const std::string SVSharedConfiguration::GetPPQName()
{
	return "PPQs";
}

const std::string SVSharedConfiguration::GetPPQRejectsName()
{
	return "PPQrejects";
}

const std::string SVSharedConfiguration::GetMonitorListName()
{
	return "monitor_list";
}

const std::string SVSharedConfiguration::GetSharedMemoryDirectoryName()
{
	return GetSharedDrive() + "\\boost_interprocess"; //BOOST_INTERPROCESS_SHARED_DIR_PATH;
}

const std::string SVSharedConfiguration::GetImageDirectoryName()
{
	return GetSharedDrive() + "\\Images";
}

const std::string SVSharedConfiguration::GetRejectImageDirectoryName()
{
	return GetSharedDrive() + "\\RejectImages";
}

// return only the Drive letter (do not append the root dir "\\")
const std::string SVSharedConfiguration::GetSharedDrive()
{
	return "V:";
}

void SVSharedConfiguration::Log(const std::string & msg)
{
#ifdef ENABLE_LOG
	static std::ofstream os;
	if (!os.is_open())
	{
		os.open(LogFilename(), std::ios_base::app);
	}
	char buf[10] = {0};
	_itoa(::GetCurrentThreadId(), buf, 16);
	std::time_t tt = ::std::time(0);
	os << std::string(std::asctime(std::localtime(&tt))) + " " + std::string(buf) + ":\t " + msg << std::endl;
#endif
}

bool SVSharedConfiguration::SharedDriveExists()
{
	return PathFileExistsA(GetSharedDrive().c_str()) ? true : false;
}

bool SVSharedConfiguration::SharedDriveSizeOk()
{
	bool bRetVal = false;
	// Get Drive Size
	std::string volName = "\\\\.\\";
	volName += GetSharedDrive();
	HANDLE hDevice = CreateFileA(volName.c_str(),    // drive to open
                        GENERIC_READ,                // no access to the drive
                        FILE_SHARE_READ | FILE_SHARE_WRITE, 
                        nullptr,          // default security attributes
                        OPEN_EXISTING,    // disposition
                        0,                // file attributes
                        nullptr);         // do not copy file attributes

	if (INVALID_HANDLE_VALUE != hDevice)    // cannot open the drive
	{
		GET_LENGTH_INFORMATION li = { 0 };
		DWORD bytesReturned = 0;
		BOOL bResult = DeviceIoControl(hDevice,							// device to be queried
								IOCTL_DISK_GET_LENGTH_INFO,				// operation to perform
								nullptr, 0,								// no input buffer
								&li, sizeof(li),						// output buffer
								&bytesReturned ,						// # bytes returned
								static_cast<LPOVERLAPPED>(nullptr));	// synchronous I/O
		CloseHandle(hDevice);
		if (bResult) 
		{
			double size = static_cast<double>(li.Length.QuadPart) / static_cast<double>(1024 * 1024 * 1024);
			if (size >= 2.0)
			{
				/*
				// check available space?
				ULARGE_INTEGER lFreeBytesAvailableToCaller;
				ULARGE_INTEGER lTotalNumberOfBytes;
				ULARGE_INTEGER lTotalNumberOfFreeBytes;
				
				BOOL bOk = ::GetDiskFreeSpaceEx(GetSharedDrive().c_str(),   // pointer to the directory name
						&lFreeBytesAvailableToCaller, // receives the number of bytes on disk available to the caller
						&lTotalNumberOfBytes,         // receives the number of bytes on disk
						&lTotalNumberOfFreeBytes);   // receives the free bytes on disk
				if (bOk)
				{
					size = static_cast<double>(lFreeBytesAvailableToCaller.QuadPart) / static_cast<double>(1024 * 1024 * 1024);
					if (size >= 2.0)
					{
						bRetVal = true;
					}
				}
				*/
				bRetVal = true;
			}
		}
	}
	else
	{
		DWORD error = GetLastError();
		if (error == ERROR_ACCESS_DENIED) // could be mapped drive
		{
			ULARGE_INTEGER FreeBytesAvailable;
			ULARGE_INTEGER TotalNumberOfBytes;
			ULARGE_INTEGER TotalNumberOfFreeBytes;

			BOOL success = GetDiskFreeSpaceExA(volName.c_str(), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);
			if (success)
			{
				double size = static_cast<double>(FreeBytesAvailable.QuadPart) / static_cast<double>(1024 * 1024 * 1024);
				if (size >= 2.0)
				{
					bRetVal = true;
				}
			}
		}
	}
	return bRetVal;
}

HRESULT SVSharedConfiguration::SharedResourcesOk()
{
	HRESULT hr = S_OK;
	// check if drive exists and at least 2 Gig of space
	if (SharedDriveExists())
	{
		if (!SharedDriveSizeOk())
		{
			hr = STG_E_INSUFFICIENTMEMORY;
		}
	}
	else
	{
		hr = STG_E_PATHNOTFOUND;
	}
	return hr;
}

void SVSharedConfiguration::EnsureShareDirectoryExists()
{
	if (SVSharedConfiguration::SharedDriveExists())
	{
		const std::string& sharedMemoryDirectory = SVSharedConfiguration::GetSharedMemoryDirectoryName();
		DWORD res = GetFileAttributes(sharedMemoryDirectory.c_str());
		if (res == INVALID_FILE_ATTRIBUTES)
		{
			// Create the directory
			CreateDirectory(sharedMemoryDirectory.c_str(), nullptr);
		}
	}
}

bool SVSharedConfiguration::ControlFileExits()
{
	std::string fname;
	boost::interprocess::ipcdetail::get_tmp_base_dir(fname);
	fname += "\\" + GetControlShareName();
	return PathFileExistsA(fname.c_str()) ? true : false;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedConfiguration.cpp_v  $
 * 
 *    Rev 1.3   15 Oct 2014 19:02:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SharedDriveSizeOK to handle substituted drive
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Sep 2014 14:05:34   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Enabled logging.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Aug 2014 17:32:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added GetControlShareName method.
 * Added ControlFileExists method.
 * Revised SharedDriveExists method to be public.
 * Renamed fname method to LogFilename.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:07:38   sjones
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
