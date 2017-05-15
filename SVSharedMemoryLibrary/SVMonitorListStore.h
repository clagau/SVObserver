//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorListStore
//* .File Name       : $Workfile:   SVMonitorListStore.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVSharedMonitorList.h"
#include "SVSharedMemorySettings.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	// typedef for std/boost containers to be used with shared memory and corresponding allocators
	// for details see boost::interprocess and/or std library documentation
	
	typedef std::pair<const bip_string, SVSharedMonitorList> MonitorMapValue;
	typedef bip::allocator<MonitorMapValue, segment_manager_t> MonitorMapValueAllocator;
	typedef bip::map<bip_string, SVSharedMonitorList, std::less< bip_string >, MonitorMapValueAllocator> MonitorMap;
	typedef bip::allocator<MonitorMap, segment_manager_t> MonitorMapAllocator;

	class SVMonitorListStore
	{
		MonitorMap m_lists; 

	public:
		SVMonitorListStore(const void_allocator & allocator);
		~SVMonitorListStore();
		SVSharedMonitorList & operator[](const SVString & listName);
		const SVSharedMonitorList & operator[](const SVString & listName) const;

		const SVStringVector GetListNames() const;
		bool Open(bip::managed_shared_memory & share);
		void Clear();
		bool HasList(const SVString & name) const;
		void Add(const SVSharedMonitorList & list);

		void_allocator m_allocator;
	};
} //namespace SvSml

