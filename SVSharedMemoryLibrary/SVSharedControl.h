//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedControl
//* .File Name       : $Workfile:   SVSharedControl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   28 Aug 2014 18:37:56  $
//******************************************************************************
#pragma once
#include "SVShared.h"

typedef unsigned long timestamp;

struct SVShareControl
{
	long ready; // zero - means no reader access, non zero is the counter from the writer
	long productFilterChanged;
	timestamp svo_heartbeat;
	timestamp rrs_heartbeat;
	byte ack;

	void_allocator allocator;
	SVShareControl(const void_allocator& rAllocator)
	: allocator(rAllocator)
	, ready(0)
	, productFilterChanged(0)
	, ack(0)
	, svo_heartbeat(0)
	, rrs_heartbeat(0)
	{}
};

typedef boost::interprocess::allocator<SVShareControl, segment_manager_t> SVShareControlAllocator;

