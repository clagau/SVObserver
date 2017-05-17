//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLCpyContainer.cpp
/// All Rights Reserved 
//*****************************************************************************
///class encapsulate an container with all active monitorlists 
//******************************************************************************
#include "StdAfx.h"
#include "MLcpyContainer.h"
#include "SVMonitorListReader.h"
#include "SVMonitorListWriter.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"


namespace SvSml
{
	MLCpyContainer::MLCpyContainer(void)
	{
		m_Version = 0;
	}


	MLCpyContainer::~MLCpyContainer(void)
	{
	}

	void MLCpyContainer::Clear()
	{
		m_Version = 0;
		m_InspectionInfoMap.clear();
		m_MonitorListCpyMap.clear();

	}
	//! Load MonitorLists from Shared  Memory
	//! \param mlReader [in]
	//! \returns void
	void MLCpyContainer::ReloadMonitorMap(SVMonitorListReader& rmlReader, DWORD version)
	{
		rmlReader.Open();
		Clear();
		std::vector<SVString> mlNames = rmlReader.GetListNames();
		std::vector<SVString>::const_iterator it;
		for (it = mlNames.begin(); it != mlNames.end(); it++)
		{
			m_MonitorListCpyMap[*it] = MonitorListCpyPointer(new MonitorListCpy(rmlReader[*it]));
			m_MonitorListCpyMap[*it]->InsertToMLInspectionInfoMap(m_InspectionInfoMap);
		}
		rmlReader.Close();
		m_Version = version;
	}


	DWORD MLCpyContainer::GetVersion()  const
	{
		return m_Version;
	}

	bool MLCpyContainer::IsActiveMonitorList(const SVString& Monitorlistname) const
	{
		MonitorListCpyMap::const_iterator it;
		it = m_MonitorListCpyMap.find(Monitorlistname);
		if (it != m_MonitorListCpyMap.end())
			return it->second->GetIsActive();
		else
			return false;
	}

	const MonitorListCpy*  MLCpyContainer::GetMonitorListCpyPointer(const SVString& Monitorlistname)  const
	{
		MonitorListCpyMap::const_iterator it = m_MonitorListCpyMap.find(Monitorlistname);

		if (it != m_MonitorListCpyMap.end())
			return it->second.get();
		else
			return nullptr;
	}


	void MLCpyContainer::Insert(MonitorListCpyPointer& MLCpyPtr)
	{
		SVString monitorlistName = MLCpyPtr->GetMonitorlistname();
		MLCpyPtr->InsertToMLInspectionInfoMap(m_InspectionInfoMap);
		m_MonitorListCpyMap[monitorlistName] = std::move(MLCpyPtr);
	}



	DWORD MLCpyContainer::GetInspectionImageSize(const SVString& inspectionName)
	{
		SvSml::MLInspectionInfoMap::const_iterator it = m_InspectionInfoMap.find(inspectionName);
		if (it != m_InspectionInfoMap.end() && it->second.get())
		{
			return it->second->TotalImageSize;
		}
		else
		{
			return 0;
		}
	}

	void MLCpyContainer::WriteMonitorList(SVMonitorListWriter& rWriter)
	{
		MonitorListCpyMap::const_iterator  it;
		for (it = m_MonitorListCpyMap.begin(); it != m_MonitorListCpyMap.end(); ++it)
		{
			if (it->second.get() && it->second->GetIsActive())
			{
				it->second->WriteMonitorList(rWriter);
			}
		}
	}


	MonitorEntryPointer MLCpyContainer::GetMonitorEntryPointer(const SVString& rname)
	{
		MonitorListCpyMap::iterator  it;
		for (it = m_MonitorListCpyMap.begin(); it != m_MonitorListCpyMap.end(); ++it)
		{
			MonitorEntriesMap::iterator MEMIt = it->second->m_EntriesMap.find(rname);
			if (MEMIt != it->second->m_EntriesMap.end())
			{
				return MEMIt->second;
			}
		}

		throw std::exception("Cant find name in Monitorentrymap");

	}

	void MLCpyContainer::CalculateStoreIds()
	{
		std::vector<DWORD> ImageItemoffsets;
		std::vector<DWORD> ImageItemIndexes;
		DWORD Storeindex(0);
		MLInspectionInfoMap::iterator it;
		for (Storeindex = 0, it = m_InspectionInfoMap.begin(); it != m_InspectionInfoMap.end(); ++it, ++Storeindex)
		{
			it->second->StoreIndex = Storeindex;
			ImageItemoffsets.push_back(0);
			ImageItemIndexes.push_back(0);
		}
		MonitorListCpyMap::iterator  MLCPyIt;
		for (MLCPyIt = m_MonitorListCpyMap.begin(); MLCPyIt != m_MonitorListCpyMap.end(); ++MLCPyIt)
		{
			if (!MLCPyIt->second.get())
				continue;
			MonitorEntriesMap::iterator MEMIt = MLCPyIt->second->m_EntriesMap.begin();
			for (; MEMIt != MLCPyIt->second->m_EntriesMap.end(); ++MEMIt)
			{
				assert(MEMIt->second.get());
				SVString inspectionName;
				std::size_t pos = MEMIt->first.find_first_of('.');
				if (pos != SVString::npos)
					inspectionName = MEMIt->first.substr(0, pos);
				else
					inspectionName = MEMIt->first;

				assert(m_InspectionInfoMap.find(inspectionName) != m_InspectionInfoMap.end());
				assert(MEMIt->second->name == MEMIt->first);

				switch (MEMIt->second->ObjectType)
				{
				case SVImageObjectType:
				{
					DWORD storeId = m_InspectionInfoMap[inspectionName]->StoreIndex;
					MEMIt->second->InspectionStoreId = storeId;
					MEMIt->second->ItemId = ImageItemIndexes[storeId]++;
					MEMIt->second->Store_Offset = ImageItemoffsets[storeId];
					ImageItemoffsets[storeId] += MonitorListCpy::ImageBufferHeaderSize;
					ImageItemoffsets[storeId] += (DWORD)MEMIt->second->ByteSize;
				}
				break;

				default:
					continue;
					break;

				}

			}
		}

	}
} //namespace SvSml
