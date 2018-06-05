#include "StdAfx.h"
#include "MonitorEntry.h"
#include "MonitorListCpy.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#pragma warning (push ,2)
#include "SVPRotobuf\MonitorListStore.pb.h"
#pragma warning(pop)
namespace SvSml
{
	const DWORD MonitorListCpy::ImageBufferHeaderSize = 1024;

	MonitorListCpy::MonitorListCpy(void)
	{
		m_ProductDepth = 0;
		m_rejectDepth = 0;
		m_IsActive = FALSE;;
		m_ProductFilter = LastInspectedFilter;
	}

	MonitorEntryPointer  MonitorListCpy::AddMultEntries(DWORD ListFlag, const std::string& name)
	{
		MonitorEntriesMap::iterator MeMIt = m_EntriesMap.find(name);
		if (MeMIt == m_EntriesMap.end())
		{
			MonitorEntryPointer MEP = MonitorEntryPointer(new MonitorEntry(name));

			for (int t = 0; t < ListType::Count; t++)
			{
				if (ListFlag & ListFlags[t])
				{
					m_MonitorEntries[t].push_back(MEP);
				}
			}

			m_EntriesMap[name] = MEP;
			return MEP;
		}
		else
		{
			assert(MeMIt->second->name == name);
			return MeMIt->second;
		}
	}
	
	MonitorEntryPointer  MonitorListCpy::AddEntry(ListType::typ ltype, const std::string& name)
	{
		MonitorEntriesMap::iterator MeMIt = m_EntriesMap.find(name);
		if (MeMIt == m_EntriesMap.end())
		{
			MonitorEntryPointer MEP = MonitorEntryPointer(new MonitorEntry(name));
			if (ltype < ListType::Count)
			{
				MEP->data.m_MonitorListFlag = ListFlags[ltype];
			}
			m_MonitorEntries[ltype].push_back(MEP);
			m_EntriesMap[name] = MEP;
			return MEP;
		}
		else
		{
			assert(MeMIt->second->name == name);
			if (ltype < ListType::Count)
			{
				MeMIt->second->data.m_MonitorListFlag |= ListFlags[ltype];
			}
			m_MonitorEntries[ltype].push_back(MeMIt->second);
			return MeMIt->second;
		}
	}

	static size_t CalcMemory(const SvSml::MonitorEntries &ListEntries)
	{
		size_t size(0);
		size_t MonitorEntryDataSize = sizeof(MonitorEntryData);
		for (SvSml::MonitorEntries::const_iterator it = ListEntries.begin(); it != ListEntries.end(); ++it)
		{
			size += MonitorEntryDataSize;
			size += it->get()->name.length();
		}
		return size;
	}

	size_t  MonitorListCpy::CalcMemorySize() const
	{
		size_t size(0);
		size = m_MonitorListName.length() + m_ppqName.length() + 2 * sizeof(DWORD) + sizeof(bool);
		for (int list = 0; list < ListType::Count; list++)
		{
			size += CalcMemory(m_MonitorEntries[list]);
		}

		return size;
	}


	const MonitorEntries& MonitorListCpy::GetMonitorEntries(ListType::typ type) const
	{
		if (type >= ListType::Count)
			throw std::exception("Wrong ListType");
		return m_MonitorEntries[type];

	}

	void MonitorListCpy::ClearAll()
	{
		for (int list = 0; list < SvSml::ListType::Count; list++)
		{
			m_MonitorEntries[list].clear();
		}

		m_EntriesMap.clear();

	}

	void MonitorListCpy::SetMonitorlistname(const std::string& listname)
	{
		m_MonitorListName = listname;
	}
	void MonitorListCpy::SetPPQname(const std::string& ppqName)
	{
		m_ppqName = ppqName;
	}
	void MonitorListCpy::SetRejectDepth(int rejectDepth)
	{
		m_rejectDepth = rejectDepth;
	}
	void MonitorListCpy::SetProductDepth(int ProductDepth)
	{
		m_ProductDepth = ProductDepth;
	}
	void MonitorListCpy::SetProductFilter(SVProductFilterEnum fEnum)
	{
		m_ProductFilter = fEnum;
	}
	void MonitorListCpy::SetIsActive(bool isactive)
	{
		m_IsActive = isactive;
	}

	const std::string&  MonitorListCpy::GetMonitorlistname() const
	{
		return m_MonitorListName;
	}
	const std::string&  MonitorListCpy::GetPPQname() const
	{
		return m_ppqName;
	}

