//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadWait
//* .File Name       : $Workfile:   SVThreadWait.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:03:16  $
//******************************************************************************

#pragma once

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVThreadWait.h"

SVThreadWait::SVThreadWait()
: m_Thread(), m_WaitEvent( NULL )
{
	m_WaitEvent = ::CreateEvent( NULL, false, false, NULL );
}

SVThreadWait::~SVThreadWait()
{
	if( m_WaitEvent != NULL )
	{
		::CloseHandle( m_WaitEvent );

		m_WaitEvent = NULL;
	}
}

HRESULT SVThreadWait::Create()
{
	HRESULT l_Status = S_OK;

	if( m_WaitEvent == NULL )
	{
		m_WaitEvent = ::CreateEvent( NULL, false, false, NULL );
	}

	if( m_WaitEvent != NULL )
	{
		l_Status = m_Thread.Create( boost::bind( &SVThreadWait::Dispatch, this, _1 ), _T( "SVThreadWait" ) );

		if( l_Status == S_OK )
		{
			m_Thread.SetPriority( THREAD_PRIORITY_IDLE );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVThreadWait::Destroy()
{
	m_Thread.Destroy();

	if( m_WaitEvent != NULL )
	{
		::CloseHandle( m_WaitEvent );

		m_WaitEvent = NULL;
	}
}

void SVThreadWait::Sleep()
{
	if( m_WaitEvent != NULL )
	{
		if( m_Thread.Signal() == S_OK )
		{
			::WaitForSingleObject( m_WaitEvent, 1 );
		}
	}
}

void SVThreadWait::Dispatch( bool& p_WaitForEvents )
{
	::SetEvent( m_WaitEvent );
}

