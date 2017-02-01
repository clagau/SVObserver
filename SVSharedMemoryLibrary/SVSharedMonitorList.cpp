//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMonitorList
//* .File Name       : $Workfile:   SVSharedMonitorList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Aug 2014 18:36:36  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVSharedMonitorList.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedMonitorList::SVSharedMonitorList(const void_allocator & allocator)
	: m_allocator(allocator)
	, prodItems(allocator)
	, rejctCond(allocator)
	, failStats(allocator)
	, m_name("", allocator)
	, m_ppq("", allocator)
	, m_rejectDepth(0)
	, m_active(false)
	, m_filter(LastInspectedFilter)
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::Constructor");
	}

	SVSharedMonitorList::SVSharedMonitorList(const SVSharedMonitorList & rho)
	: m_allocator(rho.m_allocator)
	, prodItems(rho.prodItems)
	, rejctCond(rho.rejctCond)
	, failStats(rho.failStats)
	, m_name(rho.m_name.c_str(), rho.m_allocator)
	, m_ppq(rho.m_ppq.c_str(), rho.m_allocator)
	, m_rejectDepth(rho.m_rejectDepth)
	, m_active(rho.m_active)
	, m_filter(rho.m_filter)
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::copy Constructor");
	}

	SVSharedMonitorList & SVSharedMonitorList::operator=(const SVSharedMonitorList & rho)
	{
		if (&rho != this)
		{
			SVSharedConfiguration::Log("SVSharedMonitorList::operator=");
			prodItems = rho.prodItems;
			rejctCond = rho.rejctCond;
			failStats = rho.failStats;
			m_name = rho.m_name;
			m_ppq = rho.m_ppq;
			m_rejectDepth = rho.m_rejectDepth;
			m_active = rho.m_active;
			m_filter = rho.m_filter;
		}
		return *this;
	}

	SVSharedMonitorList::~SVSharedMonitorList()
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::Destructor");
	}

	const  ShMoListEntryVector& SVSharedMonitorList::GetProductItems() const
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::GetProductItems");
		return prodItems;
	}

	const ShMoListEntryVector& SVSharedMonitorList::GetRejectCond() const
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::GetRejectCond");
		return rejctCond;
	}

	const ShMoListEntryVector&  SVSharedMonitorList::GetFailStatus() const
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::GetFailStatus");
		return failStats;
	}

	void SVSharedMonitorList::SetProductItems(const std::vector<MonitorEntry> & items)
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::SetProductItems");
		prodItems.clear();
		prodItems.reserve(items.size());
		for (std::vector<MonitorEntry>::const_iterator it = items.begin(); it != items.end(); ++it)
		{
			ShMonitorEntry  SEntry(m_allocator);
			SEntry.name =  bip_string(it->name.c_str(), m_allocator);
			SEntry.size =  it->size;
			SEntry.type =  it->type;
			prodItems.push_back(SEntry);
		}
	}

	void SVSharedMonitorList::SetRejectCond(const std::vector<MonitorEntry> & items)
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::SetRejectCond");
		rejctCond.clear();
		rejctCond.reserve(items.size());
		for (std::vector<MonitorEntry>::const_iterator it = items.begin(); it != items.end(); ++it)
		{
			ShMonitorEntry  SEntry(m_allocator);
			SEntry.name =  bip_string(it->name.c_str(), m_allocator);
			SEntry.size =  it->size;
			SEntry.type =  it->type;
			rejctCond.push_back(SEntry);
		}
	}

	void SVSharedMonitorList::SetFailStatus(const std::vector<MonitorEntry> & items)
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::SetFailStatus");
		failStats.clear();
		failStats.reserve(items.size());
		for (std::vector<MonitorEntry>::const_iterator it = items.begin(); it != items.end(); ++it)
		{
			ShMonitorEntry  SEntry(m_allocator);
			SEntry.name =  bip_string(it->name.c_str(), m_allocator);
			SEntry.size =  it->size;
			SEntry.type =  it->type;
			failStats.push_back(SEntry);
		}
	}


	MonitorEntryVector::const_iterator  SVSharedMonitorList::FindInMoListVector(SVString const &name, const MonitorEntryVector &entryVector )
	{
		MonitorEntryVector::const_iterator ret; 
		for( ret =  entryVector.begin(); ret != entryVector.end(); ++ret )
		{
			if(ret->name == name)
				break;
		}
		return ret;
	}

	bool SVSharedMonitorList::IsInMoListVector(SVString const &name, const MonitorEntryVector &entryVector )
	{
		return FindInMoListVector(name,entryVector ) != entryVector.end();
	}


} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
