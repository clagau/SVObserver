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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonCommandData.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2014 10:28:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  949
 * SCR Title:  Repeatedly Calling PutConfig Causes SVObserver to Stop Responding
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised WaitForresquest to return RPC_E_TIMEOUT when a timeout occurs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:14:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
