//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectCommandDataJson
//* .File Name       : $Workfile:   SVObjectCommandDataJson.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectCommandDataJson.h"

SVObjectCommandDataJson::SVObjectCommandDataJson()
: m_WaitHandle( NULL )
, m_JsonCommand( _T( "" ) )
, m_JsonResults( _T( "" ) )
{
	m_WaitHandle = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

SVObjectCommandDataJson::SVObjectCommandDataJson( const SVObjectCommandDataJson& p_rObject )
: m_WaitHandle( NULL )
, m_JsonCommand( p_rObject.m_JsonCommand )
, m_JsonResults( p_rObject.m_JsonResults )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVObjectCommandDataJson::~SVObjectCommandDataJson()
{
	if( m_WaitHandle != NULL )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = NULL;
	}
}

bool SVObjectCommandDataJson::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_JsonCommand.length() == 0 );
	l_Status = l_Status && ( m_JsonResults.length() == 0 );

	return l_Status;
}

void SVObjectCommandDataJson::clear()
{
	m_JsonCommand.clear();
	m_JsonResults.clear();
}

HRESULT SVObjectCommandDataJson::NotifyRequestComplete() const
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

HRESULT SVObjectCommandDataJson::WaitForRequest( DWORD p_TimeoutInMilliseconds ) const
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

const std::string& SVObjectCommandDataJson::GetJsonCommand() const
{
	return m_JsonCommand;
}

HRESULT SVObjectCommandDataJson::SetJsonCommand( const std::string& p_rCommand )
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

HRESULT SVObjectCommandDataJson::GetWaitHandle( HANDLE& p_rHandle ) const
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectCommandDataJson.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:40:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:29:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated main application and create the command data holder for the JSON commands used with the socket and the remote input tool.  Updated source code to initialize, start and stop the scoket for sending and receiving data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
