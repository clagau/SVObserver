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
#include "SVMonitorListStore.h"
#include "SVSharedMemorySettings.h"

namespace SeidenaderVision
{
	class SVMonitorListReader
	{
		typedef std::shared_ptr<boost::interprocess::managed_shared_memory> DataSharedMemPtr;
		bool m_isOpen;
		std::string m_ShareName;
		const SVMonitorListStore * m_store;
		DataSharedMemPtr shm;

	public:
		SVMonitorListReader();
		~SVMonitorListReader();

		bool Open();
		void Close();
		bool IsOpen() const;
		bool HasList(const std::string & name) const;

		const SVSharedMonitorList & operator[](const std::string & listName) const;
		const std::vector<std::string> GetListNames() const;
	};
}

