//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspectionWriter
//* .File Name       : $Workfile:   SVSharedInspectionWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:44  $
//******************************************************************************
#pragma once

//Moved to precompiled header: #include <memory>
#include "SVSharedLastInspectedCache.h"
#include "SVSharedRejectCache.h"
#include "SVSharedMemorySettings.h"

namespace SeidenaderVision
{
	class SVSharedInspectionWriter // one writer per inspection
	{
	public:
		SVSharedInspectionWriter();
		~SVSharedInspectionWriter();

		HRESULT Create(const std::string& name, const GUID& guid, const SVSharedMemorySettings& rSettings);
		void Destroy();

		const std::string& GetShareName() const;
		const GUID& GetGuid() const;

		const SVSharedData & GetLastInspectedSlot(long idx) const;
		SVSharedData & GetLastInspectedSlot(long idx);

		SVSharedData & GetRejectSlot(long idx);

		void ReleaseAll();

		HRESULT CopyLastInspectedToReject(long index, long rejectIndex);

	private:
		typedef std::shared_ptr<boost::interprocess::managed_shared_memory> managed_shared_memory_shared_ptr;

		void Init();
		void RemovePreviousImageFiles();

		void ClearHeldLastInspected();
		void ClearHeldRejects();

		managed_shared_memory_shared_ptr m_pManagedSharedMemory;

		std::string m_ShareName;
		SVSharedLastInspectedCache* m_pSharedLastInspectedCache;
		SVSharedRejectCache* m_pSharedRejectCache;

		GUID m_guid;
	};
}

