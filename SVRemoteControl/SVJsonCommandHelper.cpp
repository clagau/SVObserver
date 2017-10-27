//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandHelper
//* .File Name       : $Workfile:   SVJsonCommandHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Oct 2014 10:10:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVJsonCommandHelper.h"

#include "SVAutoLockAndReleaseTemplate.h"
#include "SVWaitFuncs.h"
#pragma endregion Includes

SVJsonCommandHelper::SVJsonCommandHelper()
:	m_WaitHandle( nullptr )
, m_JsonCommand( "" )
{
	m_WaitHandle = ::CreateEvent( nullptr, true, true, nullptr );
}

SVJsonCommandHelper::SVJsonCommandHelper( const std::string& p_rJsonCommand )
:	m_WaitHandle( nullptr )
, m_JsonCommand( p_rJsonCommand )
{
	m_WaitHandle = ::CreateEvent( nullptr, true, true, nullptr );
}

SVJsonCommandHelper::SVJsonCommandHelper( const SVJsonCommandHelper& p_rObject )
:	m_WaitHandle( nullptr )
, m_JsonCommand( p_rObject.m_JsonCommand )
, m_JsonResults( p_rObject.m_JsonResults )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVJsonCommandHelper::~SVJsonCommandHelper()
{
	if( nullptr != m_WaitHandle )
	{
		::CloseHandle( m_WaitHandle );
		m_WaitHandle = nullptr;
	}
}

bool SVJsonCommandHelper::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_JsonCommand.length() == 0 );
	l_Status = l_Status && m_JsonResults.empty();

	return l_Status;
}

void SVJsonCommandHelper::clear()
{
	m_JsonCommand.clear();
	m_JsonResults.clear();

	NotifyRequestComplete();
}

HRESULT SVJsonCommandHelper::NotifyRequestComplete() const
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

inline HRESULT HrFromWaitResult(DWORD wres)
{
	if(wres == WAIT_TIMEOUT)
	{
		return RPC_E_TIMEOUT;
	}
	return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT SVJsonCommandHelper::WaitForRequest( DWORD p_TimeoutInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
	{
		DWORD result = SVWaitFuncs::WaitForSingleObject( m_WaitHandle, p_TimeoutInMilliseconds );
		if( WAIT_OBJECT_0 != result )
		{
			l_Status = HrFromWaitResult(result);
		}
	}
	else
	{
		l_Status = STATUS_INVALID_HANDLE;
	}
	return l_Status;
}

bool SVJsonCommandHelper::IsJsonCommandEmpty() const
{
	return m_JsonCommand.empty();
}

HRESULT SVJsonCommandHelper::GetJsonCommand( std::string& p_rJsonCommand, DWORD p_TimeoutInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		if( !( m_JsonCommand.empty() ) )
		{
			p_rJsonCommand = m_JsonCommand;
		}
		else
		{
			l_Status = STG_E_INUSE;
		}
	}
	else
	{
		l_Status = STG_E_INUSE;
	}

	return l_Status;
}

HRESULT SVJsonCommandHelper::SetJsonCommand( const std::string& p_rJsonCommand, DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		if( m_JsonCommand.empty() )
		{
			if( ::ResetEvent( m_WaitHandle ) )
			{
				m_JsonCommand = p_rJsonCommand;
			}
			else
			{
				l_Status = HRESULT_FROM_WIN32(GetLastError());
			}
		}
		else
		{
			l_Status = STG_E_INUSE;
		}
	}
	else
	{
		l_Status = STG_E_INUSE;
	}

	return l_Status;
}

HRESULT SVJsonCommandHelper::ClearJsonCommand( DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection, p_TimeoutInMilliseconds ) )
	{
		m_JsonCommand.clear();
		m_JsonResults.clear();

		l_Status = NotifyRequestComplete();
	}
	else
	{
		l_Status = STG_E_INUSE;
	}

	return l_Status;
}

const Json::Value& SVJsonCommandHelper::GetJsonResults() const
{
	if (!m_JsonCommand.empty())
	{
		return m_JsonResults;
	}
	//throw ?
	return m_JsonResults;
}

HRESULT SVJsonCommandHelper::SetJsonResults( const Json::Value& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( !( m_JsonCommand.empty() ) )
	{
		m_JsonResults = p_rJsonResults;
	}
	else
	{
		l_Status = STG_E_INUSE;
	}

	return l_Status;
}

HRESULT SVJsonCommandHelper::GetWaitHandle( HANDLE& p_rHandle ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_rHandle )
	{
		::CloseHandle( p_rHandle );

		p_rHandle = nullptr;
	}

	if( ! ::DuplicateHandle( GetCurrentProcess(), m_WaitHandle, GetCurrentProcess(), &p_rHandle, 0, FALSE, DUPLICATE_SAME_ACCESS ) )
	{
		l_Status = HRESULT_FROM_WIN32(GetLastError());
	}

	return l_Status;
}

HRESULT SVJsonCommandHelper::ExtractStatus(const std::string& tag) const
{
	HRESULT hr = S_OK; // E_INVALIDARG; Status is Optional ?
	const Json::Value& status = m_JsonResults[tag.c_str()];
	if (status.isNumeric())
	{
		hr = status.asInt();
	}
	return hr;
}
