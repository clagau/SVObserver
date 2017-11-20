#include "StdAfx.h"
#include "MonitorEntry.h"
#include "MonitorListCpy.h"
#include "SVMonitorListReader.h"
#include "SVMonitorListWriter.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
namespace SvSml
{
	const DWORD MonitorListCpy::ImageBufferHeaderSize = 1024; 

	MonitorListCpy::MonitorListCpy(void)
	{
		m_ProductDepth = 0;
		m_rejectDepth =0;
		m_IsActive = FALSE;;
		m_ProductFilter = LastInspectedFilter;
		
	}

	MonitorEntryPointer  MonitorListCpy::AddEntry(ListType::typ ltype, const std::string& name)
	{
		MonitorEntriesMap::iterator MeMIt =  m_EntriesMap.find(name);
		if(MeMIt ==  m_EntriesMap.end())
		{
			MonitorEntryPointer MEP = MonitorEntryPointer(new MonitorEntry(name) );
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


	MonitorListCpy::MonitorListCpy(const SVSharedMonitorList &sharedMonitorList)
	{
		m_MonitorListName = sharedMonitorList.GetName().c_str();
		m_ppqName = sharedMonitorList.GetPPQName().c_str();
		m_rejectDepth = sharedMonitorList.GetRejectDepth();
		m_ProductDepth = sharedMonitorList.GetProductDepth();
		m_ProductFilter = sharedMonitorList.GetProductFilter();

		for( int t  =  0 ; t  < ListType::Count; t++)
		{
			ShMoListEntryVector::const_iterator it = sharedMonitorList.GetShMoListEntryVector(ListType::typ(t)).begin();
			ShMoListEntryVector::const_iterator itEnd = sharedMonitorList.GetShMoListEntryVector(ListType::typ(t)).end();
			for(; it !=  itEnd; ++it)
			{
				std::string entryname = it->name.c_str();
				MonitorEntriesMap::iterator MeMIt =  m_EntriesMap.find(entryname);
				if(MeMIt ==  m_EntriesMap.end())
				{
					MonitorEntryPointer MEP = MonitorEntryPointer(new MonitorEntry(*it) );
					m_MonitorEntries[t].push_back(MEP);
					m_EntriesMap[entryname] = MEP;
				}
				else
				{
					m_MonitorEntries[t].push_back(MeMIt->second);
				}
			}
		}
		m_IsActive = sharedMonitorList.IsActivated();
	}

	static size_t CalcMemory(const SvSml::MonitorEntries &ListEntries)
	{
		size_t size(0);
		size_t MonitorEntryDataSize  = sizeof(MonitorEntryData);
		for (SvSml::MonitorEntries::const_iterator it = ListEntries.begin();it != ListEntries.end();++it)
		{
			size += MonitorEntryDataSize;
			size += it->get()->name.length();
		}
		return size;
	}

	size_t  MonitorListCpy::CalcMemorySize() const
	{
		size_t size(0);
		size = m_MonitorListName.length() + m_ppqName.length() +   2 * sizeof(DWORD) +  sizeof(bool);
		for(int list  =  0; list  < ListType::Count; list++)
		{
			size += CalcMemory(m_MonitorEntries[list]);
		}

		return size;
	}


	const MonitorEntries& MonitorListCpy::GetMonitorEntries(ListType::typ type) const
	{
		if(type >= ListType::Count)
			throw std::exception("Wrong ListType");
		return m_MonitorEntries[type];

	}

	void MonitorListCpy::ClearAll()
	{
		for(int list  =  0; list  < SvSml::ListType::Count; list++)
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
		m_ProductFilter =fEnum;
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
			MLPPQInfoPointer& rMLPPQInfoP =  PPQInfoMap[m_ppqName] 
				= MLPPQInfoPointer(new MLPPQInfo(m_ProductDepth, m_rejectDepth));
			rMLPPQInfoP->SlotManagerIndex = PPQIndex;
			
		}
		else
		{
			PPQIndex = PPQInfoMap[m_ppqName]->SlotManagerIndex;
		}
		
		MonitorEntriesMap::const_iterator it; 
		for(it = m_EntriesMap.begin(); it !=m_EntriesMap.end(); ++it)
		{
			assert(it->second.get());
			assert(it->first == it->second->name);

			std::string inspectionName;
			std::size_t pos = it->first.find_first_of('.');
			if(pos != std::string::npos) 
				inspectionName  = it->first.substr(0,pos);
			else 
				inspectionName = it->first;
			
			if(inspectionInfoMap.find(inspectionName)  == inspectionInfoMap.end())
			{
				res++;
				auto& newEntry =  inspectionInfoMap[inspectionName] = MLInspectionInfoPointer( new MLInspectionInfo) ;
				newEntry->PPQName = m_ppqName;
				newEntry->PPQIndex = PPQIndex;
			}
			auto& newEntry = inspectionInfoMap[inspectionName];

			
			switch(it->second->data.ObjectType)
			{
			case  SvDef::SVImageObjectType:
				newEntry->TotalImageSize += (DWORD)it->second->data.ByteSize;
				newEntry->TotalImageSize +=  ImageBufferHeaderSize;
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

	void MonitorListCpy::WriteMonitorList(SVMonitorListWriter& rWriter) const
	{
		if (rWriter.IsCreated())
		{
			rWriter.AddList(m_MonitorListName, m_ppqName, m_rejectDepth,m_ProductDepth,m_IsActive);
			rWriter.SetProductFilter(m_MonitorListName,m_ProductFilter);
			for(int t  =  0; t  < ListType::Count; t++)
			{
				rWriter.FillList(m_MonitorListName, ListType::typ(t) , m_MonitorEntries[t]);
			}

		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorWriteMonitorListNoSharedMemory, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15020 );
		}
	}
} //namespace SvSml
