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

namespace SvSml
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

	int SharedMemWriter::CreateManagmentAndStores(DWORD Productslot)
	{
		m_DataContainer.CreateSlotManagment(Productslot, m_MLContainer);
		return m_DataContainer.CreateStores(m_MLContainer);
	}

	RingBufferPointer SharedMemWriter::GetSlotManager(LPCTSTR PPQname)
	{
		if (m_MLContainer.m_PPQInfoMap.find(PPQname) != m_MLContainer.m_PPQInfoMap.end() && m_MLContainer.m_PPQInfoMap[PPQname].get())
		{
			int index = m_MLContainer.m_PPQInfoMap[PPQname]->SlotManagerIndex;
			return m_DataContainer.GetSlotManager(index);
		}
		else
		{
			return nullptr;
		}
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
		catch (...)
		{
		}
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
			CloseDataConnection();
			m_monitorListWriter.Release();
			ClearMonitorListCpyVector();
		}
	}

	void SharedMemWriter::CloseDataConnection()
	{
		m_DataContainer.CloseConnection();
	}

	BYTE*  SharedMemWriter::GetDataBufferPtr(DWORD  SlotIndex, DWORD storeIndex, DWORD storeoffset)
	{
		return m_DataContainer.GetDataBufferPtr(SlotIndex, storeIndex, storeoffset);
	}
	
	///Return the MatroxSharedBuffer;
	SVMatroxBuffer& SharedMemWriter::GetImageBuffer(DWORD  SlotIndex,  DWORD storeIndex, DWORD ImageIndex)
	{
		return m_DataContainer.GetImageBuffer(SlotIndex,  storeIndex, ImageIndex);
	}

	///Creates a MatroxSharedBuffer for all images in the Monitorlist;
	void SharedMemWriter::CreateSharedMatroxBuffer()
	{
		m_DataContainer.CreateSharedMatroxBuffer(m_MLContainer);

	}
	bool SharedMemWriter::HasShares()
	{
		return true;
	}


	DWORD SharedMemWriter::GetInspectionImageSize(const SVString& inspectionName)
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
	DWORD  SharedMemWriter::GetActiveMonitorListCount() const
	{
		return m_MLContainer.GetActiveMonitorlistCount();
							 
	}

} //namespace SvSml
