//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySingleton
//* .File Name       : $Workfile:   SVSharedMemorySingleton.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 Oct 2014 19:03:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVOMFCLibrary/SVOIniClass.h"
#include "SVSharedMemoryLibrary/SVSharedConfiguration.h"
#include "SVSharedMemorySingleton.h"

SVSharedMemorySingleton& SVSharedMemorySingleton::Instance()
{
	static SVSharedMemorySingleton l_Object;
	return l_Object;
}

SVSharedMemorySingleton::~SVSharedMemorySingleton()
{
}

SVSharedMemorySingleton::SVSharedMemorySingleton()
{
	ReadSettings();
	CheckDirectories();
}

void SVSharedMemorySingleton::ReadSettings()
{
	SVOINIClass reader;
	reader.SetFile(_T("C:\\SVObserver\\bin\\SVIM.ini"));

	long monitorStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("MonitorStoreSize"), SVSharedMemorySettings::DefaultMonitorStoreSize);
	long productStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("ProductStoreSize"), SVSharedMemorySettings::DefaultProductStoreSize);
	long dataStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("DataStoreSize"), SVSharedMemorySettings::DefaultDataStoreSize);
	long numProductSlots = reader.GetValueInt(_T("SharedMemory"), _T("ProductNumSlots"), SVSharedMemorySettings::DefaultProductNumSlots);
	long numRejectSlots = reader.GetValueInt(_T("SharedMemory"), _T("RejectsNumSlots"), SVSharedMemorySettings::DefaultRejectsNumSlots);
	
	m_settings = SVSharedMemorySettings(monitorStoreSize, productStoreSize, dataStoreSize, numProductSlots, numRejectSlots);
}

void SVSharedMemorySingleton::CheckDirectories()
{
	// Ensure Directories exist
	try
	{
		//if (S_OK == SeidenaderVision::SVSharedConfiguration::SharedResourcesOk()) // Check if drive exists and enough space
		if (SeidenaderVision::SVSharedConfiguration::SharedDriveExists()) // just check that the drive exists
		{
			const std::string& sharedMemoryDirectory = SeidenaderVision::SVSharedConfiguration::GetSharedMemoryDirectoryName();
			DWORD res = GetFileAttributes(sharedMemoryDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				// Create the directory
				CreateDirectory(sharedMemoryDirectory.c_str(), nullptr);
			}
			const std::string& imageFileDirectory = SeidenaderVision::SVSharedConfiguration::GetImageDirectoryName();
			res = GetFileAttributes(imageFileDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				// Create the directory
				CreateDirectory(imageFileDirectory.c_str(), nullptr);
			}
			const std::string& rejectImageFileDirectory = SeidenaderVision::SVSharedConfiguration::GetRejectImageDirectoryName();
			res = GetFileAttributes(rejectImageFileDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				// Create the directory
				CreateDirectory(rejectImageFileDirectory.c_str(), nullptr);
			}
		}
	}
	catch(...)
	{
	}
}

HRESULT SVSharedMemorySingleton::InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const SeidenaderVision::InspectionIDs& rIDs)
{
	HRESULT hr = S_OK;

	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);

	if (it == m_PPQSharedMemory.end())
	{
		SeidenaderVision::SVSharedPPQWriter& rMemory = m_PPQSharedMemory[rGuid];
		hr = rMemory.Create(rName.c_str(), rIDs, m_settings);
	}
	return hr;
}

HRESULT SVSharedMemorySingleton::ErasePPQSharedMemory(const SVGUID& rGuid)
{
	HRESULT hr = S_OK;
	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);
	if (it != m_PPQSharedMemory.end())
	{
		m_PPQSharedMemory.erase(it);
	}
	return hr;
}

SeidenaderVision::SVSharedPPQWriter& SVSharedMemorySingleton::GetPPQWriter(const SVGUID& rGuid)
{
	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);

	if (it != m_PPQSharedMemory.end())
	{
		return it->second;
	}
	throw std::exception("SharedPPQWriter Not Found");
}

SeidenaderVision::SVSharedInspectionWriter& SVSharedMemorySingleton::GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid)
{
	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rPPQGuid);
	if (it != m_PPQSharedMemory.end())
	{
		return it->second[rGuid];
	}
	throw std::exception("SharedInspectionWriter Not Found");
}

std::string SVSharedMemorySingleton::GetInspectionShareName(const SVGUID& rPPQGuid, const SVGUID& rGuid)
{
	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rPPQGuid);
	if (it != m_PPQSharedMemory.end())
	{
		return it->second[rGuid].GetShareName();
	}
	throw std::exception("SharedInspectionWriter Not Found");
}

const SVSharedMemorySettings& SVSharedMemorySingleton::GetSettings() const
{
	return m_settings;
}

SeidenaderVision::SVMonitorListWriter& SVSharedMemorySingleton::GetMonitorListWriter()
{
	return m_monitorListWriter;
}

SeidenaderVision::SVShareControlHandler& SVSharedMemorySingleton::GetIPCShare()
{
	return m_shareControlHandler;
}

long SVSharedMemorySingleton::GetRejectDepth() const
{
	return m_settings.NumRejectSlots();
}

void SVSharedMemorySingleton::SetRejectDepth(long rejectDepth)
{
	m_settings.SetNumRejectSlots(rejectDepth);
}

void SVSharedMemorySingleton::SetProductDepth(long productDepth, long extra)
{
	m_settings.SetNumProductSlots(productDepth + extra);
}

HRESULT SVSharedMemorySingleton::SetProductFilter(const SVString& listName, SVProductFilterEnum filter)
{
	HRESULT hr = S_OK;
	if (SeidenaderVision::SVSharedConfiguration::SharedDriveExists() && SeidenaderVision::SVSharedConfiguration::ControlFileExits())
	{
		SVSharedMemorySingleton& instance = SVSharedMemorySingleton::Instance();
		try
		{
			instance.m_monitorListWriter.SetProductFilter(listName.c_str(), filter);
			instance.m_shareControlHandler.SetProductFilterChanged();
		}
		catch (std::exception& e)
		{
			UNREFERENCED_PARAMETER(e);
			hr = E_UNEXPECTED;
		}
	}
	else
	{
		hr = E_HANDLE;
	}
	return hr;
}

void SVSharedMemorySingleton::Destroy()
{
	if (SeidenaderVision::SVSharedConfiguration::SharedDriveExists() && SeidenaderVision::SVSharedConfiguration::ControlFileExits())
	{
		SVSharedMemorySingleton& instance = SVSharedMemorySingleton::Instance();
		instance.m_PPQSharedMemory.clear();
		instance.m_monitorListWriter.Release();
		instance.m_shareControlHandler.Release();
	}
}

bool SVSharedMemorySingleton::HasShares()
{
	return (SeidenaderVision::SVSharedConfiguration::SharedDriveExists() && SeidenaderVision::SVSharedConfiguration::ControlFileExits());
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSharedMemorySingleton.cpp_v  $
 * 
 *    Rev 1.4   15 Oct 2014 19:03:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CheckDirectories to only check for the existance of the drive and not check the size
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Aug 2014 18:48:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetProductFilter method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Aug 2014 12:06:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added HasShares method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Aug 2014 17:43:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Destroy method to be static and to check for the existence of the Shared Memory Drive and Control Shared Memory file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:56:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jan 2012 18:48:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Revised to log exception if shared memory creation fails due to low disk space
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 15:24:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files associated with socket and shared memory functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/