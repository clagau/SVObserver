//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedControl
//* .File Name       : $Workfile:   SVSharedControl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   21 Aug 2014 12:09:28  $
//******************************************************************************
#pragma once
#include "SVShared.h"

typedef unsigned long timestamp;

struct SVShareControl
{
	long ready; // zero - means no reader access, non zero is the counter from the writer
	timestamp svo_heartbeat;
	timestamp rrs_heartbeat;
	byte ack;

	void_allocator allocator;
	SVShareControl(const void_allocator& rAllocator)
	: allocator(rAllocator)
	, ready(0)
	, ack(0)
	, svo_heartbeat(0)
	, rrs_heartbeat(0)
	{}
};

typedef boost::interprocess::allocator<SVShareControl, segment_manager_t> SVShareControlAllocator;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedControl.h_v  $
 * 
 *    Rev 1.1   21 Aug 2014 12:09:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to move the m_ack member variable to the end of the structure to reduce padding
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:07:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/