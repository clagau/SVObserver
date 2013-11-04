//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCriticalSection
//* .File Name       : $Workfile:   SVCriticalSection.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 11:25:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVCriticalSection.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"

SVCriticalSection::SVCriticalSection( DWORD p_SpinCount )
: m_SpinCount( p_SpinCount ), m_IsLockCreated( false ), m_Lock()
{
	m_IsLockCreated = ( ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) != FALSE );
}

SVCriticalSection::~SVCriticalSection()
{
	if( m_IsLockCreated )
	{
		m_IsLockCreated = false;

		::DeleteCriticalSection( &m_Lock );
	}
}

BOOL SVCriticalSection::Lock( DWORD p_TimeOutMilliseconds ) const
{
	BOOL l_Status = true;

	if( p_TimeOutMilliseconds == INFINITE )
	{
		l_Status = EnterLock();
	}
	else
	{
		SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

		do
		{
			l_Status = TryEnterLock();

			if( !l_Status )
			{
				SVClock::SVTimeStamp l_Current = SVClock::GetTimeStamp(); 

				DWORD l_Diff = static_cast< DWORD >( SVClock::ConvertTo( SVClock::Milliseconds, ( l_Current - l_Start ) ) );

				if( l_Diff < p_TimeOutMilliseconds )
				{
					::Sleep( 1 );
				}
			}
		}
		while( !l_Status );
	}

	return l_Status;
}

BOOL SVCriticalSection::Unlock() const
{
	BOOL l_Status = true;

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		::LeaveCriticalSection( &m_Lock );
	}

	return l_Status;
}

BOOL SVCriticalSection::EnterLock() const
{
	BOOL l_Status = true;

	if( !m_IsLockCreated )
	{
		m_IsLockCreated = ( ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) != FALSE );
	}

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		::EnterCriticalSection( &m_Lock );
	}

	return l_Status;
}

BOOL SVCriticalSection::TryEnterLock() const
{
	BOOL l_Status = true;

	if( !m_IsLockCreated )
	{
		m_IsLockCreated = ( ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) != FALSE );
	}

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		l_Status = ::TryEnterCriticalSection( &m_Lock );
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVCriticalSection.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 11:25:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 May 2013 13:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:51:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 13:47:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:49:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Feb 2011 12:17:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Critical Section object and fixed bug in Auto Lock and Release object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
