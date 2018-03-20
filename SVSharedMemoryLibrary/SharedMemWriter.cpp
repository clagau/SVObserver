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
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "MLInspectionInfo.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/MatroxImageProps.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SharedMemIniFileTags.h"

namespace SvSml
{
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

	int SharedMemWriter::CreateManagmentAndStores()
	{
		const SvSml::SVSharedMemorySettings& rSettings = SvSml::SharedMemWriter::Instance().GetSettings();
		SMParameterStruct Param(rSettings.GetCreateTimeout(),rSettings.GetCreateWaitTime());

		m_DataContainer.CreateSlotManagment(m_MLContainer, Param);
		return m_DataContainer.CreateStores(m_MLContainer, Param);
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
		int  monitorStoreSize = reader.GetValueInt(SharedMemorySectionTag, MonitorStoreSizeTag, SvSml::SVSharedMemorySettings::DefaultMonitorStoreSize);
		int  numProductSlot = reader.GetValueInt(SharedMemorySectionTag, NumProductSlotTag, SvSml::SVSharedMemorySettings::DefaultNumProductSlot);
		int ConnectionTimout = reader.GetValueInt(SharedMemorySectionTag, CreateTimeoutTag, SvSml::SVSharedMemorySettings::DefaultConnectionTimout);
		int CreateWaitTime = reader.GetValueInt(SharedMemorySectionTag, CreateWaitTimeTag, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
		m_settings.SetMonitorStoreSize(monitorStoreSize);
		m_settings.SetNumProductSlot(numProductSlot);
		m_settings.SeCreateTimout(ConnectionTimout);
		m_settings.SetCreateWaitTime(CreateWaitTime);
	}

	void SharedMemWriter::CheckDirectories()
	{
		// Ensure Directories exist
		try
		{
			if (SVSharedConfiguration::SharedDriveExists()) // just check that the drive exists
			{
				const std::string& sharedMemoryDirectory = SVSharedConfiguration::GetSharedMemoryDirectoryName();
				DWORD res = GetFileAttributes(sharedMemoryDirectory.c_str());
				if (res == INVALID_FILE_ATTRIBUTES)
				{
					CreateDirectory(sharedMemoryDirectory.c_str(), nullptr);
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

	void SharedMemWriter::Destroy()
	{
		CloseDataConnection();
		m_monitorListStore.CloseConnection();
		ClearMonitorListCpyVector();
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

	DWORD SharedMemWriter::GetInspectionImageSize(const std::string& inspectionName)
	{
		return m_MLContainer.GetInspectionImageSize(inspectionName);
	}
	DWORD SharedMemWriter::GetInspectionStoreId(const std::string& InspectionName)
	{
		return m_MLContainer.GetInspectionStoreId(InspectionName);
	}
	const MonitorListCpy*  SharedMemWriter::GetMonitorListCpyPointer(const std::string& Monitorlistname)  const
	{
		return m_MLContainer.GetMonitorListCpyPointer(Monitorlistname);
	}
	const MonitorListCpy*  SharedMemWriter::GetMonitorListCpyPointerForPPQ(const std::string& PPQNAME)  const
	{
		return m_MLContainer.GetMonitorListCpyPointerForPPQ(PPQNAME);
	}
	void SharedMemWriter::Insert(MonitorListCpyPointer& MLCpyPtr)
	{
		return m_MLContainer.Insert(MLCpyPtr);
	}

	MonitorEntryPointer SharedMemWriter::GetMonitorEntryPointer(const std::string& rname)
	{
		return m_MLContainer.GetMonitorEntryPointer(rname);
	}

	void  SharedMemWriter::ClearMonitorListCpyVector()
	{
		return m_MLContainer.Clear();
	}
	void SharedMemWriter::WriteMonitorList()
	{
		SvPml::MesMLCpyContainer MesMLCpyCont;
		m_MLContainer.BuildProtoMessage(MesMLCpyCont);
		size_t size = MesMLCpyCont.ByteSizeLong();
		const SvSml::SVSharedMemorySettings& rSettings = SvSml::SharedMemWriter::Instance().GetSettings();
		SMParameterStruct Param(rSettings.GetCreateTimeout(), rSettings.GetCreateWaitTime());
		m_monitorListStore.CreateMonitorStore("Monitor", static_cast<DWORD>(size), Param);
		MesMLCpyCont.SerializeToArray(m_monitorListStore.GetPtr(), static_cast<DWORD>(size));
		return;
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
