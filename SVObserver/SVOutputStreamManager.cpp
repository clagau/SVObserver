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

#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVObjectCommandDataJson.h"

SVOutputStreamManager& SVOutputStreamManager::Instance()
{
	static SVOutputStreamManager l_Object;

	return l_Object;
}

SVOutputStreamManager::~SVOutputStreamManager()
{
	Shutdown();
}

void SVOutputStreamManager::Startup(unsigned short p_PortNumber)
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
					SVObjectManagerClass::Instance().DetachObserver( "SVRemoteOutputGroup", m_OutputStream.second, m_OutputSocketCookie );
					SVObjectManagerClass::Instance().EraseObserver( m_OutputSocketCookie );

					m_OutputSocketCookie = 0;
				}

				SVObjectManagerClass::Instance().AttachObserver( "SVRemoteOutputGroup", m_OutputStream.second, l_NewCookie );
			}

			m_OutputSocketCookie = l_NewCookie;
		}

		m_SocketServer.ListenForClients( p_PortNumber, boost::bind( &SVOutputStreamManager::ProcessJsonCommand, this, _1, _2 ) );
	}
}

void SVOutputStreamManager::Shutdown()
{
	m_SocketServer.DisconnectFromClients();

	if( m_OutputSocketCookie != 0 )
	{
		if( !( m_OutputStream.second.empty() ) )
		{
			SVObjectManagerClass::Instance().DetachObserver( "SVRemoteOutputGroup", m_OutputStream.second, m_OutputSocketCookie );
		}

		SVObjectManagerClass::Instance().EraseObserver( m_OutputSocketCookie );
	}

	m_OutputStream.second.clear();
	m_OutputStream.first.clear();
	m_OutputSocketCookie = 0;
	m_SocketNotifyPtr.clear();
}

HRESULT SVOutputStreamManager::InsertOutputController( const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	m_OutputControllerId = p_rObjectId;

	return l_Status;
}

HRESULT SVOutputStreamManager::EraseOutputController()
{
	HRESULT l_Status = S_OK;

	m_OutputControllerId.clear();

	return l_Status;
}

HRESULT SVOutputStreamManager::InsertOutputStream( const std::string& p_rName, const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	EraseOutputStream();

	if( !( p_rObjectId.empty() ) && ( p_rObjectId != m_OutputStream.second ) )
	{
		if( m_OutputSocketCookie != 0 )
		{
			l_Status = SVObjectManagerClass::Instance().AttachObserver( "SVRemoteOutputGroup", p_rObjectId, m_OutputSocketCookie );
		}

		if( l_Status == S_OK )
		{
			m_OutputStream.first = p_rName;
			m_OutputStream.second = p_rObjectId;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVOutputStreamManager::EraseOutputStream()
{
	HRESULT l_Status = S_OK;

	if( !( m_OutputStream.second.empty() ) )
	{
		if( m_OutputSocketCookie != 0 )
		{
			SVObjectManagerClass::Instance().DetachObserver( "SVRemoteOutputGroup", m_OutputStream.second, m_OutputSocketCookie );
		}

		m_OutputStream.first.clear();
		m_OutputStream.second.clear();
	}

	return l_Status;
}

HRESULT SVOutputStreamManager::ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;
	SVString l_StreamName;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		if( l_JsonValues.isObject() )
		{
			Json::Value l_JsonCommand = l_JsonValues[ _T( "StreamName" ) ];

			if( l_JsonCommand.isString() )
			{
				l_StreamName = l_JsonCommand.asString();
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

	if( l_Status == S_OK )
	{
		if( l_StreamName.empty() )
		{
			l_Status = ProcessStreamManagerJsonCommand( p_rJsonCommand, p_rJsonResults );
		}
		else
		{
			l_Status = SendCommandToOutputStream( l_StreamName.ToString(), p_rJsonCommand, p_rJsonResults );
		}
	}

	return l_Status;
}

SVOutputStreamManager::SVOutputStreamManager()
: m_SocketServer()
, m_OutputSocketCookie( 0 )
, m_SocketNotifyPtr()
, m_OutputControllerId()
, m_OutputStream()
{
}

HRESULT SVOutputStreamManager::Rename( LPCTSTR p_OldName, LPCTSTR p_NewName)
{
	HRESULT l_hr = S_OK;
	l_hr = p_OldName == m_OutputStream.first ? S_OK : S_FALSE;
	if( l_hr == S_OK )
	{
		m_OutputStream.first = p_NewName ;
	}
	return l_hr;
}


HRESULT SVOutputStreamManager::SendCommandToOutputStream( const std::string& p_rName, const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( !( p_rName.empty() ) && ( p_rName == m_OutputStream.first ) )
	{
		SVObjectCommandDataJsonPtr l_CommandDataPtr = new SVObjectCommandDataJson();

		if( !( l_CommandDataPtr.empty() ) )
		{
			l_Status = l_CommandDataPtr->SetJsonCommand( p_rJsonCommand );

			if( l_Status == S_OK )
			{
				l_Status = SVObjectManagerClass::Instance().Notify( m_OutputStream.second, l_CommandDataPtr );
			}

			if( l_Status == S_OK )
			{
				l_Status = l_CommandDataPtr->WaitForRequest( 60000 );

				if( l_Status == S_OK )
				{
					p_rJsonResults = l_CommandDataPtr->m_JsonResults;
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
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVOutputStreamManager::ProcessStreamManagerJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	if( 0 < p_rJsonCommand.length() )
	{
		Json::Reader l_Reader;
		Json::Value l_JsonValues;
		SVString l_Command;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			if( l_JsonValues.isObject() )
			{
				Json::Value l_JsonCommand = l_JsonValues[ _T( "Command" ) ];

				if( l_JsonCommand.isString() )
				{
					l_Command = l_JsonCommand.asString();
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

		if( l_Status == S_OK )
		{
			if( l_Command == _T( "QueryStreamNames" ) )
			{
				Json::FastWriter l_Writer;
				Json::Value l_Object(Json::objectValue);
				Json::Value l_Array(Json::arrayValue);

				if( !( m_OutputStream.first.empty() ) )
				{
					l_Array.append( m_OutputStream.first.c_str() );
				}

				l_Object[ _T( "StreamNames" ) ] = l_Array;
				l_Object[ _T( "Response" ) ] = l_Command.c_str();

				p_rJsonResults = l_Writer.write( l_Object ).c_str();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
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
