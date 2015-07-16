//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputStreamManager
//* .File Name       : $Workfile:   SVOutputStreamManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:16:36  $
//******************************************************************************

#include "StdAfx.h"
#include <boost/bind.hpp>
#include "SVOutputStreamManager.h"
#include "SVRemoteControlConstants.h"
#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVObjectCommandDataJson.h"

#pragma region Constants
static const std::string scRemoteOutputGroupTag = "SVRemoteOutputGroup";
#pragma endregion

SVOutputStreamManager& SVOutputStreamManager::Instance()
{
	static SVOutputStreamManager l_Object;

	return l_Object;
}

SVOutputStreamManager::~SVOutputStreamManager()
{
	Shutdown();
}

void SVOutputStreamManager::Startup(unsigned short PortNumber)
{
	m_SocketNotifyPtr = new SVOutputSocketObserver( boost::bind( &SVJsonCommandServer::WriteJson, &m_SocketServer, _1 ) );

	if( !( m_SocketNotifyPtr.empty() ) )
	{
		long l_NewCookie = 0;

		SVObjectManagerClass::Instance().InsertObserver( m_SocketNotifyPtr, l_NewCookie );

		if( l_NewCookie != 0 )
		{
			if( !( m_OutputStream.second.empty() ) )
			{
				if( m_OutputSocketCookie != 0 )
				{
					SVObjectManagerClass::Instance().DetachObserver( scRemoteOutputGroupTag.c_str(), m_OutputStream.second, m_OutputSocketCookie );
					SVObjectManagerClass::Instance().EraseObserver( m_OutputSocketCookie );

					m_OutputSocketCookie = 0;
				}

				SVObjectManagerClass::Instance().AttachObserver( scRemoteOutputGroupTag.c_str(), m_OutputStream.second, l_NewCookie );
			}

			m_OutputSocketCookie = l_NewCookie;
		}

		m_SocketServer.ListenForClients( PortNumber, boost::bind( &SVOutputStreamManager::ProcessJsonCommand, this, _1, _2 ) );
	}
}

void SVOutputStreamManager::Shutdown()
{
	m_SocketServer.DisconnectFromClients();

	if( m_OutputSocketCookie != 0 )
	{
		if( !( m_OutputStream.second.empty() ) )
		{
			SVObjectManagerClass::Instance().DetachObserver( scRemoteOutputGroupTag.c_str(), m_OutputStream.second, m_OutputSocketCookie );
		}

		SVObjectManagerClass::Instance().EraseObserver( m_OutputSocketCookie );
	}

	m_OutputStream.second.clear();
	m_OutputStream.first.clear();
	m_OutputSocketCookie = 0;
	m_SocketNotifyPtr.clear();
}

HRESULT SVOutputStreamManager::InsertOutputController( const SVGUID& rObjectId )
{
	HRESULT hr = S_OK;

	m_OutputControllerId = rObjectId;

	return hr;
}

HRESULT SVOutputStreamManager::EraseOutputController()
{
	HRESULT hr = S_OK;

	m_OutputControllerId.clear();

	return hr;
}

