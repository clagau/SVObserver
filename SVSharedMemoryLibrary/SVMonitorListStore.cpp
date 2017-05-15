//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorListStore
//* .File Name       : $Workfile:   SVMonitorListStore.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:28  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVMonitorListStore.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace SvSml
{
	const SVString g_shName = "MonitorListStore.";

	SVMonitorListStore::SVMonitorListStore(const void_allocator& rAllocator)
	: m_allocator(rAllocator)
	, m_lists(std::less<bip_string>(), rAllocator)
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Constructor");
	}

	SVMonitorListStore::~SVMonitorListStore()
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Destructor");
	}

	SVSharedMonitorList & SVMonitorListStore::operator[](const SVString & listName)
	{
		SVSharedConfiguration::Log("SVMonitorListStore::operator[]");

		/* // Do not try to allocate a char_string and use find on the map, it will cause an access violation in 32 bit
		char_string lName(listName.c_str(), m_allocator);
		MonitorMap::iterator it = m_lists.find(lName);
		if (it != m_lists.end())
		{
			return it->second;
		}
		*/
		// iterate over the monitor list names
		for (MonitorMap::iterator it = m_lists.begin();it != m_lists.end();++it)
		{
			const bip_string& key = it->first;
			if (listName == key.c_str())
			{
				return it->second;
			}
		}
		throw std::exception((SVString("Monitor list name: ") + listName + " not found.").c_str());
	}

	const SVSharedMonitorList & SVMonitorListStore::operator[](const SVString & listName) const
	{
		SVSharedConfiguration::Log("SVMonitorListStore::operator[]");
		/* // Do not try to allocate a char_string and use find on the map, it will cause an access violation in 32 bit
		char_string lName(listName.c_str(), m_allocator);
		MonitorMap::const_iterator it = m_lists.find(lName);
		if (it != m_lists.end())
		{
			return it->second;
		}
		*/
		for (MonitorMap::const_iterator it = m_lists.begin();it != m_lists.end();++it)
		{
			const bip_string& key = it->first;
			if (listName == key.c_str())
			{
				return it->second;
			}
		}
		throw std::exception((SVString("Monitor list name: ") + listName + " not found.").c_str());
	}

	const SVStringVector SVMonitorListStore::GetListNames() const
	{
		SVSharedConfiguration::Log("SVMonitorListStore::GetListNames");
		if (!m_lists.empty())
		{
			SVStringVector tmp;
			size_t sz = m_lists.size();
			tmp.reserve(sz);
			for (MonitorMap::const_iterator it = m_lists.begin(); it != m_lists.end(); ++it)
			{
				tmp.push_back(it->first.c_str());
			}
			return tmp;
		}
		throw std::exception("Monitor list empty.");
	}

	void SVMonitorListStore::Add(const SVSharedMonitorList & list)
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Add");
		const bip_string& name = list.GetName();
		MonitorMapValue val(name, list);
		m_lists.insert(val);
	}

	void SVMonitorListStore::Clear()
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Clear");
		m_lists.clear();
	}

	bool SVMonitorListStore::HasList(const SVString & name) const
	{
		SVSharedConfiguration::Log("SVMonitorListStore::HasList");
		// Do Not try to allocate a char_string and use find, it will cause an access violation on 32 bit
		//return m_lists.find(char_string(name.c_str(), m_allocator)) != m_lists.end();
		for (MonitorMap::const_iterator it = m_lists.begin();it != m_lists.end();++it)
		{
			if (name == it->first.c_str())
			{
				return true;
			}
		}
		return false;
	}
} //namespace SvSml
