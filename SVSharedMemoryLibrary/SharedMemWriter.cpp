//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySingleton
//* .File Name       : $Workfile:   SharedMemWriter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 Oct 2014 19:03:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SMParameterStruct.h"
#include "SharedMemIniFileTags.h"
#include "SharedMemWriter.h"
#include "SVLibrary/SVOIniClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

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
	}

	int SharedMemWriter::CreateManagment()
	{
		const SvSml::SVSharedMemorySettings& rSettings = SvSml::SharedMemWriter::Instance().GetSettings();
		SMParameterStruct Param(rSettings.GetCreateTimeout(), rSettings.GetCreateWaitTime());

		return m_DataContainer.CreateSlotManagment(m_MLContainer, Param);
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

	bool SharedMemWriter::clearInspectionIdsVector(const std::string& rPPQName)
	{
		return m_MLContainer.clearInspectionIdsVector(rPPQName);
	}

	bool SharedMemWriter::addInspectionIdEntry(const std::string& rPPQName, int ipMLId, int ipTRCId)
	{
		return m_MLContainer.addInspectionIdEntry(rPPQName, ipMLId, ipTRCId);
	}

	void  SharedMemWriter::CalculateStoreIds()
	{
		return m_MLContainer.CalculateStoreIds();
	}

	void SharedMemWriter::setDataTrcPos(const std::string& rPPQName, int inspectionStoreId, int inspectionTRCPos, const std::unordered_map<uint32_t, int>& rDataDefMap, const std::unordered_map<uint32_t, int>& rImageMap, const std::unordered_map<uint32_t, int>& rChildImageMap)
	{
		m_MLContainer.setDataTrcPos(rPPQName, inspectionStoreId, inspectionTRCPos, rDataDefMap, rImageMap, rChildImageMap);
	}

	DWORD  SharedMemWriter::GetActiveMonitorListCount() const
	{
		return m_MLContainer.GetActiveMonitorlistCount();
	}

} //namespace SvSml
