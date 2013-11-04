//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAsynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectAsynchronousCommandTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:38:20  $
//******************************************************************************

#ifndef SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_INL
#define SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_INL

#include "SVObjectAsynchronousCommandTemplate.h"

#include "SVObjectManagerClass.h"
#include "SVObjectThreadCommandTemplate.h"

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SVObjectAsynchronousCommandTemplate()
: m_ObjectID(), m_CommandPtr(), m_WrapperPtr()
{
}

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SVObjectAsynchronousCommandTemplate( const SVObjectAsynchronousCommandTemplate& p_rObject )
: m_ObjectID( p_rObject.m_ObjectID ), m_CommandPtr( p_rObject.m_CommandPtr ), m_WrapperPtr( p_rObject.m_WrapperPtr )
{
}

template< typename SVCommandPtr >
SVObjectAsynchronousCommandTemplate< SVCommandPtr >::SVObjectAsynchronousCommandTemplate( const SVGUID& p_rObjectID, const SVCommandPtr& p_rCommandPtr )
: m_ObjectID( p_rObjectID ), m_CommandPtr( p_rCommandPtr ), m_WrapperPtr()
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

	if( !( m_ObjectID.empty() ) && !( m_CommandPtr.empty() ) )
	{
		m_WrapperPtr = new SVObjectCommandWrapperTemplate< SVCommandPtr >( m_CommandPtr );

		if( !( m_WrapperPtr.empty() ) )
		{
			SVCommandTemplatePtr l_CommandPtr = new SVObjectThreadCommandTemplate< SVObjectCommandWrapperPtr >( m_WrapperPtr );

			if( !( l_CommandPtr.empty() ) )
			{
				l_Status = SVObjectManagerClass::Instance().SubmitCommand( m_ObjectID, l_CommandPtr );
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

		if( l_Status == S_OK )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectAsynchronousCommandTemplate.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:38:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Feb 2013 10:57:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added and updated code to include asychronous command processing fucntionality to the object based command procesing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





