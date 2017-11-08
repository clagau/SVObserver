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

#pragma endregion Includes

namespace SvSml
{
	class SVMonitorListStore;
	class SVMonitorListWriter
	{
		std::string m_ShareName;
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
		void AddList(const std::string& listName, const std::string& ppqName, int rejectDepth, int productDepth, bool isActive);
		void FillList(const std::string& listName, ListType::typ type, const SvSml::MonitorEntries& list);
		void SetProductFilter(const std::string& listName, SVProductFilterEnum filter);
		void Release(); //<Destroy sharedMemory
		std::shared_ptr<bip::managed_shared_memory> m_pManagedSharedMemory;
	};
	typedef bip::allocator<SVMonitorListStore, segment_manager_t> MonitorListStoreAllocator;

} //namespace SvSml
