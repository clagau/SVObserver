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
#include "SVSharedMemoryLibrary/SVSharedMemorySingleton.h"
#include "SVStatusLibrary/GlobalPath.h"

namespace Seidenader { namespace SVSharedMemoryLibrary
{

static const double TwentyPercent = .20;

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
	SvOml::SVOINIClass reader(SvStl::GlobalPath::Inst().GetSVIMIniPath());

	long monitorStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("MonitorStoreSize"), SvSml::SVSharedMemorySettings::DefaultMonitorStoreSize);
	long productStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("ProductStoreSize"), SvSml::SVSharedMemorySettings::DefaultProductStoreSize);
	long dataStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("DataStoreSize"), SvSml::SVSharedMemorySettings::DefaultDataStoreSize);
	
	m_settings = SvSml::SVSharedMemorySettings(monitorStoreSize, productStoreSize, dataStoreSize);
}

void SVSharedMemorySingleton::CheckDirectories()
{
	// Ensure Directories exist
	try
	{
		//if (S_OK == SvSml::SVSharedConfiguration::SharedResourcesOk()) // Check if drive exists and enough space
		if (SVSharedConfiguration::SharedDriveExists()) // just check that the drive exists
		{
			const std::string& sharedMemoryDirectory = SVSharedConfiguration::GetSharedMemoryDirectoryName();
			DWORD res = GetFileAttributes(sharedMemoryDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				// Create the directory
				CreateDirectory(sharedMemoryDirectory.c_str(), nullptr);
			}
			const std::string& imageFileDirectory = SVSharedConfiguration::GetImageDirectoryName();
			res = GetFileAttributes(imageFileDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				// Create the directory
				CreateDirectory(imageFileDirectory.c_str(), nullptr);
			}
			const std::string& rejectImageFileDirectory = SVSharedConfiguration::GetRejectImageDirectoryName();
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

////////////////////////////////////////////////////////////////////////////
// Calculate the size required to hold the Inspection data in the PPQ Share
////////////////////////////////////////////////////////////////////////////
static size_t CalcPPQSharedMemorySize(const SVString& rName, const SvSml::InspectionWriterCreationInfos& rCreationInfos)
{
	size_t size = rName.size() + sizeof(SvSml::SVSharedProduct);
	for (SvSml::InspectionWriterCreationInfos::const_iterator it = rCreationInfos.begin(); it != rCreationInfos.end();++it)
	{
	
		std::string name = it->inspectionID.first;
		std::string shareName = name + "." + SVSharedConfiguration::GetShareName();
		size += shareName.length() + name.length() + sizeof(SvSml::SVSharedInspection);
	}
	size += static_cast<size_t>(static_cast<double>(size) * TwentyPercent);
	return size;
}

HRESULT SVSharedMemorySingleton::InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const long ProductSlots, const long RejectSlots, const SvSml::InspectionWriterCreationInfos& rCreationInfos)
{
	HRESULT hr = S_OK;

	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);

	if (it == m_PPQSharedMemory.end())
	{
		size_t size = CalcPPQSharedMemorySize(rName, rCreationInfos);
		SvSml::SVSharedPPQWriter& rMemory = m_PPQSharedMemory[rGuid];
		hr = rMemory.Create(rName.c_str(), rCreationInfos, m_settings, ProductSlots, RejectSlots, size);
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

SvSml::SVSharedPPQWriter& SVSharedMemorySingleton::GetPPQWriter(const SVGUID& rGuid)
{
	SVPPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);

	if (it != m_PPQSharedMemory.end())
	{
		return it->second;
	}
	throw std::exception("SharedPPQWriter Not Found");
}

SvSml::SVSharedInspectionWriter& SVSharedMemorySingleton::GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid)
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

const SvSml::SVSharedMemorySettings& SVSharedMemorySingleton::GetSettings() const
{
	return m_settings;
}

SvSml::SVMonitorListWriter& SVSharedMemorySingleton::GetMonitorListWriter()
{
	return m_monitorListWriter;
}

SvSml::SVShareControlHandler& SVSharedMemorySingleton::GetIPCShare()
{
	return m_shareControlHandler;
}

HRESULT SVSharedMemorySingleton::SetProductFilter(const SVString& listName, SvSml::SVProductFilterEnum filter)
{
	HRESULT hr = S_OK;
	if (SVSharedConfiguration::SharedDriveExists() && SVSharedConfiguration::ControlFileExits())
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

void SVSharedMemorySingleton::QuiesceSharedMemory()
{
	SvSml::SVShareControlHandler& rControlShare = SVSharedMemorySingleton::Instance().GetIPCShare();
	if (rControlShare.IsCreated())
	{
		rControlShare.ClearReady();
		bool bStop = false;
		long timetoWait = 25; // ticks are approx 10 to 16 ms
		long startTime = GetTickCount();
		while (!bStop)
		{
			bStop = rControlShare.GotAck();
			if (!bStop)
			{
				long curTime = GetTickCount();
				if ((curTime - startTime) > timetoWait)
				{
					bStop = true;
				}
				else
				{
					::Sleep(20);
				}
			}
		}
	}
}



void SVSharedMemorySingleton::Destroy()
{
	if (SVSharedConfiguration::SharedDriveExists() && SVSharedConfiguration::ControlFileExits())
	{
		SVSharedMemorySingleton& instance = SVSharedMemorySingleton::Instance();
		instance.m_PPQSharedMemory.clear();
		instance.m_monitorListWriter.Release();
		instance.m_shareControlHandler.Release();
	}
}


void SVSharedMemorySingleton::ClearPPQSharedMemory()
{
	if (SVSharedConfiguration::SharedDriveExists() && SVSharedConfiguration::ControlFileExits())
	{
		SVSharedMemorySingleton& instance = SVSharedMemorySingleton::Instance();
		instance.m_PPQSharedMemory.clear();
	}
}



bool SVSharedMemorySingleton::HasShares()
{
	return (SVSharedConfiguration::SharedDriveExists() && SVSharedConfiguration::ControlFileExits());
}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/