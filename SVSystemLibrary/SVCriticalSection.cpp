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

