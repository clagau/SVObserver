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
#include "SVSharedControl.h"
#include "SVSharedConfiguration.h"

namespace SeidenaderVision
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
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVShareControlHandler.h_v  $
 * 
 *    Rev 1.4   02 Oct 2014 09:04:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  954
 * SCR Title:  Fix Issues with Run/Reject Server and Shared Memory Synchronization
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved code into SVShareControlHandler.cpp file.
 * Renamed GetCount to GetReadyCount.
 * Renamed GetFilterChangeCount to GetProductFilterChangeCount.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Sep 2014 15:27:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added GetFilterChangeCount method.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Aug 2014 17:33:00   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added GetCount method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Aug 2014 18:39:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added variable and accessor methods for filterChanged. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:07:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/