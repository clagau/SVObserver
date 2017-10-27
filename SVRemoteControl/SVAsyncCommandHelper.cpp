//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncCommandHelper
//* .File Name       : $Workfile:   SVAsyncCommandHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:35:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVAsyncCommandHelper.h"
#include "SVAutoLockAndReleaseTemplate.h"
#include "SVWaitFuncs.h"
#pragma endregion Includes

SVAsyncCommandHelper::SVAsyncCommandHelper()
:	m_WaitHandle( nullptr ),
	m_CommandPtr(),
	m_Cancel( false ),
	m_Status(),
	m_CommandActive( 0 )
{
	m_WaitHandle = ::CreateEvent( nullptr, true, true, nullptr );
}

SVAsyncCommandHelper::SVAsyncCommandHelper( const SVAsyncCommandHelper& p_rObject )
:	m_WaitHandle( nullptr ),
	m_CommandPtr( p_rObject.m_CommandPtr )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVAsyncCommandHelper::~SVAsyncCommandHelper()
{
	if( nullptr != m_WaitHandle )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = nullptr;
	}
}

bool SVAsyncCommandHelper::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( nullptr == m_CommandPtr.get() );

	return l_Status;
}

bool SVAsyncCommandHelper::GetCancel() const
{
	return m_Cancel;
}

void SVAsyncCommandHelper::SetCancel( bool p_Cancel )
{
	m_Cancel = p_Cancel;
}

void SVAsyncCommandHelper::SetCommandActive()
{
	::InterlockedExchange( &m_CommandActive, 1 );
}

void SVAsyncCommandHelper::SetCommandComplete()
{
	::InterlockedExchange( &m_CommandActive, 0 );
}

HRESULT SVAsyncCommandHelper::NotifyRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
	{
		::SetEvent( m_WaitHandle );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVAsyncCommandHelper::WaitForRequest( DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle && nullptr != m_CommandPtr.get() )
	{
		unsigned long l_WaitStatus = SVWaitFuncs::WaitForSingleObject( m_WaitHandle, p_TimeoutInMilliseconds );
		
		if( WAIT_OBJECT_0 == l_WaitStatus )
		{
			ClearCommand();
		}
		else if( WAIT_TIMEOUT == l_WaitStatus )
		{
			if( m_Cancel )
			{
				CancelCommand();
			}
			l_Status = RPC_E_TIMEOUT;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = RPC_E_CALL_COMPLETE;
	}
	return l_Status;
}

bool SVAsyncCommandHelper::IsCommandEmpty() const
{
	return nullptr == m_CommandPtr.get();
}

HRESULT SVAsyncCommandHelper::GetCommand( SVCommandTemplatePtr& p_rCommand, DWORD p_TimeoutInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		if( nullptr != m_CommandPtr.get() )
		{
			p_rCommand = m_CommandPtr;
		}
		else
		{
			l_Status = STG_E_INUSE;
		}
	}
	else
	{
		l_Status = RPC_E_TIMEOUT;
	}

	return l_Status;
}

HRESULT SVAsyncCommandHelper::SetCommand( const SVCommandTemplatePtr& p_rCommand, DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		if( ( 0 == m_CommandActive ) && ( nullptr == m_CommandPtr.get() ) )
		{
			if( ::ResetEvent( m_WaitHandle ) )
			{
				m_CommandPtr = p_rCommand;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = STG_E_INUSE;
		}
	}
	else
	{
		l_Status = RPC_E_TIMEOUT;
	}

	return l_Status;
}

HRESULT SVAsyncCommandHelper::ClearCommand( DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		m_CommandPtr.reset();
	}
	else
	{
		l_Status = RPC_E_TIMEOUT;
	}

	return l_Status;
}

SVCommandStatus SVAsyncCommandHelper::GetStatus() const
{
	SVCommandStatus l_Status = m_Status;

	m_Status.clear();

	return l_Status;
}

void SVAsyncCommandHelper::SetStatus( const SVCommandStatus& p_Status )
{
	m_Status = p_Status;
}

HRESULT SVAsyncCommandHelper::CancelCommand( DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		if( nullptr != m_CommandPtr.get() )
		{
			m_CommandPtr->Cancel();

			m_CommandPtr.reset();
		}
	}
	else
	{
		l_Status = RPC_E_TIMEOUT;
	}

	return l_Status;
}

HRESULT SVAsyncCommandHelper::GetWaitHandle( HANDLE& p_rHandle ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_rHandle )
	{
		::CloseHandle( p_rHandle );

		p_rHandle = nullptr;
	}

	if( ! ::DuplicateHandle( GetCurrentProcess(), m_WaitHandle, GetCurrentProcess(), &p_rHandle, 0, FALSE, DUPLICATE_SAME_ACCESS ) )
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}
