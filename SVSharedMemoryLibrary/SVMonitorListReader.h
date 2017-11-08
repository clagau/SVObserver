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
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvSml
{
	class SVMonitorListReader
	{
		bool m_isOpen;
		std::string m_ShareName;
		const SVMonitorListStore* m_store;
		std::shared_ptr<bip::managed_shared_memory> m_pManagedSharedMemory;
	public:
		SVMonitorListReader();
		~SVMonitorListReader();

		bool Open();
		void Close();
		bool IsOpen() const;
		bool HasList(const std::string& name) const;

		const SVSharedMonitorList& operator[](const std::string& listName) const;
		const SvDef::StringVector GetListNames() const;
	};
} //namespace SvSml

