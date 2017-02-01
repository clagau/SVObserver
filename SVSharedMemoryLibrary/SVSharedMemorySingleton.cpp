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
#include "SVLibrary/SVOIniClass.h"
#include "SVSharedMemoryLibrary/SVSharedConfiguration.h"
#include "SVSharedMemoryLibrary/SVSharedMemorySingleton.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "ShareEvents.h"

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
	SvLib::SVOINIClass reader(SvStl::GlobalPath::Inst().GetSVIMIniPath());

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
		if (SVSharedConfiguration::SharedDriveExists()) // just check that the drive exists
		{
			const SVString& sharedMemoryDirectory = SVSharedConfiguration::GetSharedMemoryDirectoryName();
			DWORD res = GetFileAttributes(sharedMemoryDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				CreateDirectory(sharedMemoryDirectory.c_str(), nullptr);
			}
			const SVString& imageFileDirectory = SVSharedConfiguration::GetImageDirectoryName();
			res = GetFileAttributes(imageFileDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
				CreateDirectory(imageFileDirectory.c_str(), nullptr);
			}
			const SVString& rejectImageFileDirectory = SVSharedConfiguration::GetRejectImageDirectoryName();
			res = GetFileAttributes(rejectImageFileDirectory.c_str());
			if (res == INVALID_FILE_ATTRIBUTES)
			{
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
	
		SVString name = it->inspectionID.first;
		SVString shareName = name + "." + SVSharedConfiguration::GetShareName();
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

SVString SVSharedMemorySingleton::GetInspectionShareName(const SVGUID& rPPQGuid, const SVGUID& rGuid)
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


void SVSharedMemorySingleton::Destroy()
{
	if (SVSharedConfiguration::SharedDriveExists())
	{
		SVSharedMemorySingleton& instance = SVSharedMemorySingleton::Instance();
		instance.m_PPQSharedMemory.clear();
		instance.m_monitorListWriter.Release();
	}
}


void SVSharedMemorySingleton::ClearPPQSharedMemory()
{
	if (SVSharedConfiguration::SharedDriveExists() )
	{
		SVSharedMemorySingleton& instance = SVSharedMemorySingleton::Instance();
		instance.m_PPQSharedMemory.clear();
	}
}



bool SVSharedMemorySingleton::HasShares()
{
	return (SVSharedConfiguration::SharedDriveExists() );
}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/