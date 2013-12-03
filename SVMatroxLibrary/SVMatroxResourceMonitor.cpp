// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxResourceMonitor
// * .File Name       : $Workfile:   SVMatroxResourceMonitor.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:15:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxResourceMonitor.h"
#include "SVMatroxErrorEnum.h"

SVMatroxResourceMonitor::SVMatroxResourceMonitor()
: m_CriticalSectionPtr()
, m_Identifiers( SVIdentifierCount )
, m_AllIdentifiers()
{
	ValidateCriticalSection();
}

SVMatroxResourceMonitor& SVMatroxResourceMonitor::Instance()
{
	static SVMatroxResourceMonitor l_Object;
	return l_Object;
}

HRESULT SVMatroxResourceMonitor::InsertIdentifier( SVMatroxIdentifierEnum p_IdentifierType, SVMatroxIdentifier p_Identifier )
{
	HRESULT l_Status = S_OK;

	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();

	SVAutoLock l_AutoLock;

	l_Status = GetAutoLock( l_AutoLock );

	if( l_Status == SVMEE_STATUS_OK )
	{
		SVIdentifierSet::iterator l_AllIter = monitor.m_AllIdentifiers.find( p_Identifier );

		if( l_AllIter == monitor.m_AllIdentifiers.end() )
		{
			monitor.m_AllIdentifiers.insert( p_Identifier );
		}
		else
		{
			l_Status = E_FAIL;
		}

		assert( l_Status == S_OK );

		if( 0 <= p_IdentifierType && p_IdentifierType < static_cast< int >( monitor.m_Identifiers.size() ) )
		{
			SVIdentifierSet& p_rSet = monitor.m_Identifiers[ p_IdentifierType ];
			SVIdentifierSet::iterator l_Iter = p_rSet.find( p_Identifier );

			if( l_Iter == p_rSet.end() )
			{
				p_rSet.insert( p_Identifier );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	assert( l_Status == S_OK );

	return l_Status;
}

HRESULT SVMatroxResourceMonitor::EraseIdentifier( SVMatroxIdentifierEnum p_IdentifierType, SVMatroxIdentifier p_Identifier )
{
	HRESULT l_Status = S_OK;

	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();
	SVAutoLock l_AutoLock;

	l_Status = GetAutoLock( l_AutoLock );

	if( l_Status == SVMEE_STATUS_OK )
	{
		SVIdentifierSet::iterator l_AllIter = monitor.m_AllIdentifiers.find( p_Identifier );

		if( l_AllIter != monitor.m_AllIdentifiers.end() )
		{
			monitor.m_AllIdentifiers.erase( p_Identifier );
		}
		else
		{
			l_Status = E_FAIL;
		}

		assert( l_Status == S_OK );

		if( 0 <= p_IdentifierType && p_IdentifierType < static_cast< int >( monitor.m_Identifiers.size() ) )
		{
			SVIdentifierSet& p_rSet = monitor.m_Identifiers[ p_IdentifierType ];
			SVIdentifierSet::iterator l_Iter = p_rSet.find( p_Identifier );

			if( l_Iter != p_rSet.end() )
			{
				p_rSet.erase( p_Identifier );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	assert( l_Status == S_OK );

	return l_Status;
}

SVMatroxResourceMonitor::SVStatusCode SVMatroxResourceMonitor::GetAutoLock( SVAutoLock& p_rAutoLock )
{
	SVMatroxResourceMonitor& monitor = SVMatroxResourceMonitor::Instance();
	SVStatusCode l_Status = monitor.ValidateCriticalSection();

	if( l_Status == SVMEE_STATUS_OK )
	{
		if( !( p_rAutoLock.Assign( monitor.m_CriticalSectionPtr.get() ) ) )
		{
			l_Status = SVMEE_INVALID_LOCK;
		}
	}

	return l_Status;
}

SVMatroxResourceMonitor::SVStatusCode SVMatroxResourceMonitor::ValidateCriticalSection() const
{
	SVStatusCode l_Status = SVMEE_STATUS_OK;

	if( m_CriticalSectionPtr.empty() )
	{
		m_CriticalSectionPtr = new SVCriticalSection;

		if( m_CriticalSectionPtr.empty() )
		{
			l_Status = SVMEE_INVALID_LOCK;
		}
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxResourceMonitor.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:15:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 15:27:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2011 14:29:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
