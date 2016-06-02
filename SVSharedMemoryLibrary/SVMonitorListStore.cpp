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

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	const std::string g_shName = "MonitorListStore.";

	SVMonitorListStore::SVMonitorListStore(const void_allocator & allocator)
	: m_allocator(allocator)
	, m_lists(std::less<char_string>(), allocator)
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Constructor");
	}

	SVMonitorListStore::~SVMonitorListStore()
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Destructor");
	}

	SVSharedMonitorList & SVMonitorListStore::operator[](const std::string & listName)
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
			if (listName == it->first.c_str())
			{
				return it->second;
			}
		}
		throw std::exception((std::string("Monitor list name: ") + listName + " not found.").c_str());
	}

	const SVSharedMonitorList & SVMonitorListStore::operator[](const std::string & listName) const
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
			if (listName == it->first.c_str())
			{
				return it->second;
			}
		}
		throw std::exception((std::string("Monitor list name: ") + listName + " not found.").c_str());
	}

	const std::vector<std::string> SVMonitorListStore::GetListNames() const
	{
		SVSharedConfiguration::Log("SVMonitorListStore::GetListNames");
		if (!m_lists.empty())
		{
			std::vector<std::string> tmp;
			size_t sz = m_lists.size();
			tmp.reserve(sz);
			for (MonitorMap::const_iterator it = m_lists.begin(); it != m_lists.end(); ++it)
			{
				tmp.push_back(it->first.c_str());
			}
			return tmp;
		}
		else
		{
			throw std::exception("Monitor list empty.");
		}
	}

	void SVMonitorListStore::Add(const SVSharedMonitorList & list)
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Add");
		MonitorMapValue val(list.GetName(), list);
		m_lists.insert(val);
	}

	void SVMonitorListStore::Clear()
	{
		SVSharedConfiguration::Log("SVMonitorListStore::Clear");
		m_lists.clear();
	}

	bool SVMonitorListStore::HasList(const std::string & name) const
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

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
