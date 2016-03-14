//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSimpleIndexQueue
//* .File Name       : $Workfile:   SVSimpleIndexQueue.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:02:44  $
//******************************************************************************

#include "stdafx.h"
#include <iterator> // for std::inserter (VS2010)
#include "SVSimpleIndexQueue.h"

SVSimpleIndexQueue::SVSimpleIndexQueue()
{
}

// specify which indexes are available
HRESULT SVSimpleIndexQueue::reset( const std::vector<long>& vecIndexes )
{
	m_Available.clear();
	m_Used.clear();
	std::copy( vecIndexes.begin(), vecIndexes.end(), std::inserter( m_Available, m_Available.begin() ) );
	return S_OK;
}

// initialized to [0, lNumIndexes)
HRESULT SVSimpleIndexQueue::reset( long lNumIndexes )
{
	m_Available.clear();
	m_Used.clear();
	for ( long l=0; l < lNumIndexes; ++l )
	{
		m_Available.push_back( l );
	}
	return S_OK;
}

HRESULT SVSimpleIndexQueue::clear()
{
	long lDummy;
	while ( pop(lDummy) )
		{}
	return S_OK;
}

// index is an [out] param; return value is success
bool SVSimpleIndexQueue::push(long& rlIndex)
{
	if ( m_Available.size() == 0 )
		return false;
	rlIndex = m_Available.front();
	m_Used.push_back( rlIndex );
	m_Available.pop_front();
	return true;
}

// index is an [out] param; return value is success
bool SVSimpleIndexQueue::pop(long& rlIndex)
{
	if ( m_Used.size() == 0 )
		return false;
	rlIndex = m_Used.front();
	m_Available.push_back( rlIndex );
	m_Used.pop_front();
	return true;
}

bool SVSimpleIndexQueue::is_used( long lIndex ) const
{
	std::deque<long>::const_iterator iter;
	for ( iter = m_Used.begin(); iter != m_Used.end(); ++iter );
	{
		if ( *iter == lIndex )
			return true;
	}
	return false;
}

bool SVSimpleIndexQueue::is_empty() const
{
	return m_Used.size() == 0;
}

void SVSimpleIndexQueue::get_used( std::vector<long>& rlUsedList ) const
{
	rlUsedList.resize( m_Used.size() );
	std::copy( m_Used.begin(), m_Used.end(), rlUsedList.begin() );
}

bool SVSimpleIndexQueue::get_next_available_index( long& rlNextAvailable ) const	// returns false if none available
{
	if ( m_Available.size() == 0 )
		return false;

	rlNextAvailable = m_Available.front();
	return true;
}

long SVSimpleIndexQueue::get_used_count() const
{
	return static_cast<long>(m_Used.size());
}

long SVSimpleIndexQueue::get_available_count() const
{
	return static_cast<long>(m_Available.size());
}

