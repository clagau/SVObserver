//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAsynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectAsynchronousCommandTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Nov 2014 10:00:04  $
//******************************************************************************

#ifndef SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_INL
#define SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_INL

#include "SVObjectAsynchronousCommandTemplate.h"

#include "SVObjectManagerClass.h"
#include "SVObjectThreadCommandTemplate.h"

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SVObjectAsynchronousCommandTemplate()
: m_Object(nullptr), m_CommandPtr(), m_WrapperPtr()
{
}

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SVObjectAsynchronousCommandTemplate( const SVObjectAsynchronousCommandTemplate& p_rObject )
: m_Object( p_rObject.m_Object ), m_CommandPtr( p_rObject.m_CommandPtr ), m_WrapperPtr( p_rObject.m_WrapperPtr )
{
}

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SVObjectAsynchronousCommandTemplate( SVObjectClass& p_rObject, const SVCommandPtr& p_rCommandPtr )
: m_Object( &p_rObject ), m_CommandPtr( p_rCommandPtr ), m_WrapperPtr()
{
}

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::~SVObjectAsynchronousCommandTemplate()
{
}

template< typename SVCommandPtr >
HRESULT SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SubmitCommand()
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_Object && !( m_CommandPtr.empty() ) )
	{
		m_WrapperPtr = new SVObjectCommandWrapperTemplate< SVCommandPtr >( m_CommandPtr );

		if( !( m_WrapperPtr.empty() ) )
		{
			SVCommandTemplatePtr l_CommandPtr = new SVObjectThreadCommandTemplate< SVObjectCommandWrapperPtr >( m_WrapperPtr );

			if( !( l_CommandPtr.empty() ) )
			{
				l_Status = SVObjectManagerClass::Instance().SubmitCommand( *m_Object, l_CommandPtr );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
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
HRESULT SVObjectAsynchronousCommandTemplate< SVCommandPtr >::IsCommandComplete()
{
	HRESULT l_Status = S_OK;

	if( !( m_WrapperPtr.empty() ) )
	{
		l_Status = m_WrapperPtr->IsRequestComplete();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVCommandPtr >
HRESULT SVObjectAsynchronousCommandTemplate< SVCommandPtr >::WaitForCommandToComplete( DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	if( !( m_WrapperPtr.empty() ) )
	{
		l_Status = m_WrapperPtr->WaitForRequest( p_TimeoutInMilliseconds );

		if( S_OK == l_Status )
		{
			l_Status = m_WrapperPtr->GetStatus();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVCommandPtr >
const SVCommandPtr& SVObjectAsynchronousCommandTemplate< SVCommandPtr >::GetCommandPtr() const
{
	return m_CommandPtr;
}

#endif

