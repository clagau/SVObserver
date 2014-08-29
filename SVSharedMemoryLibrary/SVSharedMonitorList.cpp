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
#include "StdAfx.h"
#include "SVSharedMonitorList.h"
#include "SVSharedConfiguration.h"

using namespace SeidenaderVision;

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

SeidenaderVision::SVSharedMonitorList::SVSharedMonitorList(const SVSharedMonitorList & rho)
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

SeidenaderVision::SVSharedMonitorList & SeidenaderVision::SVSharedMonitorList::operator=(const SVSharedMonitorList & rho)
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

const StringList & SeidenaderVision::SVSharedMonitorList::GetProductItems() const
{
	SVSharedConfiguration::Log("SVSharedMonitorList::GetProductItems");
	return prodItems;
}

const StringList & SVSharedMonitorList::GetRejectCond() const
{
	SVSharedConfiguration::Log("SVSharedMonitorList::GetRejectCond");
	return rejctCond;
}

const StringList & SVSharedMonitorList::GetFailStatus() const
{
	SVSharedConfiguration::Log("SVSharedMonitorList::GetFailStatus");
	return failStats;
}

void SVSharedMonitorList::SetProductItems(const std::vector<std::string> & items)
{
	SVSharedConfiguration::Log("SVSharedMonitorList::SetProductItems");
	prodItems.clear();
	prodItems.reserve(items.size());
	for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		prodItems.push_back(char_string(it->c_str(), m_allocator));
	}
}

void SVSharedMonitorList::SetRejectCond(const std::vector<std::string> & items)
{
	SVSharedConfiguration::Log("SVSharedMonitorList::SetRejectCond");
	rejctCond.clear();
	rejctCond.reserve(items.size());
	for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		rejctCond.push_back(char_string(it->c_str(), m_allocator));
	}
}

void SVSharedMonitorList::SetFailStatus(const std::vector<std::string> & items)
{
	SVSharedConfiguration::Log("SVSharedMonitorList::SetFailStatus");
	failStats.clear();
	failStats.reserve(items.size());
	for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		failStats.push_back(char_string(it->c_str(), m_allocator));
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedMonitorList.cpp_v  $
 * 
 *    Rev 1.1   28 Aug 2014 18:36:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SVProductFilterEnum m_filter member variable 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:09:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/