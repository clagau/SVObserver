//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMonitorList
//* .File Name       : $Workfile:   SVSharedMonitorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:18  $
//******************************************************************************
#pragma once

#include "SVShared.h"

namespace SeidenaderVision
{
	// typedef for std/boost containers to be used with shared memory and corresponding allocators
	// for details see boost::interprocess and/or std library documentation
	typedef boost::interprocess::allocator<char_string, segment_manager_t> StringAllocator;
	typedef boost::interprocess::vector<char_string, StringAllocator> StringList;
	typedef boost::interprocess::allocator<StringList, segment_manager_t> StringListAllocator;

	class SVSharedMonitorList
	{
		StringList  prodItems;
		StringList  rejctCond;
		StringList  failStats;

		char_string m_name; // list name as registered
		char_string m_ppq; // ppq name
		int m_rejectDepth;

		bool m_active; // set when activated

	public:
		SVSharedMonitorList(const void_allocator & allocator);
		SVSharedMonitorList(const SVSharedMonitorList & rho);
		~SVSharedMonitorList();
		SVSharedMonitorList & operator=(const SVSharedMonitorList & rho);
		bool IsActivated() const { return m_active; }
		// for reader's use
		const StringList & GetProductItems() const;
		const StringList & GetRejectCond() const;
		const StringList & GetFailStatus() const;

		const char_string & GetName() const { return m_name; }
		const char_string & GetPPQName() const { return m_ppq; }
		int GetRejectDepth() const { return m_rejectDepth; }

		void SetProductItems(const std::vector<std::string> & items);
		void SetRejectCond(const std::vector<std::string> & items);
		void SetFailStatus(const std::vector<std::string> & items);

		void_allocator m_allocator;

		void Activate() { m_active = true; }
		void Deactivate() { m_active = false; }
		// for writer's use

		void SetNames(const std::string & list, const std::string & ppq)
		{
			m_name = char_string(list.c_str(), m_allocator);
			m_ppq = char_string(ppq.c_str(), m_allocator);
		}
		void SetRejectDepth(int depth) { m_rejectDepth = depth; }
	};

	typedef boost::interprocess::allocator<SVSharedMonitorList, segment_manager_t> MonitorListAllocator;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedMonitorList.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:09:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/