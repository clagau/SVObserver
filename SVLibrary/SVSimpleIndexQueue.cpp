//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSimpleIndexQueue
//* .File Name       : $Workfile:   SVSimpleIndexQueue.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:51:24  $
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
	return m_Used.size();
}

long SVSimpleIndexQueue::get_available_count() const
{
	return m_Available.size();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVSimpleIndexQueue.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:51:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 13:31:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:08:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

