//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMonitorList
//* .File Name       : $Workfile:   SVSharedMonitorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Aug 2014 18:36:36  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVShared.h"
#include "SVProductFilterEnum.h"
#include "MonitorEntry.h"
#pragma endregion Includes


namespace Seidenader { namespace SVSharedMemoryLibrary
{

	class SVSharedMonitorList
	{
	public:
		SVSharedMonitorList(const void_allocator & allocator);
		SVSharedMonitorList(const SVSharedMonitorList & rho);
		~SVSharedMonitorList();
		SVSharedMonitorList & operator=(const SVSharedMonitorList & rho);
		bool IsActivated() const { return m_active; }
		// for reader's use
		const ShMoListEntryVector & GetProductItems() const;
		const ShMoListEntryVector & GetRejectCond() const;
		const ShMoListEntryVector & GetFailStatus() const;

		const bip_string & GetName() const { return m_name; }
		const bip_string & GetPPQName() const { return m_ppq; }
		int GetRejectDepth() const { return m_rejectDepth; }
		SVProductFilterEnum GetProductFilter() const { return m_filter; }

		void SetProductItems(const MonitorEntryVector &items);
		void SetRejectCond(const MonitorEntryVector &items);
		void SetFailStatus(const MonitorEntryVector &items);

		void_allocator m_allocator;

		void Activate() { m_active = true; }
		void Deactivate() { m_active = false; }
		// for writer's use

		void SetNames(const SVString & list, const SVString & ppq)
		{
			m_name = bip_string(list.c_str(), m_allocator);
			m_ppq = bip_string(ppq.c_str(), m_allocator);
		}
		void SetRejectDepth(int depth) { m_rejectDepth = depth; }
		void SetProductFilter(SVProductFilterEnum filter) { m_filter = filter; }
	

		static MonitorEntryVector::const_iterator  FindInMoListVector(SVString const &name, const MonitorEntryVector &entryVector );
		static bool IsInMoListVector(SVString const &name, const MonitorEntryVector &entryVector );
		

	private:	
		ShMoListEntryVector  prodItems;
		ShMoListEntryVector  rejctCond;
		ShMoListEntryVector  failStats;

		bip_string m_name; // list name as registered
		bip_string m_ppq; // ppq name
		int m_rejectDepth;

		SVProductFilterEnum m_filter;
		bool m_active; // set when activated

	};

	typedef bip::allocator<SVSharedMonitorList, segment_manager_t> MonitorListAllocator;
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

