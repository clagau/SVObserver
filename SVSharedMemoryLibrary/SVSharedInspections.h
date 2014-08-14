//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspection
//* .File Name       : $Workfile:   SVSharedInspections.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:42  $
//******************************************************************************
#pragma once

#include "SVShared.h"

struct SVSharedInspection
{
	char_string m_ShareName;
	long m_Index;

	void_allocator m_Allocator;

	SVSharedInspection(const char* inspectionShareName, long index, const void_allocator& rAlloc);
	SVSharedInspection(const void_allocator& rAlloc);
	SVSharedInspection(const SVSharedInspection& rData);

	const SVSharedInspection& operator=(const SVSharedInspection& rData);
};

typedef boost::interprocess::allocator<SVSharedInspection, segment_manager_t> SVSharedInspectionAllocator;
// The first item in the pair is the Inspection Share Name
typedef std::pair<char_string, SVSharedInspection> SVSharedInspectionPair; // non const key for flt map
typedef boost::interprocess::allocator<SVSharedInspectionPair, segment_manager_t> SVSharedInspectionPairAllocator;
// The Key is the Inspection Share Name
typedef boost::interprocess::flat_map<char_string, SVSharedInspection, std::less<char_string>, SVSharedInspectionPairAllocator> SVSharedInspectionMap;
typedef boost::interprocess::allocator<SVSharedInspectionMap, segment_manager_t> SVSharedInspectionMapAllocator;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedInspections.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
