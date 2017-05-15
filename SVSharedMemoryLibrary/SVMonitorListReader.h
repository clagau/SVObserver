//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorReader
//* .File Name       : $Workfile:   SVMonitorListReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMonitorListStore.h"
#include "SVSharedMemorySettings.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	class SVMonitorListReader
	{
		bool m_isOpen;
		SVString m_ShareName;
		const SVMonitorListStore* m_store;
		std::shared_ptr<bip::managed_shared_memory> m_pManagedSharedMemory;

	public:
		SVMonitorListReader();
		~SVMonitorListReader();

		bool Open();
		void Close();
		bool IsOpen() const;
		bool HasList(const SVString& name) const;

		const SVSharedMonitorList& operator[](const SVString& listName) const;
		const SVStringVector GetListNames() const;
	};
} //namespace SvSml

