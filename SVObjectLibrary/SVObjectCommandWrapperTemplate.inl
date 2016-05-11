//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectCommandWrapperTemplate
//* .File Name       : $Workfile:   SVObjectCommandWrapperTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:47:22  $
//******************************************************************************

#ifndef SVOBJECTCOMMANDWRAPPERTEMPLATE_INL
#define SVOBJECTCOMMANDWRAPPERTEMPLATE_INL

#include "SVObjectCommandWrapperTemplate.h"

template< typename SVCommandPtr >
SVObjectCommandWrapperTemplate< SVCommandPtr >::SVObjectCommandWrapperTemplate( const SVCommandPtr& p_rCommandPtr )
: m_CommandPtr( p_rCommandPtr ), m_WaitHandle( nullptr ), m_Status( E_FAIL )
{
	m_WaitHandle = ::CreateEvent( nullptr, true, false, nullptr );
}

template< typename SVCommandPtr >
SVObjectCommandWrapperTemplate< SVCommandPtr >::~SVObjectCommandWrapperTemplate()
{
	if( nullptr != m_WaitHandle )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = nullptr;
	}
}

template< typename SVCommandPtr >
HRESULT SVObjectCommandWrapperTemplate< SVCommandPtr >::Execute()
{
	HRESULT l_Status = S_OK;

	if( !( m_CommandPtr.empty() ) )
	{
		m_Status = m_CommandPtr->Execute();

		NotifyRequestComplete();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVCommandPtr >
HRESULT SVObjectCommandWrapperTemplate< SVCommandPtr >::NotifyRequestComplete() const
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

template< typename SVCommandPtr >
HRESULT SVObjectCommandWrapperTemplate< SVCommandPtr >::IsRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
	{
		DWORD l_WaitStatus = ::WaitForSingleObject( m_WaitHandle, 0 );

		switch( l_WaitStatus )
		{
		case WAIT_OBJECT_0:
			{
				break;
			}
		case WAIT_TIMEOUT:
			{
				l_Status = S_FALSE;

				break;
			}
		case WAIT_FAILED:
		default:
			{
				l_Status = E_FAIL;

				break;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVCommandPtr >
HRESULT SVObjectCommandWrapperTemplate< SVCommandPtr >::WaitForRequest( DWORD p_TimeoutInMilliseconds ) const
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

template< typename SVCommandPtr >
HRESULT SVObjectCommandWrapperTemplate< SVCommandPtr >::GetStatus() const
{
	return m_Status;
}

#endif