HRESULT SVOutputStreamManager::InsertOutputStream( const std::string& rName, const SVGUID& rObjectId )
{
	HRESULT hr = S_OK;

	EraseOutputStream();

	if( !( rObjectId.empty() ) && ( rObjectId != m_OutputStream.second ) )
	{
		if( 0 != m_OutputSocketCookie )
		{
			hr = SVObjectManagerClass::Instance().AttachObserver( scRemoteOutputGroupTag.c_str(), rObjectId, m_OutputSocketCookie );
		}

		if( S_OK == hr )
		{
			m_OutputStream.first = rName;
			m_OutputStream.second = rObjectId;
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

HRESULT SVOutputStreamManager::EraseOutputStream()
{
	HRESULT hr = S_OK;

	if( !( m_OutputStream.second.empty() ) )
	{
		if( 0 != m_OutputSocketCookie )
		{
			SVObjectManagerClass::Instance().DetachObserver( scRemoteOutputGroupTag.c_str(), m_OutputStream.second, m_OutputSocketCookie );
		}

		m_OutputStream.first.clear();
		m_OutputStream.second.clear();
	}

	return hr;
}

HRESULT SVOutputStreamManager::ProcessJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = E_INVALIDARG;

	Json::Reader Reader;
	Json::Value JsonValues;
	std::string StreamName;
	std::string CmdName;

	if (Reader.parse(rJsonCommand, JsonValues, false))
	{
		if (JsonValues.isObject())
		{
			// Stream name is optional for QueryListNames (JSON tag = QueryStreamNames)
			// Stream name is required for QueryListItems (JSON tag = QueryDataItems) and for Start/Stop(JSON tags = StartStream/StopStream)
			const Json::Value& streamNameValue = JsonValues[SVRC::stream::streamName];
			if (streamNameValue.isString())
			{
				StreamName = streamNameValue.asString();
			}
			// Comamnd Name is required
			const Json::Value& cmdValue = JsonValues[SVRC::stream::command];
			if (cmdValue.isString())
			{
				CmdName = cmdValue.asString(); 
					
				if( StreamName.empty() )
				{
					hr = ProcessStreamManagerJsonCommand( rJsonCommand, rJsonResults );
				}
				else
				{
					hr = SendCommandToOutputStream( CmdName, StreamName, rJsonCommand, rJsonResults );
				}
			}
		}
	}

	// always send a response...
	if (S_OK != hr)
	{
		//Build Response
		Json::FastWriter l_Writer;
		Json::Value value(Json::objectValue);

		value[SVRC::stream::response] = CmdName.c_str();
		value[SVRC::stream::streamName] = StreamName.c_str();
		value[SVRC::stream::status] = hr;

		rJsonResults = l_Writer.write(value).c_str();
	}
	return hr;
}

SVOutputStreamManager::SVOutputStreamManager()
: m_SocketServer()
, m_OutputSocketCookie( 0 )
, m_SocketNotifyPtr()
, m_OutputControllerId()
, m_OutputStream()
{
}

HRESULT SVOutputStreamManager::Rename( LPCTSTR OldName, LPCTSTR NewName)
{
	HRESULT l_hr = S_OK;
	l_hr = OldName == m_OutputStream.first ? S_OK : S_FALSE;
	if( l_hr == S_OK )
	{
		m_OutputStream.first = NewName ;
	}
	return l_hr;
}

enum CommandTimeouts
{
	SIXTY_SECONDS = 60000
};

HRESULT SVOutputStreamManager::SendCommandToOutputStream( const std::string& rCmdName, const std::string& rName, const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = E_INVALIDARG;

	if( !( rName.empty() ) && ( rName == m_OutputStream.first ) )
	{
		SVObjectCommandDataJsonPtr CommandDataPtr = new SVObjectCommandDataJson();

		if( !( CommandDataPtr.empty() ) )
		{
			hr = CommandDataPtr->SetJsonCommand( rJsonCommand );

			if( S_OK == hr )
			{
				hr = SVObjectManagerClass::Instance().Notify( m_OutputStream.second, CommandDataPtr );
			}

			if( S_OK == hr )
			{
				hr = CommandDataPtr->WaitForRequest( SIXTY_SECONDS );

				if( S_OK == hr )
				{
					rJsonResults = CommandDataPtr->m_JsonResults;
				}
			}
		}
	}
	return hr;
}

HRESULT SVOutputStreamManager::ProcessStreamManagerJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = E_INVALIDARG;

	if( 0 < rJsonCommand.length() )
	{
		Json::Reader Reader;
		Json::Value JsonValues;
		SVString Command;

		if( Reader.parse( rJsonCommand, JsonValues, false ) )
		{
			if( JsonValues.isObject() )
			{
				Json::Value JsonCommand = JsonValues[ SVRC::stream::command ];

				if( JsonCommand.isString() )
				{
					Command = JsonCommand.asString();
					if( Command == SVRC::iobroker::queryStreamNames.c_str() )
					{
						hr = S_OK;
						Json::FastWriter Writer;
						Json::Value Object(Json::objectValue);
						Json::Value Array(Json::arrayValue);

						// @TODO - this will need to change when there can be more than one
						if( !( m_OutputStream.first.empty() ) )
						{
							Array.append( m_OutputStream.first.c_str() );
						}

						Object[ SVRC::iobroker::streamNames ] = Array;
						Object[ SVRC::stream::response ] = Command.c_str();
						Object[ SVRC::stream::status ] = hr;

						rJsonResults = Writer.write( Object ).c_str();
						
					}
				}
			}
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOutputStreamManager.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:16:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Feb 2013 14:13:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that uses the subject/observer design patten as part of the object manager with the new parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Apr 2012 11:36:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added remane function for the output stream name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 10:06:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote output stream and communications to that remote output manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jan 2012 11:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
