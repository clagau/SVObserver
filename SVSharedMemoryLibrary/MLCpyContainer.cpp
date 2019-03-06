//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLCpyContainer.cpp
/// All Rights Reserved 
//*****************************************************************************
///class encapsulate an container with all active monitorlists 
//******************************************************************************
#include "StdAfx.h"
#include "MLcpyContainer.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/StringHelper.h"


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
		m_PPQInfoMap.clear();
	}
	DWORD MLCpyContainer::GetVersion()  const
	{
		return m_Version;
	}

	int MLCpyContainer::GetAllActiveMonitorlistName(SvDef::StringVector& monotorlistnames) const
	{
			
			for (auto& mlc : m_MonitorListCpyMap)
			{
				if (mlc.second->GetIsActive())
				{ 
					monotorlistnames.push_back(mlc.first);
					
				}
					
			}
			return static_cast<int>(monotorlistnames.size());
		
	}
	
	DWORD MLCpyContainer::GetActiveMonitorlistCount() const
	{
		DWORD res(0);
		for (auto& mlc : m_MonitorListCpyMap)
		{
			if (mlc.second->GetIsActive())
				res++;
		}
		return res;
	}

	bool MLCpyContainer::IsActiveMonitorList(const std::string& Monitorlistname) const
	{
		MonitorListCpyMap::const_iterator it;
		it = m_MonitorListCpyMap.find(Monitorlistname);
		if (it != m_MonitorListCpyMap.end())
			return it->second->GetIsActive();
		else
			return false;
	}

	const MonitorListCpy*  MLCpyContainer::GetMonitorListCpyPointer(const std::string& Monitorlistname)  const
	{
		MonitorListCpyMap::const_iterator it = m_MonitorListCpyMap.find(Monitorlistname);

		if (it != m_MonitorListCpyMap.end())
			return it->second.get();
		else
			return nullptr;
	}
	
	const MonitorListCpy*  MLCpyContainer::GetMonitorListCpyPointerForPPQ(const std::string& PPQName)  const
	{
		for (auto& element : m_MonitorListCpyMap)
		{
			if (element.second->m_ppqName == PPQName)
				return element.second.get();
		}
		return nullptr;
	}
	
	DWORD MLCpyContainer::GetInspectionStoreId(const std::string& InspectionName) const
	{
		auto& it = m_InspectionInfoMap.find(InspectionName);
		if (it != m_InspectionInfoMap.end())
		{
			return it->second->StoreIndex;
		}
		return -1;
	}

	void MLCpyContainer::Insert(MonitorListCpyPointer& MLCpyPtr)
	{
		std::string monitorlistName = MLCpyPtr->GetMonitorlistname();
		MLCpyPtr->InsertToMLInspectionInfoMap(m_InspectionInfoMap, m_PPQInfoMap);
		m_MonitorListCpyMap[monitorlistName] = std::move(MLCpyPtr);
	}

	DWORD MLCpyContainer::GetInspectionImageSize(const std::string& inspectionName) const
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


	MonitorEntryPointer MLCpyContainer::GetMonitorEntryPointer(const std::string& rname) 
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
		std::vector<DWORD> DataItemOffset;
		std::vector<DWORD> DataItemIndex;

		DWORD Storeindex(0);
		MLInspectionInfoMap::iterator it;
		for (Storeindex = 0, it = m_InspectionInfoMap.begin(); it != m_InspectionInfoMap.end(); ++it, ++Storeindex)
		{
			it->second->StoreIndex = Storeindex;
			ImageItemoffsets.push_back(0);
			ImageItemIndexes.push_back(0);
			DataItemOffset.push_back(0);
			DataItemIndex.push_back(0);

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
				std::string inspectionName {MEMIt->first};
				std::string Inspections {SvDef::FqnInspections};
				Inspections += _T('.');
				if (0 == inspectionName.find(Inspections))
				{
					inspectionName = inspectionName.substr(Inspections.size(), inspectionName.size() - Inspections.size());
				}

				std::size_t pos = inspectionName.find_first_of('.');
				if (pos != std::string::npos)
				{
					inspectionName = inspectionName.substr(0, pos);
				}

				assert(m_InspectionInfoMap.find(inspectionName) != m_InspectionInfoMap.end());
				assert(MEMIt->second->name == MEMIt->first);

				DWORD storeId = m_InspectionInfoMap[inspectionName]->StoreIndex;
				switch (MEMIt->second->data.ObjectType)
				{
				case SvPb::SVImageObjectType:
				{
					MEMIt->second->data.InspectionStoreId = storeId;
					MEMIt->second->data.ItemId = ImageItemIndexes[storeId]++;
					MEMIt->second->data.Store_Offset = ImageItemoffsets[storeId];
					ImageItemoffsets[storeId] += MonitorListCpy::ImageBufferHeaderSize;
					ImageItemoffsets[storeId] += (DWORD)MEMIt->second->data.ByteSize;
				}
				break;

				default:
				{
					MEMIt->second->data.InspectionStoreId = storeId;
					MEMIt->second->data.ItemId = DataItemIndex[storeId]++;
					MEMIt->second->data.Store_Offset = DataItemOffset[storeId];
					DataItemOffset[storeId] += (DWORD)MEMIt->second->data.ByteSize;

				}
				break;

				}

			}
		}
	}
	void  MLCpyContainer::BuildProtoMessage(SvPml::MesMLCpyContainer& rMesMLCpyCont) const
	{
		for (auto &mlCp : m_MonitorListCpyMap)
		{
			mlCp.second->BuildProtoMessage(*(rMesMLCpyCont.add_monitorlistcpy()));

		}

	}

	void MLCpyContainer::BuildFromProtoMessage(const SvPml::MesMLCpyContainer& rMesMLCpyContainer)
	{
		Clear();
		for (int m = 0; m < rMesMLCpyContainer.monitorlistcpy_size(); m++)
		{
			const SvPml::MesMonitorListCpy& rMlCpy = rMesMLCpyContainer.monitorlistcpy(m);
			MonitorListCpyPointer pMonitorListCpy = std::make_unique<MonitorListCpy>();
			pMonitorListCpy->BuildFromProtoMessage(rMlCpy);
			Insert(pMonitorListCpy);
		}
	}

	bool  MLCpyContainer::QueryListName(const SvPb::QueryListNameRequest&, SvPb::QueryListNameResponse& resp, SvPenv::Error& err) const
	{
		for (auto& mlc : m_MonitorListCpyMap)
		{
			if (mlc.second->GetIsActive())
			{
				*(resp.add_listname()) = mlc.first;
			}
		}
		return true;
	}

	bool MLCpyContainer::QueryListItem(const SvPb::QueryListItemRequest& request, SvPb::QueryListItemResponse& resp, SvPenv::Error& err) const
	{
		
		bool result {true};
		std::string listName = SvUl::to_ansi(request.listname());
		if (0 == listName.size())
		{
			for (const auto& pair : m_MonitorListCpyMap)
			{
				if (pair.second->GetIsActive())
				{
					pair.second->QueryListItem(request, resp);
				}
			}
		}
		else
		{
			MonitorListCpyMap::const_iterator it;
			it = m_MonitorListCpyMap.find(listName);
			if (it != m_MonitorListCpyMap.end() && it->second->GetIsActive())
			{
				it->second->QueryListItem( request, resp);
			}
			else
			{
				SvPenv::Error err;
				err.set_errorcode(SvPenv::ErrorCode::notFound);
				err.set_message("MonitorList with given name does not exist or is not active");
				result = false;
			}

		}
		return result;

	}
	
} //namespace SvSml
