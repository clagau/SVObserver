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
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVThreadWait.h"

SVThreadWait::SVThreadWait()
: m_Thread(), m_WaitEvent( nullptr )
{
	m_WaitEvent = ::CreateEvent( nullptr, false, false, nullptr );
}

SVThreadWait::~SVThreadWait()
{
	if( nullptr != m_WaitEvent )
	{
		::CloseHandle( m_WaitEvent );

		m_WaitEvent = nullptr;
	}
}

HRESULT SVThreadWait::Create()
{
	HRESULT l_Status = S_OK;

	if( nullptr == m_WaitEvent )
	{
		m_WaitEvent = ::CreateEvent( nullptr, false, false, nullptr );
	}

	if( nullptr != m_WaitEvent )
	{
		l_Status = m_Thread.Create( boost::bind( &SVThreadWait::Dispatch, this, _1 ), _T( "SVThreadWait" ) );

		if( S_OK == l_Status )
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

	if( nullptr != m_WaitEvent )
	{
		::CloseHandle( m_WaitEvent );

		m_WaitEvent = nullptr;
	}
}

void SVThreadWait::Sleep()
{
	if( nullptr != m_WaitEvent )
	{
		if( S_OK == m_Thread.Signal() )
		{
			::WaitForSingleObject( m_WaitEvent, 1 );
		}
	}
}

void SVThreadWait::Dispatch( bool& )
{
	::SetEvent( m_WaitEvent );
}

