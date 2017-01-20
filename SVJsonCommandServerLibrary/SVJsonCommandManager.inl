//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandManager
//* .File Name       : $Workfile:   SVJsonCommandManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Dec 2014 12:22:52  $
//******************************************************************************

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
HRESULT SVJsonCommandManager< SVCommandProcessor >::ProcessJsonCommand( const SVString& rJsonCommand, SVString& rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( SVCommandProcessor::IsAsyncCommand( rJsonCommand ) )
	{
		l_Status = ProcessAsyncJsonCommand( rJsonCommand, rJsonResults );
	}
	else
	{
		l_Status = SVCommandProcessor::ProcessCommand( rJsonCommand, rJsonResults );
	}
	return l_Status;
}

template< typename SVCommandProcessor >
HRESULT SVJsonCommandManager< SVCommandProcessor >::ProcessJsonNotification( const SVString& rJsonNotification )
{
	HRESULT l_Status = S_OK;

	l_Status = m_SocketServer.WriteJson( rJsonNotification );

	return l_Status;
}

template< typename SVCommandProcessor >
void SVJsonCommandManager< SVCommandProcessor >::Startup(unsigned short p_PortNumber)
{
	m_AsyncProcedure.Create( &SVJsonCommandManager::APCThreadProcess, boost::bind(&SVJsonCommandManager::ThreadProcess, this, _1), "SVJsonCommandManager", SVNone );
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
void CALLBACK SVJsonCommandManager< SVCommandProcessor >::APCThreadProcess( DWORD_PTR dwParam )
{
}

template< typename SVCommandProcessor >
HRESULT SVJsonCommandManager< SVCommandProcessor >::ProcessAsyncJsonCommand( const SVString& rJsonCommand, SVString& rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( ::InterlockedExchange( &m_ProcessingAsyncCommand, 1 ) == 0 )
	{
		if( !( rJsonCommand.empty() ) )
		{
			m_JsonCommandDataPtr = new SVJsonCommandData();

			if( !( m_JsonCommandDataPtr.empty() ) )
			{
				l_Status = m_JsonCommandDataPtr->SetJsonCommand( rJsonCommand );

				if( S_OK == l_Status )
				{
					l_Status = m_AsyncProcedure.Signal( nullptr );
				}

				if( S_OK == l_Status )
				{
					l_Status = m_JsonCommandDataPtr->WaitForRequest( 60000 * 5 );

					if( S_OK == l_Status )
					{
						rJsonResults = m_JsonCommandDataPtr->GetJsonResultsObject();
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
	if (S_OK != l_Status)
	{
		SVCommandProcessor::BuildErrorResponse(rJsonCommand, rJsonResults, l_Status);
	}
	return l_Status;
}

template< typename SVCommandProcessor >
void SVJsonCommandManager< SVCommandProcessor >::ThreadProcess( bool& p_WaitForEvents )
{
	SVJsonCommandDataPtr l_CommandDataPtr = m_JsonCommandDataPtr;

	if( ( m_ProcessingAsyncCommand == 1 ) && !( l_CommandDataPtr.empty() ) )
	{
		SVCommandProcessor::ProcessAsyncCommand( l_CommandDataPtr->GetJsonCommand(), l_CommandDataPtr->GetJsonResultsObject() );

		l_CommandDataPtr->NotifyRequestComplete();
	}

	p_WaitForEvents = true;
}

