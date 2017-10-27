//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCriticalSection
//* .File Name       : $Workfile:   SVCriticalSection.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:37:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVCriticalSection.h"
#pragma endregion Includes

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

	if( INFINITE == p_TimeOutMilliseconds )
	{
		l_Status = EnterLock();
	}
	else
	{
		__int64 l_Frequency = 0;
		__int64 l_Start = 0;

		::QueryPerformanceFrequency( reinterpret_cast< LARGE_INTEGER* >( &l_Frequency ) );
		::QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER* >( &l_Start ) ); 

		do
		{
			l_Status = TryEnterLock();

			if( !l_Status )
			{
				__int64 l_Current = 0;

				::QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER* >( &l_Current ) ); 

				unsigned long l_Diff = static_cast< unsigned long >( ( l_Current - l_Start ) * 1000 / l_Frequency );

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
		m_IsLockCreated = ( FALSE != ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) );
	}

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		l_Status = ::TryEnterCriticalSection( &m_Lock );
	}

	return l_Status;
}