	int MonitorListCpy::GetRejectDepth() const
	{
		return m_rejectDepth;
	}
	int MonitorListCpy::GetProductDepth() const
	{
		return m_ProductDepth;
	}
	SVProductFilterEnum MonitorListCpy::GetProductFilter() const
	{
		return m_ProductFilter;
	}
	bool MonitorListCpy::GetIsActive() const
	{
		return m_IsActive;
	}
	int  MonitorListCpy::InsertToMLInspectionInfoMap(MLInspectionInfoMap & inspectionInfoMap, MLPPQInfoMap& PPQInfoMap) const
	{
		int res(0);
		DWORD PPQIndex = (DWORD)PPQInfoMap.size();
		if (PPQInfoMap.find(m_ppqName) == PPQInfoMap.end())
		{
			MLPPQInfoPointer& rMLPPQInfoP = PPQInfoMap[m_ppqName]
				= MLPPQInfoPointer(new MLPPQInfo(m_ProductDepth, m_rejectDepth));
			rMLPPQInfoP->SlotManagerIndex = PPQIndex;

		}
		else
		{
			PPQInfoMap[m_ppqName]->NumRejectSlot = m_rejectDepth;
			PPQInfoMap[m_ppqName]->NumLastSlot = m_ProductDepth;
			PPQIndex = PPQInfoMap[m_ppqName]->SlotManagerIndex;
		}

		MonitorEntriesMap::const_iterator it;
		for (it = m_EntriesMap.begin(); it != m_EntriesMap.end(); ++it)
		{
			assert(it->second.get());
			assert(it->first == it->second->name);

			std::string inspectionName{it->first};
			std::string Inspections{SvDef::FqnInspections};
			Inspections += _T('.');
			if(0 == inspectionName.find(Inspections))
			{
				inspectionName = inspectionName.substr(Inspections.size(), inspectionName.size() - Inspections.size());
			}

			std::size_t pos = inspectionName.find_first_of('.');
			if (pos != std::string::npos)
			{
				inspectionName = inspectionName.substr(0, pos);
			}

			if (inspectionInfoMap.find(inspectionName) == inspectionInfoMap.end())
			{
				res++;
				auto& newEntry = inspectionInfoMap[inspectionName] = MLInspectionInfoPointer(new MLInspectionInfo);
				newEntry->PPQName = m_ppqName;
				newEntry->PPQIndex = PPQIndex;
			}
			auto& newEntry = inspectionInfoMap[inspectionName];


			switch (it->second->data.ObjectType)
			{
			case  SvDef::SVImageObjectType:
				newEntry->TotalImageSize += (DWORD)it->second->data.ByteSize;
				newEntry->TotalImageSize += ImageBufferHeaderSize;
				newEntry->TotalImageCount++;
				newEntry->StoreIndex = it->second->data.InspectionStoreId;
				break;

			default:
				newEntry->TotalDataSize += (DWORD)it->second->data.ByteSize;
				newEntry->TotalDataCount++;
				newEntry->StoreIndex = it->second->data.InspectionStoreId;
				break;

			}

		}
		return res;
	}

	bool MonitorListCpy::BuildProtoMessage(SvPml::MesMonitorListCpy& rMessage) const
	{
		rMessage.set_monitorlistname(m_MonitorListName.c_str());
		rMessage.set_ppqname(m_ppqName.c_str());
		rMessage.set_isactive(m_IsActive);
		rMessage.set_productdepth(m_ProductDepth);
		rMessage.set_rejectdepth(m_rejectDepth);

		for (auto &MonEntry : m_EntriesMap)
		{
			auto pMonEntryMessage = rMessage.add_monitorentries();
			MonEntry.second->BuildProtoMessage(*pMonEntryMessage);
		}
		return true;
	}
	void MonitorListCpy::BuildFromProtoMessage(const SvPml::MesMonitorListCpy& rMessage)
	{
		m_MonitorListName = rMessage.monitorlistname();
		m_ppqName = rMessage.ppqname();
		m_IsActive = rMessage.isactive();
		m_ProductDepth = rMessage.productdepth();
		m_rejectDepth = rMessage.rejectdepth();
		for (int m = 0; m < rMessage.monitorentries_size(); m++)
		{
			const SvPml::MesMonitorEntry& mEntry = rMessage.monitorentries(m);
			std::string name = mEntry.name();
			DWORD monitorlistflag = mEntry.entrydata().monitorlistflag();
			MonitorEntryPointer pMonitorEntry = AddMultEntries(monitorlistflag, name);
			pMonitorEntry->BuildFromProtoMessage(mEntry);
		}

	}

	void MonitorListCpy::QueryListItem(const SvPb::QueryListItemRequest& request, SvPb::QueryListItemResponse& resp) const
	{
		if (!m_IsActive)
		{
			return;
		}
		const MonitorEntries* pMonitorEntries{ nullptr };
		const MonitorEntries*  pMonitorEntriesImages{ nullptr };
		switch (request.type())
		{
		case SvPb::ProductItem:
			pMonitorEntries =  &(m_MonitorEntries[ListType::productItemsData]);
			pMonitorEntriesImages = &(m_MonitorEntries[ListType::productItemsImage]);
			break;
		case SvPb::RejectCondition:
			pMonitorEntries = &(m_MonitorEntries[ListType::rejectCondition]);
			break;

		case SvPb::FailStatus:
			pMonitorEntries = &(m_MonitorEntries[ListType::failStatus]);
			break;
		}
		if (nullptr !=  pMonitorEntries  && request.queryvalues())
		{
				for (const auto&  MoEntyPtr : *pMonitorEntries)
				{
					MoEntyPtr->AddListItem(resp);
				}
		}
		if (nullptr != pMonitorEntriesImages  && request.queryimages())
		{
			for (const auto& MoEntyPtr : *pMonitorEntriesImages)
			{
				MoEntyPtr->AddListItem(resp);
			}
		}
		
		if(nullptr == pMonitorEntries)
		{
			for (const auto& Pair : m_EntriesMap)
			{
				if (Pair.second->IsImage() && request.queryimages())
				{
					Pair.second->AddListItem(resp);
				} 
				else if (!Pair.second->IsImage() && request.queryvalues())
				{
					Pair.second->AddListItem(resp);
				}

			}

		}
	}
} //namespace SvSml
