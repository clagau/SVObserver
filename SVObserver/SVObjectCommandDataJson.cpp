//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectCommandDataJson
//* .File Name       : $Workfile:   SVObjectCommandDataJson.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectCommandDataJson.h"

SVObjectCommandDataJson::SVObjectCommandDataJson()
: m_WaitHandle( nullptr )
, m_JsonCommand( _T( "" ) )
, m_JsonResults( _T( "" ) )
{
	m_WaitHandle = ::CreateEvent( nullptr, true, false, nullptr );
}

SVObjectCommandDataJson::SVObjectCommandDataJson( const SVObjectCommandDataJson& p_rObject )
: m_WaitHandle( nullptr )
, m_JsonCommand( p_rObject.m_JsonCommand )
, m_JsonResults( p_rObject.m_JsonResults )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVObjectCommandDataJson::~SVObjectCommandDataJson()
{
	if( nullptr != m_WaitHandle )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = nullptr;
	}
}

bool SVObjectCommandDataJson::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_JsonCommand.length() == 0 );
	l_Status = l_Status && ( m_JsonResults.length() == 0 );

	return l_Status;
}

void SVObjectCommandDataJson::clear()
{
	m_JsonCommand.clear();
	m_JsonResults.clear();
}

HRESULT SVObjectCommandDataJson::NotifyRequestComplete() const
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

HRESULT SVObjectCommandDataJson::WaitForRequest( DWORD p_TimeoutInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
	{
		if( ::WaitForSingleObject( m_WaitHandle, p_TimeoutInMilliseconds ) != WAIT_OBJECT_0 )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

const std::string& SVObjectCommandDataJson::GetJsonCommand() const
{
	return m_JsonCommand;
}

HRESULT SVObjectCommandDataJson::SetJsonCommand( const std::string& p_rCommand )
{
	HRESULT l_Status = S_OK;

	m_JsonCommand = p_rCommand;

	if( nullptr != m_WaitHandle )
	{
		if( !( ::ResetEvent( m_WaitHandle ) ) )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectCommandDataJson::GetWaitHandle( HANDLE& p_rHandle ) const
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

