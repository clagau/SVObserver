//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandManager
//* .File Name       : $Workfile:   SVJsonCommandManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Jun 2013 17:15:28  $
//******************************************************************************

#ifndef SVJSONCOMMANDMANAGER_INL
#define SVJSONCOMMANDMANAGER_INL

#include <boost/bind.hpp>

#include "SVJsonCommandManager.h"

#include "SVStatusLibrary/SVStatusCodes.h"

template< typename SVCommandProcessor >
SVJsonCommandManager< SVCommandProcessor >::SVJsonCommandManager()
: m_SocketServer(), m_ProcessingAsyncCommand( 0 ), m_JsonCommandDataPtr(), m_AsyncProcedure()
{
}

template< typename SVCommandProcessor >
SVJsonCommandManager< SVCommandProcessor >& SVJsonCommandManager< SVCommandProcessor >::Instance()
{
	static SVJsonCommandManager l_Object;

	return l_Object;
}

template< typename SVCommandProcessor >
SVJsonCommandManager< SVCommandProcessor >::~SVJsonCommandManager()
{
	Shutdown();
}

template< typename SVCommandProcessor >
HRESULT SVJsonCommandManager< SVCommandProcessor >::ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( SVCommandProcessor::IsAsyncCommand( p_rJsonCommand ) )
	{
		l_Status = ProcessAsyncJsonCommand( p_rJsonCommand, p_rJsonResults );
	}
	else
	{
		l_Status = SVCommandProcessor::ProcessCommand( p_rJsonCommand, p_rJsonResults );
	}

	return l_Status;
}

template< typename SVCommandProcessor >
HRESULT SVJsonCommandManager< SVCommandProcessor >::ProcessJsonNotification( const std::string& p_rJsonNotification )
{
	HRESULT l_Status = S_OK;

	l_Status = m_SocketServer.WriteJson( p_rJsonNotification );

	return l_Status;
}

template< typename SVCommandProcessor >
void SVJsonCommandManager< SVCommandProcessor >::Startup(unsigned short p_PortNumber)
{
	m_AsyncProcedure.Create( &SVJsonCommandManager::APCThreadProcess, boost::bind(&SVJsonCommandManager::ThreadProcess, this, _1), "SVJsonCommandManager" );
	m_SocketServer.ListenForClients( p_PortNumber, boost::bind( &SVJsonCommandManager::ProcessJsonCommand, this, _1, _2 ) );
}

template< typename SVCommandProcessor >
void SVJsonCommandManager< SVCommandProcessor >::Shutdown()
{
	m_JsonCommandDataPtr.clear();
	m_SocketServer.DisconnectFromClients();
	m_AsyncProcedure.Destroy();
}

template< typename SVCommandProcessor >
void CALLBACK SVJsonCommandManager< SVCommandProcessor >::APCThreadProcess( DWORD dwParam )
{
}

template< typename SVCommandProcessor >
HRESULT SVJsonCommandManager< SVCommandProcessor >::ProcessAsyncJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( ::InterlockedExchange( &m_ProcessingAsyncCommand, 1 ) == 0 )
	{
		if( !( p_rJsonCommand.empty() ) )
		{
			m_JsonCommandDataPtr = new SVJsonCommandData();

			if( !( m_JsonCommandDataPtr.empty() ) )
			{
				l_Status = m_JsonCommandDataPtr->SetJsonCommand( p_rJsonCommand );

				if( l_Status == S_OK )
				{
					l_Status = m_AsyncProcedure.Signal( NULL );
				}

				if( l_Status == S_OK )
				{
					l_Status = m_JsonCommandDataPtr->WaitForRequest( 60000 );

					if( l_Status == S_OK )
					{
						p_rJsonResults = m_JsonCommandDataPtr->m_JsonResults;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		m_JsonCommandDataPtr.clear();
		::InterlockedExchange( &m_ProcessingAsyncCommand, 0 );
	}
	else
	{
		l_Status = SVMSG_54_SVIM_BUSY;
	}

	return l_Status;
}

template< typename SVCommandProcessor >
void SVJsonCommandManager< SVCommandProcessor >::ThreadProcess( bool& p_WaitForEvents )
{
	SVJsonCommandDataPtr l_CommandDataPtr = m_JsonCommandDataPtr;

	if( ( m_ProcessingAsyncCommand == 1 ) && !( l_CommandDataPtr.empty() ) )
	{
		SVCommandProcessor::ProcessAsyncCommand( l_CommandDataPtr->GetJsonCommand(), l_CommandDataPtr->m_JsonResults );

		l_CommandDataPtr->NotifyRequestComplete();
	}

	p_WaitForEvents = true;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVJsonCommandServerLibrary\SVJsonCommandManager.inl_v  $
 * 
 *    Rev 1.1   18 Jun 2013 17:15:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 May 2013 07:40:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  839
 * SCR Title:  Add Last Modified Notification to Remote Socket Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updted source code to add the new process notification methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:14:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
