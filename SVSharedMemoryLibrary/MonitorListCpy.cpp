#include "StdAfx.h"
#include "MonitorEntry.h"
#include "MonitorListCpy.h"
#include "SVMonitorListReader.h"
#include "SVMonitorListWriter.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
namespace Seidenader { namespace SVSharedMemoryLibrary
{
	const DWORD MonitorListCpy::ImageBufferHeaderSize = 1024; 

	MonitorListCpy::MonitorListCpy(void)
	{
		m_rejectDepth =0;
		m_IsActive = FALSE;;
		m_ProductFilter = LastInspectedFilter;
	}

	MonitorEntryPointer  MonitorListCpy::AddEntry(ListType::typ ltype, const SVString& name, DWORD type)
	{
		MonitorEntriesMap::iterator MeMIt =  m_EntriesMap.find(name);
		if(MeMIt ==  m_EntriesMap.end())
		{
			MonitorEntryPointer MEP = MonitorEntryPointer(new MonitorEntry(name,type) );
			m_MonitorEntries[ltype].push_back(MEP);
			m_EntriesMap[name] = MEP;
			return MEP;
		}
		else
		{
			assert(MeMIt->second->name == name);
			assert(MeMIt->second->ObjectType == type);
			m_MonitorEntries[ltype].push_back(MeMIt->second);
			return MeMIt->second;
		}
	}


	MonitorListCpy::MonitorListCpy(const SVSharedMonitorList &sharedMonitorList)
	{
		m_MonitorListName = sharedMonitorList.GetName().c_str();
		m_ppqName = sharedMonitorList.GetPPQName().c_str();
		m_rejectDepth = sharedMonitorList.GetRejectDepth();
		m_ProductFilter = sharedMonitorList.GetProductFilter();

		for( int t  =  0 ; t  < ListType::Count; t++)
		{
			ShMoListEntryVector::const_iterator it = sharedMonitorList.GetShMoListEntryVector(ListType::typ(t)).begin();
			ShMoListEntryVector::const_iterator itEnd = sharedMonitorList.GetShMoListEntryVector(ListType::typ(t)).end();
			for(; it !=  itEnd; ++it)
			{
				SVString entryname = it->name.c_str();
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
		for (SvSml::MonitorEntries::const_iterator it = ListEntries.begin();it != ListEntries.end();++it)
		{
			size += 2* sizeof(DWORD);
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

	void MonitorListCpy::SetMonitorlistname(const SVString& listname)
	{
		m_MonitorListName = listname;
	}
	void MonitorListCpy::SetPPQname(const SVString& ppqName)
	{
		m_ppqName = ppqName;
	}
	void MonitorListCpy::SetRejectDepth(int rejectDepth)
	{
		m_rejectDepth = rejectDepth;
	}
	void MonitorListCpy::SetProductFilter(SVProductFilterEnum fEnum)
	{
		m_ProductFilter =fEnum;
	}
	void MonitorListCpy::SetIsActive(bool isactive)
	{
		m_IsActive = isactive;
	}

	const SVString&  MonitorListCpy::GetMonitorlistname() const
	{
		return m_MonitorListName;
	}
	const SVString&  MonitorListCpy::GetPPQname() const
	{
		return m_ppqName;
	}

	int MonitorListCpy::GetRejectDepth() const
	{
		return m_rejectDepth;
	}
	SVProductFilterEnum MonitorListCpy::GetProductFilter() const
	{
		return m_ProductFilter;
	}
	bool MonitorListCpy::GetIsActive() const
	{
		return m_IsActive;
	}
	int  MonitorListCpy::InsertToMLInspectionInfoMap(MLInspectionInfoMap & inspectionInfoMap) const
	{
		int res(0);
		MonitorEntriesMap::const_iterator it; 
		for(it = m_EntriesMap.begin(); it !=m_EntriesMap.end(); ++it)
		{
			assert(it->second.get());
			assert(it->first == it->second->name);

			SVString inspectionName;
			std::size_t pos = it->first.find_first_of('.');
			if(pos != SVString::npos) 
				inspectionName  = it->first.substr(0,pos);
			else 
				inspectionName = it->first;


			if(inspectionInfoMap.find(inspectionName)  == inspectionInfoMap.end())
			{
				res++;
				inspectionInfoMap[inspectionName] = MLInspectionInfoPointer( new MLInspectionInfo) ;
				
			}

			switch(it->second->ObjectType)
			{
			case  SVImageObjectType:
				inspectionInfoMap[inspectionName]->TotalImageSize += (DWORD) it->second->size;
				inspectionInfoMap[inspectionName]->TotalImageSize +=  ImageBufferHeaderSize;
				inspectionInfoMap[inspectionName]->TotalImageCount++; 
				inspectionInfoMap[inspectionName]->StoreIndex = it->second->InspectionStoreId; 
				break;
			
			default:
				inspectionInfoMap[inspectionName]->TotalDataCount++; 
				break;
				//@Todo[MEC][7.50] [22.02.2017] ADD other types

			}

		}
		return res;
	}

	void MonitorListCpy::WriteMonitorList(SVMonitorListWriter& rWriter) const
	{
		if (rWriter.IsCreated())
		{
			rWriter.AddList(m_MonitorListName, m_ppqName, m_rejectDepth, m_IsActive);
			rWriter.SetProductFilter(m_MonitorListName,m_ProductFilter);
			for(int t  =  0; t  < ListType::Count; t++)
			{
				rWriter.FillList(m_MonitorListName, ListType::typ(t) , m_MonitorEntries[t]);
			}

		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ErrorWriteMonitorListNoSharedMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15020 );
		}
	}

}}