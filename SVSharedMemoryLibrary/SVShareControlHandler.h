//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShareControlHandler
//* .File Name       : $Workfile:   SVShareControlHandler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   02 Sep 2014 15:27:58  $
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

		bool IsCreated() const;
		// SVObserver is setting/clearing ready flag and clearing ack flag at the same time
		void SetReady() { SVSharedConfiguration::Log("ControlHandler::SetReady"); m_ctrl->ready = ++m_count; m_ctrl->ack = 0; }
		void ClearReady(){ SVSharedConfiguration::Log("ControlHandler::ClearReady"); m_ctrl->ready = 0; m_ctrl->ack = 0; }
		bool IsReady() const { SVSharedConfiguration::Log("ControlHandler::IsReady"); return (IsCreated() && m_ctrl->ready) ? true : false; }
		bool ReadyChanged(long previousValue) const { SVSharedConfiguration::Log("ControlHandler::ReadyChanged"); return m_ctrl->ready != previousValue; }

		void SetAck(){ SVSharedConfiguration::Log("ControlHandler::SetAck"); m_ctrl->ack = 1; }
		bool GotAck() const { SVSharedConfiguration::Log("ControlHandler::GotAck"); return m_ctrl->ack ? true : false; }
		long GetCount() const { return m_count; }
		void TickSVOHeartbeat() { m_ctrl->svo_heartbeat = GetTickCount(); }
		void TickRRSHeartbeat() { m_ctrl->rrs_heartbeat = GetTickCount(); }

		timestamp GetSVOHeartbeat() const { return m_ctrl->svo_heartbeat; }
		timestamp GetRRSHeartbeat() const { return m_ctrl->rrs_heartbeat; }

		void SetProductFilterChanged() { SVSharedConfiguration::Log("ControlHandler::SetProductFilterChanged"); m_ctrl->productFilterChanged = ++m_filterChangeCount; }
		bool ProductFilterChanged(long previousValue) const { SVSharedConfiguration::Log("ControlHandler::ProductFilterChanged"); return m_ctrl->productFilterChanged != previousValue; }
		long GetFilterChangeCount() const { return m_filterChangeCount; }

		void Release();
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVShareControlHandler.h_v  $
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