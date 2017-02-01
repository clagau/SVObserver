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

#pragma region Includes
#include "SVMonitorListStore.h"
#include "SVSharedMemorySettings.h"
#include "SVProductFilterEnum.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	enum listType { productItems, rejectCondition, failStatus };

	class SVMonitorListWriter
	{
		SVString m_ShareName;
		SVMonitorListStore* m_lists;
		void Init();

	public:
		SVMonitorListWriter();
		~SVMonitorListWriter();

		bool IsCreated() const;
		/////////////////////////////////////////////////////////////////////////////////
		// returns S_OK on success
		/////////////////////////////////////////////////////////////////////////////////
		HRESULT Create(const SVSharedMemorySettings& settings, size_t requiredSize);
		void AddList(const SVString& listName, const SVString& ppqName, int rejectDepth, bool isActive);
		void FillList(const SVString& listName, listType type, const SvSml::MonitorEntryVector& list);
		void SetProductFilter(const SVString& listName, SVProductFilterEnum filter);

		void Release();

		typedef std::shared_ptr< boost::interprocess::managed_shared_memory > managed_shared_memory_shared_ptr;
		managed_shared_memory_shared_ptr shm;
	};

	typedef boost::interprocess::allocator<SVMonitorListStore, segment_manager_t> MonitorListStoreAllocator;
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
