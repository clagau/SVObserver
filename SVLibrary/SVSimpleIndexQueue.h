//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSimpleIndexQueue
//* .File Name       : $Workfile:   SVSimpleIndexQueue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:51:30  $
//******************************************************************************
#ifndef SVSIMPLEINDEXQUEUE_H_
#define SVSIMPLEINDEXQUEUE_H_
#pragma once

#include <deque>
#include <vector>
#include "SVUtilityLibrary/SVUtilityGlobals.h"

class SVSimpleIndexQueue
{
public:
	SVSimpleIndexQueue();

	HRESULT reset( const std::vector<long>& vecIndexes );	// specify which indexes are available
	HRESULT reset( long lNumIndexes );	// initialized to [0, lNumIndexes)
	HRESULT clear();

	bool push(long& rlIndex);  // index is an [out] param; return value is success
	bool pop(long& rlIndex);   // index is an [out] param; return value is success
	bool is_used( long lIndex ) const;
	bool is_empty() const;

	void get_used( std::vector<long>& rlUsedList ) const;
	bool get_next_available_index( long& rlNextAvailable ) const;	// returns false if none available

	long get_used_count() const;
	long get_available_count() const;

private:
	std::deque<long> m_Available;
	std::deque<long> m_Used;
};

#endif

