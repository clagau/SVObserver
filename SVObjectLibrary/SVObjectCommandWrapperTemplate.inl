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
: m_CommandPtr( p_rCommandPtr ), m_WaitHandle( NULL ), m_Status( E_FAIL )
{
	m_WaitHandle = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

template< typename SVCommandPtr >
SVObjectCommandWrapperTemplate< SVCommandPtr >::~SVObjectCommandWrapperTemplate()
{
	if( m_WaitHandle != NULL )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = NULL;
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

template< typename SVCommandPtr >
HRESULT SVObjectCommandWrapperTemplate< SVCommandPtr >::IsRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( m_WaitHandle != NULL )
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

	if( m_WaitHandle != NULL )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectCommandWrapperTemplate.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:47:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2013 10:57:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added and updated code to include asychronous command processing fucntionality to the object based command procesing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





