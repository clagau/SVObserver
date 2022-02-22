//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MLCpyContainer.cpp
/// All Rights Reserved 
//*****************************************************************************
///class encapsulate an container with all active monitorlists 
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "MLcpyContainer.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVProtoBuf/Envelope.h"
#pragma endregion Includes

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
	
	int MLCpyContainer::GetInspectionStoreId(const std::string& InspectionName) const
	{
		auto it = m_InspectionInfoMap.find(InspectionName);
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
		DWORD Storeindex(0);
		MLInspectionInfoMap::iterator it;
		for (Storeindex = 0, it = m_InspectionInfoMap.begin(); it != m_InspectionInfoMap.end(); ++it, ++Storeindex)
		{
			it->second->StoreIndex = Storeindex;
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

				MEMIt->second->data.m_inspectionStoreId = m_InspectionInfoMap[inspectionName]->StoreIndex;
			}
		}
	}

	void MLCpyContainer::setDataTrcPos(const std::string& rPPQName, int inspectionStoreId, int inspectionTrcPos, const std::unordered_map<uint32_t, int>& rDataDefMap, const std::unordered_map<uint32_t, int>& rImageMap, const std::unordered_map<uint32_t, int>& rChildImageMap)
	{
		MonitorListCpyMap::iterator  MLCPyIt;
		for (auto& rMLCPy : m_MonitorListCpyMap)
		{
			if (nullptr == rMLCPy.second || rMLCPy.second->GetPPQname() != rPPQName)
			{
				continue;
			}

			for (auto& rEntry : rMLCPy.second->m_EntriesMap)
			{
				if (nullptr == rEntry.second || rEntry.second->data.m_inspectionStoreId != inspectionStoreId)
				{
					continue;
				}
				uint32_t objectId = rEntry.second->m_objectId;
				rEntry.second->data.m_inspectionTRCPos = inspectionTrcPos;
				auto iter = rDataDefMap.find(objectId);
				if (rDataDefMap.end() != iter)
				{
					rEntry.second->data.m_triggerRecordPos = iter->second;
					continue;
				}

				auto iterImage = rImageMap.find(objectId);
				if (rImageMap.end() != iterImage)
				{
					rEntry.second->data.m_triggerRecordPos = iterImage->second;
					continue;
				}

				auto iterChildImage = rChildImageMap.find(objectId);
				if (rChildImageMap.end() != iterChildImage)
				{
					rEntry.second->data.m_triggerRecordPos = iterChildImage->second| MonitorEntryData::c_childFlagForTrPos;
					continue;
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

	bool  MLCpyContainer::QueryListName(const SvPb::QueryListNameRequest&, SvPb::QueryListNameResponse& resp, SvPenv::Error&) const
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
				err.set_errorcode(SvPenv::ErrorCode::notFound);
				err.set_message("MonitorList with given name does not exist or is not active");
				result = false;
			}

		}
		return result;

	}

	bool MLCpyContainer::clearInspectionIdsVector(const std::string& rPPQName)
	{
		for (auto& element : m_MonitorListCpyMap)
		{
			if (nullptr != element.second && element.second->m_ppqName == rPPQName)
			{
				element.second->clearInspectionIdsVector();
				return true;
			}
		}
		return false;
	}

	bool MLCpyContainer::addInspectionIdEntry(const std::string& rPPQName, int ipMLId, int ipTRCId)
	{
		for (auto& element : m_MonitorListCpyMap)
		{
			if (nullptr != element.second && element.second->m_ppqName == rPPQName)
			{
				element.second->addInspectionIdEntry(ipMLId, ipTRCId);
				return true;
			}
		}
		return false;
	}
	
} //namespace SvSml
