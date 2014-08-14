//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedControl
//* .File Name       : $Workfile:   SVSharedControl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:18  $
//******************************************************************************
#pragma once
#include "SVShared.h"

typedef unsigned long timestamp;

struct SVShareControl
{
	long ready; // zero - means no reader access, non zero is the counter from the writer
	byte ack;
	timestamp svo_heartbeat;
	timestamp rrs_heartbeat;

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