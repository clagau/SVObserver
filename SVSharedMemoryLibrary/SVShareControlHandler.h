//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShareControlHandler
//* .File Name       : $Workfile:   SVShareControlHandler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   02 Oct 2014 09:04:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVSharedControl.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	class SVShareControlHandler
	{
		typedef std::shared_ptr< boost::interprocess::managed_shared_memory > managed_shared_memory_shared_ptr;
		SVShareControl * m_ctrl;
		const std::string m_shareName;
		managed_shared_memory_shared_ptr shm;
		long m_count;
		long m_filterChangeCount;

	public:
		SVShareControlHandler();
		~SVShareControlHandler();

		void Release();
		bool IsCreated() const;

		// SVObserver is setting/clearing ready flag and clearing ack flag at the same time
		void SetReady();										// for writer use
		void ClearReady();										// for writer use
		bool IsReady() const;									// for reader use
		bool ReadyChanged(long previousValue) const;			// for reader use

		void SetAck();											// for reader use
		bool GotAck() const;									// for writer use
		long GetReadyCount() const;								// used by reader only
		void TickSVOHeartbeat();								// not used (future ?)
		void TickRRSHeartbeat();								// not used (future ?)

		timestamp GetSVOHeartbeat() const;						// not used (future ?)
		timestamp GetRRSHeartbeat() const;						// not used (future ?)

		void SetProductFilterChanged();							// for writer use
		bool ProductFilterChanged(long previousValue) const;	// for reader use
		long GetProductFilterChangeCount() const;				// for reader use
	};
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

