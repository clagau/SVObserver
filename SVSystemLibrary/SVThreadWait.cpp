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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVThreadWait.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:03:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

