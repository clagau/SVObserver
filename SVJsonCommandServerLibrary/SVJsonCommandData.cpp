//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandData
//* .File Name       : $Workfile:   SVJsonCommandData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2014 10:28:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVJsonCommandData.h"

SVJsonCommandData::SVJsonCommandData()
: m_WaitHandle( NULL )
, m_JsonCommand( _T( "" ) )
, m_JsonResults( _T( "" ) )
{
	m_WaitHandle = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

SVJsonCommandData::SVJsonCommandData( const SVJsonCommandData& p_rObject )
: m_WaitHandle( NULL )
, m_JsonCommand( p_rObject.m_JsonCommand )
, m_JsonResults( p_rObject.m_JsonResults )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVJsonCommandData::~SVJsonCommandData()
{
	if( m_WaitHandle != NULL )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = NULL;
	}
}

bool SVJsonCommandData::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_JsonCommand.length() == 0 );
	l_Status = l_Status && ( m_JsonResults.length() == 0 );

	return l_Status;
}

void SVJsonCommandData::clear()
{
	m_JsonCommand.clear();
	m_JsonResults.clear();
}

HRESULT SVJsonCommandData::NotifyRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( m_WaitHandle != NULL )
	{
		::SetEvent( m_WaitHandle );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVJsonCommandData::WaitForRequest( DWORD p_TimeoutInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	if( m_WaitHandle != NULL )
	{
		if( ::WaitForSingleObject( m_WaitHandle, p_TimeoutInMilliseconds ) != WAIT_OBJECT_0 )
		{
			l_Status = RPC_E_TIMEOUT; // timeout occurred
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

const std::string& SVJsonCommandData::GetJsonCommand() const
{
	return m_JsonCommand;
}

HRESULT SVJsonCommandData::SetJsonCommand( const std::string& p_rCommand )
{
	HRESULT l_Status = S_OK;

	m_JsonCommand = p_rCommand;

	if( m_WaitHandle != NULL )
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

HRESULT SVJsonCommandData::GetWaitHandle( HANDLE& p_rHandle ) const
{
	HRESULT l_Status = S_OK;

	if( p_rHandle != NULL )
	{
		::CloseHandle( p_rHandle );

		p_rHandle = NULL;
	}

	if( ! ::DuplicateHandle( GetCurrentProcess(), m_WaitHandle, GetCurrentProcess(), &p_rHandle, 0, FALSE, DUPLICATE_SAME_ACCESS ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

