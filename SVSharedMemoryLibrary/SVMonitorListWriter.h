//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorListWriter
//* .File Name       : $Workfile:   SVMonitorListWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Aug 2014 18:41:54  $
//******************************************************************************
#pragma once
#include "SVMonitorListStore.h"
#include "SVSharedMemorySettings.h"
#include "SVProductFilterEnum.h"

namespace SeidenaderVision
{
	enum listType { productItems, rejectCondition, failStatus };

	class SVMonitorListWriter
	{
		std::string m_ShareName;
		SVMonitorListStore * m_lists;
		void Init();

	public:
		SVMonitorListWriter();
		~SVMonitorListWriter();

		bool IsCreated() const;
		HRESULT Create(const SVSharedMemorySettings & settings);
		void AddList(const std::string & listName, const std::string & ppqName, int rejectDepth);
		void FillList(const std::string & listName, listType type, const std::vector<std::string> & list);
		void SetProductFilter(const std::string & listName, SVProductFilterEnum filter);

		void Release();

		typedef std::shared_ptr< boost::interprocess::managed_shared_memory > managed_shared_memory_shared_ptr;
		managed_shared_memory_shared_ptr shm;
	};

	typedef boost::interprocess::allocator<SVMonitorListStore, segment_manager_t> MonitorListStoreAllocator;
}

