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


SVCriticalSection::SVCriticalSection( DWORD p_SpinCount )
: m_SpinCount( p_SpinCount ), m_IsLockCreated( false ), m_Lock()
{
	m_IsLockCreated = ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) ? true : false;
}

SVCriticalSection::~SVCriticalSection()
{
	if( m_IsLockCreated )
	{
		m_IsLockCreated = false;

		::DeleteCriticalSection( &m_Lock );
	}
}

bool SVCriticalSection::Lock( DWORD p_TimeOutMilliseconds ) const
{
	bool l_Status = true;

	if( p_TimeOutMilliseconds == INFINITE )
	{
		l_Status = EnterLock();
	}
	else
	{
		DWORD Start = GetTickCount();

		do
		{
			l_Status = TryEnterLock();

			if( !l_Status )
			{
				DWORD Diff = GetTickCount() - Start;

				if( Diff < p_TimeOutMilliseconds )
				{
					::Sleep( 1 );
				}
			}
		}
		while( !l_Status );
	}

	return l_Status;
}

bool SVCriticalSection::Unlock() const
{
	bool l_Status = true;

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		::LeaveCriticalSection( &m_Lock );
	}

	return l_Status;
}

bool SVCriticalSection::EnterLock() const
{
	bool l_Status = true;

	if( !m_IsLockCreated )
	{
		m_IsLockCreated = ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) ? true : false;
	}

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		::EnterCriticalSection( &m_Lock );
	}

	return l_Status;
}

bool SVCriticalSection::TryEnterLock() const
{
	bool l_Status = true;

	if( !m_IsLockCreated )
	{
		m_IsLockCreated = ::InitializeCriticalSectionAndSpinCount( &m_Lock, m_SpinCount ) ? true : false;
	}

	l_Status = m_IsLockCreated;

	if( l_Status )
	{
		l_Status = ::TryEnterCriticalSection(&m_Lock) ? true : false;
	}

	return l_Status;
}

