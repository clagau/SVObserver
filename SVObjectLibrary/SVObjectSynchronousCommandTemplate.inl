//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectSynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectSynchronousCommandTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:01:02  $
//******************************************************************************

#ifndef SVOBJECTSYNCHRONOUSCOMMANDTEMPLATE_INL
#define SVOBJECTSYNCHRONOUSCOMMANDTEMPLATE_INL

#include "SVObjectSynchronousCommandTemplate.h"

#include "SVObjectCommandWrapperTemplate.h"
#include "SVObjectManagerClass.h"
#include "SVObjectThreadCommandTemplate.h"

template< typename SVCommandPtr >
SVObjectSynchronousCommandTemplate< SVCommandPtr >::SVObjectSynchronousCommandTemplate( const SVGUID& p_rObjectID, const SVCommandPtr& p_rCommandPtr )
: m_ObjectID( p_rObjectID ), m_CommandPtr( p_rCommandPtr )
{
}

template< typename SVCommandPtr >
SVObjectSynchronousCommandTemplate< SVCommandPtr >::~SVObjectSynchronousCommandTemplate()
{
}

template< typename SVCommandPtr >
HRESULT SVObjectSynchronousCommandTemplate< SVCommandPtr >::Execute( DWORD p_TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	if( !( m_ObjectID.empty() ) && !( m_CommandPtr.empty() ) )
	{
		typedef SVObjectCommandWrapperTemplate< SVCommandPtr >::SVObjectCommandWrapperPtr SVObjectCommandWrapperPtr;

		SVObjectCommandWrapperPtr l_WrapperPtr = new SVObjectCommandWrapperTemplate< SVCommandPtr >( m_CommandPtr );

		if( !( l_WrapperPtr.empty() ) )
		{
			SVCommandTemplatePtr l_CommandPtr = new SVObjectThreadCommandTemplate< SVObjectCommandWrapperPtr >( l_WrapperPtr );

			if( !( l_CommandPtr.empty() ) )
			{
				l_Status = SVObjectManagerClass::Instance().SubmitCommand( m_ObjectID, l_CommandPtr );

				if( l_Status == S_OK )
				{
					l_Status = l_WrapperPtr->WaitForRequest( p_TimeoutInMilliseconds );
				}

				if( l_Status == S_OK )
				{
					l_Status = l_WrapperPtr->GetStatus();
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
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

#endif

