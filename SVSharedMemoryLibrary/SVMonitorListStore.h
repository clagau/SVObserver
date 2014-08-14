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

#include "SVSharedMonitorList.h"
#include "SVSharedMemorySettings.h"

namespace SeidenaderVision
{
	// typedef for std/boost containers to be used with shared memory and corresponding allocators
	// for details see boost::interprocess and/or std library documentation
	
	typedef std::pair<char_string, SVSharedMonitorList> MonitorMapValue;
	typedef boost::interprocess::allocator<MonitorMapValue, segment_manager_t> MonitorMapValueAllocator;
	typedef boost::interprocess::flat_map<char_string, SVSharedMonitorList, std::less< char_string >, MonitorMapValueAllocator> MonitorMap;
	typedef boost::interprocess::allocator<MonitorMap, segment_manager_t> MonitorMapAllocator;

	class SVMonitorListStore
	{
		MonitorMap m_lists; 

	public:
		SVMonitorListStore(const void_allocator & allocator);
		~SVMonitorListStore();
		SVSharedMonitorList & operator[](const std::string & listName);
		const SVSharedMonitorList & operator[](const std::string & listName) const;

		const std::vector<std::string> GetListNames() const;
		bool Open(boost::interprocess::managed_shared_memory & share);
		void Clear();
		bool HasList(const std::string & name) const;
		void Add(const SVSharedMonitorList & list);

		void_allocator m_allocator;
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVMonitorListStore.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:07:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/