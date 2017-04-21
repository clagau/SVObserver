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
#include "SVSharedConfiguration.h"
#include "SharedmemWriter.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "ShareEvents.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "MLInspectionInfo.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/MatroxImageProps.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"

namespace Seidenader { namespace SVSharedMemoryLibrary
{

static const double TwentyPercent = .20;

SharedMemWriter& SharedMemWriter::Instance()
{
	static SharedMemWriter l_Object;
	return l_Object;
}

SharedMemWriter::~SharedMemWriter()
{
	ClearMonitorListCpyVector();
}

SharedMemWriter::SharedMemWriter()
{
	ReadSettings();
	CheckDirectories();
}

void SharedMemWriter::ReadSettings()
{
	SvLib::SVOINIClass reader(SvStl::GlobalPath::Inst().GetSVIMIniPath());

	long monitorStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("MonitorStoreSize"), SvSml::SVSharedMemorySettings::DefaultMonitorStoreSize);
	long productStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("ProductStoreSize"), SvSml::SVSharedMemorySettings::DefaultProductStoreSize);
	long dataStoreSize = reader.GetValueInt(_T("SharedMemory"), _T("DataStoreSize"), SvSml::SVSharedMemorySettings::DefaultDataStoreSize);
	
	m_settings = SvSml::SVSharedMemorySettings(monitorStoreSize, productStoreSize, dataStoreSize);
}

void SharedMemWriter::CheckDirectories()
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

HRESULT SharedMemWriter::InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const long ProductSlots, const long RejectSlots, const SvSml::InspectionWriterCreationInfos& rCreationInfos)
{
	HRESULT hr = S_OK;
	PPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);
	if (it == m_PPQSharedMemory.end())
	{
		size_t size = CalcPPQSharedMemorySize(rName, rCreationInfos);
		m_PPQSharedMemory[rGuid] = std::unique_ptr<SVSharedPPQWriter>(new SVSharedPPQWriter);
		hr = m_PPQSharedMemory[rGuid]->Create(rName.c_str(), rCreationInfos, m_settings, ProductSlots, RejectSlots, size);
	}
	return hr;
}

int SharedMemWriter::CreateImageStores(const SVString& InspName, long ProductSlots, long RejectSlots )
{
	return m_ImageContainer.CreateImageStores(InspName,ProductSlots,RejectSlots,m_MLContainer); 
}



HRESULT SharedMemWriter::ErasePPQSharedMemory(const SVGUID& rGuid)
{
	HRESULT hr = S_OK;
	PPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);
	if (it != m_PPQSharedMemory.end())
	{
		m_PPQSharedMemory.erase(it);
	}
	return hr;
}

SvSml::SVSharedPPQWriter& SharedMemWriter::GetPPQWriter(const SVGUID& rGuid)
{
	PPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rGuid);

	if (it != m_PPQSharedMemory.end())
	{
		return *(it->second.get());
	}
	throw std::exception("SharedPPQWriter Not Found");
}

SvSml::SVSharedInspectionWriter& SharedMemWriter::GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid)
{
	PPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rPPQGuid);
	if (it != m_PPQSharedMemory.end())
	{
		return it->second->operator[](rGuid);
	}
	throw std::exception("SharedInspectionWriter Not Found");
}

SVString SharedMemWriter::GetInspectionShareName(const SVGUID& rPPQGuid, const SVGUID& rGuid)
{
	PPQSharedMemoryMap::iterator it = m_PPQSharedMemory.find(rPPQGuid);
	if (it != m_PPQSharedMemory.end())
	{
		return it->second->operator[](rGuid).GetShareName();
		//return it->second[rGuid].GetShareName();
	}
	throw std::exception("SharedInspectionWriter Not Found");
}

const SvSml::SVSharedMemorySettings& SharedMemWriter::GetSettings() const
{
	return m_settings;
}

SvSml::SVMonitorListWriter& SharedMemWriter::GetMonitorListWriter()
{
	return m_monitorListWriter;
}


void SharedMemWriter::Destroy()
{
	{
		ClearPPQSharedMemory();
		m_monitorListWriter.Release();
		ClearMonitorListCpyVector();
	}
}

void SharedMemWriter::ClearPPQSharedMemory()
{
	m_ImageContainer.DestroySharedMemory();
	m_PPQSharedMemory.clear();
}

BYTE*  SharedMemWriter::GetImageBufferPtr(DWORD  SlotIndex, SharedImageStore::StoreType Store, DWORD storeIndex , DWORD storeoffset)
{
	return m_ImageContainer.GetImageBufferPtr(SlotIndex, Store, storeIndex , storeoffset);
	
}


///Return the MatroxSharedBuffer;
SVMatroxBuffer& SharedMemWriter::GetImageBuffer(DWORD  SlotIndex, SharedImageStore::StoreType t, DWORD storeIndex, DWORD ImageIndex)
{
	return m_ImageContainer.GetImageBuffer(SlotIndex,t,storeIndex,ImageIndex);
}

///Creates a MatroxSharedBuffer for all images in the Monitorlist;
void SharedMemWriter::CreateSharedMatroxBuffer()
{
	m_ImageContainer.CreateSharedMatroxBuffer(m_MLContainer);
}
bool SharedMemWriter::HasShares()
{
	return true;
	//@Todo[MEC][7.50] [21.02.2017]
	//return (SVSharedConfiguration::SharedDriveExists() );
}


DWORD SharedMemWriter::GetInspectionImageSize(const SVString& inspectionName   )
{

	return m_MLContainer.GetInspectionImageSize(inspectionName);

}
void SharedMemWriter::Insert(MonitorListCpyPointer& MLCpyPtr)
{
	return m_MLContainer.Insert(MLCpyPtr);
}

MonitorEntryPointer SharedMemWriter::GetMonitorEntryPointer(const SVString& rname)
{
	return m_MLContainer.GetMonitorEntryPointer(rname);
}

void  SharedMemWriter::ClearMonitorListCpyVector()
{
	return m_MLContainer.Clear();
}
void SharedMemWriter::WriteMonitorList()  
{
	return m_MLContainer.WriteMonitorList(m_monitorListWriter);
}

void  SharedMemWriter::CalculateStoreIds()
{
	return m_MLContainer.CalculateStoreIds();
}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/