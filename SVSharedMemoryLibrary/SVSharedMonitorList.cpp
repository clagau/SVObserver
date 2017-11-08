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
#include "SVMonitorListWriter.h"
#pragma endregion Includes

namespace SvSml
{
	SVSharedMonitorList::SVSharedMonitorList(const void_allocator & allocator)
		: m_allocator(allocator)
		, prodItemsData(allocator)
		, prodItemsImage(allocator)
		, rejctCond(allocator)
		, failStats(allocator)
		, m_name("", allocator)
		, m_ppq("", allocator)
		, m_rejectDepth(0)
		, m_ProductDepth(0)
		, m_active(false)
		, m_filter(LastInspectedFilter)
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::Constructor");
	}

	SVSharedMonitorList::SVSharedMonitorList(const SVSharedMonitorList & rho)
		: m_allocator(rho.m_allocator)
		, prodItemsData(rho.prodItemsData)
		, prodItemsImage(rho.prodItemsImage)
		, rejctCond(rho.rejctCond)
		, failStats(rho.failStats)
		, m_name(rho.m_name.c_str(), rho.m_allocator)
		, m_ppq(rho.m_ppq.c_str(), rho.m_allocator)
		, m_rejectDepth(rho.m_rejectDepth)
		, m_ProductDepth(rho.m_ProductDepth)
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
			prodItemsData = rho.prodItemsData;
			prodItemsImage = rho.prodItemsImage;
			rejctCond = rho.rejctCond;
			failStats = rho.failStats;
			m_name = rho.m_name;
			m_ppq = rho.m_ppq;
			m_rejectDepth = rho.m_rejectDepth;
			m_ProductDepth = rho.m_ProductDepth;
			m_active = rho.m_active;
			m_filter = rho.m_filter;
		}
		return *this;
	}

	SVSharedMonitorList::~SVSharedMonitorList()
	{
		SVSharedConfiguration::Log("SVSharedMonitorList::Destructor");
	}


	const  ShMoListEntryVector& SVSharedMonitorList::GetShMoListEntryVector(ListType::typ ltype) const
	{
		switch (ltype)
		{
		case ListType::productItemsData:
			return prodItemsData;
			break;
		case ListType::productItemsImage:
			return prodItemsImage;
			break;
		case ListType::failStatus:
			return failStats;
			break;
		case ListType::rejectCondition:
			return rejctCond;
			break;
		default:
			throw std::exception("wrong listtype");

		}

	}



	void SVSharedMonitorList::SetEntries(ListType::typ ltype, const MonitorEntries & items)
	{

		ShMoListEntryVector*  pEntryVector = nullptr;
		switch (ltype)
		{
		case ListType::failStatus:
			pEntryVector = &failStats;
			break;
		case ListType::productItemsImage:
			pEntryVector = &prodItemsImage;
			break;
		case ListType::productItemsData:
			pEntryVector = &prodItemsData;
			break;
		case ListType::rejectCondition:
			pEntryVector = &rejctCond;
			break;
		}
		if (pEntryVector)
		{
			pEntryVector->clear();
			pEntryVector->reserve(items.size());
			for (MonitorEntries::const_iterator it = items.begin(); it != items.end(); ++it)
			{
				ShMonitorEntry  SEntry(m_allocator, *(it->get()));
				pEntryVector->push_back(SEntry);
			}
		}
	}


	MonitorEntries::const_iterator  SVSharedMonitorList::FindInMoListVector(std::string const &name, const MonitorEntries &entryVector)
	{
		MonitorEntries::const_iterator ret;
		for (ret = entryVector.begin(); ret != entryVector.end(); ++ret)
		{
			if (ret->get()->name == name)
				break;
		}
		return ret;
	}

	bool SVSharedMonitorList::IsInMoListVector(std::string const &name, const MonitorEntries &entryVector)
	{
		return FindInMoListVector(name, entryVector) != entryVector.end();
	}
} //namespace SvSml
