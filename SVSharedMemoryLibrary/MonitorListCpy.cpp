#include "StdAfx.h"
#include "MonitorEntry.h"
#include "MonitorListCpy.h"
#include "SVMonitorListReader.h"
#include "SVMonitorListWriter.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "ObjectInterfaces\ErrorNumbers.h"
namespace Seidenader { namespace SVSharedMemoryLibrary
{
	MonitorListCpy::MonitorListCpy(void)
	{
		m_rejectDepth =0;
		m_IsActive = FALSE;;
		m_ProductFilter = LastInspectedFilter;
	}

	MonitorListCpy::MonitorListCpy(const SVSharedMonitorList &sharedMonitorList)
	{
		m_name = sharedMonitorList.GetName().c_str();
		m_ppq = sharedMonitorList.GetPPQName().c_str();
		m_rejectDepth = sharedMonitorList.GetRejectDepth();
		m_ProductFilter = sharedMonitorList.GetProductFilter();
		MonitorListCpyHelper::Assign2MoListEntryVector(prodItems,sharedMonitorList.GetProductItems());
		MonitorListCpyHelper::Assign2MoListEntryVector(failStats,sharedMonitorList.GetFailStatus());
		MonitorListCpyHelper::Assign2MoListEntryVector(rejctCond,sharedMonitorList.GetRejectCond());
		m_IsActive = sharedMonitorList.IsActivated();
	}

	static size_t CalcMemory(SvSml::MonitorEntryVector &ListEntries)
	 {
		 size_t size(0);
		 for (SvSml::MonitorEntryVector::const_iterator it = ListEntries.begin();it != ListEntries.end();++it)
		 {
			 size += 2* sizeof(DWORD);
			 size += it->name.length();
		 }
		 return size;
	 }

	size_t  MonitorListCpy::CalcMemorySize()
	 {
		 size_t size(0);
		 size = m_name.length() + m_ppq.length() +   2 * sizeof(DWORD) +  sizeof(bool);
		 size += CalcMemory(prodItems);
		 size += CalcMemory(rejctCond);
		 size += CalcMemory(failStats);
		 return size;
	 }
	 
	void MonitorListCpyHelper::ReloadMonitorMap(SVMonitorListReader& mlReader, MonitorListCpyMap& monitorMap)
	{
		mlReader.Open();
		monitorMap.clear();
		std::vector<SVString> mlNames = mlReader.GetListNames();
		std::vector<SVString>::const_iterator it;

		for(it = mlNames.begin(); it != mlNames.end(); it++)
		{
			SvSml::MonitorListCpy mcp(mlReader[*it]);
			monitorMap[*it] = mcp;
		}
		mlReader.Close();
	}

	void MonitorListCpyHelper::WriteMonitorList(SVMonitorListWriter& rWriter,  const MonitorListCpy& rmonitorList)
	{
		if (rWriter.IsCreated())
		{
			rWriter.AddList(rmonitorList.m_name, rmonitorList.m_ppq, rmonitorList.m_rejectDepth, rmonitorList.m_IsActive);
			rWriter.SetProductFilter(rmonitorList.m_name,rmonitorList.m_ProductFilter);
			rWriter.FillList(rmonitorList.m_name, SvSml::productItems, rmonitorList.prodItems);
			rWriter.FillList(rmonitorList.m_name, SvSml::rejectCondition, rmonitorList.rejctCond);
			rWriter.FillList(rmonitorList.m_name, SvSml::failStatus, rmonitorList.failStats);
		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ErrorWriteMonitorListNoSharedMemory, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15020 );
		}
	}

void  MonitorListCpyHelper::Assign2MoListEntryVector (MonitorEntryVector &cpy, const ShMoListEntryVector  &sev)
	{
		cpy.clear();
		cpy.reserve(sev.size());
		std::for_each(sev.begin(), sev.end(),
			[&cpy](const ShMonitorEntry &se)
		{
			SvSml::MonitorEntry e;
			e.name =  se.name.c_str();
			e.size = se.size;
			e.type = se.type;
			cpy.push_back(e);
		}
		);
	}
}}