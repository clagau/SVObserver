//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, INc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputStreamManager
//* .File Name       : $Workfile:   SVInputStreamManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:20  $
//******************************************************************************

#include "stdafx.h"
#include <boost/bind.hpp>
#include "SVInputStreamManager.h"

#include "JsonLib/include/json.h"
#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"

#include "SVObjectCommandDataJson.h"

SVInputStreamManager& SVInputStreamManager::Instance()
{
	static SVInputStreamManager l_Object;

	return l_Object;
}

SVInputStreamManager::~SVInputStreamManager()
{
	Shutdown();
}

void SVInputStreamManager::Startup(unsigned short p_PortNumber)
{
	m_SocketServer.ListenForClients( p_PortNumber, boost::bind( &SVInputStreamManager::ProcessJsonCommand, this, _1, _2 ) );
}

void SVInputStreamManager::Shutdown()
{
	m_SocketServer.DisconnectFromClients();
}

HRESULT SVInputStreamManager::QueryInputStreamNames( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	std::string l_JsonCommand;
	std::string l_JsonResults;
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[ _T( "Command" ) ] = _T( "QueryStreamNames" );

	l_JsonCommand = l_CommandWriter.write( l_Command );

	l_Status = ProcessJsonCommand( l_JsonCommand, l_JsonResults );

	HRESULT l_Temp = ConvertJsonToCommandDataHolder( p_rResults, _T( "Results" ), l_JsonResults );

	if( l_Status == S_OK )
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVInputStreamManager::QueryInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( &m_Lock );

	_variant_t l_VariantStreamName;

	l_Status = p_rAttributes.GetValue( _T( "StreamName" ), l_VariantStreamName );

	if( l_Status == S_OK )
	{
		std::string l_JsonCommand;
		std::string l_JsonResults;
		std::string l_StreamName = _bstr_t( l_VariantStreamName );
		Json::FastWriter l_CommandWriter;
		Json::Value l_Command(Json::objectValue);

		l_Command[ _T( "Command" ) ] = _T( "QueryDataItems" );
		l_Command[ _T( "StreamName" ) ] = l_StreamName;

		l_JsonCommand = l_CommandWriter.write( l_Command );

		l_Status = ProcessJsonCommand( l_JsonCommand, l_JsonResults );

		HRESULT l_Temp = ConvertJsonToCommandDataHolder( p_rResults, _T( "Results" ), l_JsonResults );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	return l_Status;
}

HRESULT SVInputStreamManager::ClearInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( &m_Lock );

	_variant_t l_VariantStreamName;

	l_Status = p_rAttributes.GetValue( _T( "StreamName" ), l_VariantStreamName );

	if( l_Status == S_OK )
	{
		std::string l_JsonCommand;
		std::string l_JsonResults;
		std::string l_StreamName = _bstr_t( l_VariantStreamName );
		Json::FastWriter l_CommandWriter;
		Json::Value l_Command(Json::objectValue);

		l_Command[ _T( "Command" ) ] = _T( "ClearDataItems" );
		l_Command[ _T( "StreamName" ) ] = l_StreamName;

		l_JsonCommand = l_CommandWriter.write( l_Command );

		l_Status = ProcessJsonCommand( l_JsonCommand, l_JsonResults );
	}

	return l_Status;
}

SVGUID SVInputStreamManager::FindInputStream( const std::string& p_rName )
{
	SVGUID l_SubjectID;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( &m_Lock );

	SVInputStreamMap::const_iterator l_Iter = m_InputStreams.find( p_rName );

	if( l_Iter != m_InputStreams.end() )
	{
		l_SubjectID = l_Iter->second;
	}

	return l_SubjectID;
}

HRESULT SVInputStreamManager::InsertInputStream( const std::string& p_rName, const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( &m_Lock );

	m_InputStreams[ p_rName ] = p_rObjectId;

	return l_Status;
}

HRESULT SVInputStreamManager::EraseInputStream( const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( &m_Lock );

	SVInputStreamMap::iterator l_Iter = m_InputStreams.begin();

	while( l_Iter != m_InputStreams.end() )
	{
		SVInputStreamMap::iterator l_Temp = l_Iter;

		++l_Iter;

		if( l_Temp->second == p_rObjectId )
		{
			m_InputStreams.erase( l_Temp );
		}
	}

	return l_Status;
}

HRESULT SVInputStreamManager::ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
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
			l_Status = SendCommandToInputStream( l_StreamName.ToString(), p_rJsonCommand, p_rJsonResults );
		}
	}

	return l_Status;
}

SVInputStreamManager::SVInputStreamManager()
: m_SocketServer()
, m_Lock()
, m_InputStreams()
{
}

HRESULT SVInputStreamManager::SendCommandToInputStream( const std::string& p_rName, const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;
	SVGUID l_SubjectID = FindInputStream( p_rName );

	p_rJsonResults = std::string( _T( "" ) );

	if( !( l_SubjectID.empty() ) )
	{
		SVObjectCommandDataJsonPtr l_CommandDataPtr = new SVObjectCommandDataJson();

		if( !( l_CommandDataPtr.empty() ) )
		{
			l_Status = l_CommandDataPtr->SetJsonCommand( p_rJsonCommand );

			if( l_Status == S_OK )
			{
				l_Status = SVObjectManagerClass::Instance().Notify( l_SubjectID, l_CommandDataPtr );
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

HRESULT SVInputStreamManager::ProcessStreamManagerJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
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

				if( !( m_InputStreams.empty() ) )
				{
					SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

					l_AutoLock.Assign( &m_Lock );

					SVInputStreamMap::iterator l_Iter = m_InputStreams.begin();

					while( l_Iter != m_InputStreams.end() )
					{
						l_Array.append( l_Iter->first.c_str() );

						++l_Iter;
					}
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

HRESULT SVInputStreamManager::ProcessJsonToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, Json::Value& p_rJson ) const
{
	HRESULT l_Status = S_OK;

	if( p_rJson.isNull() )
	{
		l_Status = p_rDataHolder.SetValue( p_rLabel.c_str(), p_rJson.asString().c_str(), true );
	}
	if( p_rJson.isInt() )
	{
		l_Status = p_rDataHolder.SetValue( p_rLabel.c_str(), p_rJson.asInt(), true );
	}
	else if( p_rJson.isUInt() )
	{
		l_Status = p_rDataHolder.SetValue( p_rLabel.c_str(), p_rJson.asUInt(), true );
	}
	else if( p_rJson.isDouble() )
	{
		l_Status = p_rDataHolder.SetValue( p_rLabel.c_str(), p_rJson.asDouble(), true );
	}
	else if( p_rJson.isString() )
	{
		l_Status = p_rDataHolder.SetValue( p_rLabel.c_str(), p_rJson.asString().c_str(), true );
	}
	else if( p_rJson.isBool() )
	{
		l_Status = p_rDataHolder.SetValue( p_rLabel.c_str(), p_rJson.asString().c_str(), true );
	}
	else if( p_rJson.isArray() )
	{
		l_Status = ProcessJsonValuesToCommandDataHolder( p_rDataHolder, p_rLabel, p_rJson );
	}
	else if( p_rJson.isObject() )
	{
		l_Status = ProcessJsonValuesToCommandDataHolder( p_rDataHolder, p_rLabel, p_rJson );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInputStreamManager::ProcessJsonValuesToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, Json::Value& p_rJson ) const
{
	HRESULT l_Status = S_OK;
	// SEJ - this needs to be fixed if anyone ever calls this...
	if( p_rJson.isArray() || p_rJson.isObject() )
	{
		SVCommandDataHolder l_ValuesDataHolder;

		Json::Value::iterator l_Iter = p_rJson.begin();

		while( l_Iter != p_rJson.end() )
		{
			HRESULT l_Temp = ProcessJsonToCommandDataHolder( l_ValuesDataHolder, l_Iter.memberName(), (*l_Iter) );

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_Iter;
		}

		HRESULT l_Temp = p_rDataHolder.SetContainer( p_rLabel.c_str(), l_ValuesDataHolder, true );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInputStreamManager::ConvertJsonToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, const std::string& p_rJson ) const
{
	HRESULT l_Status = S_OK;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJson, l_JsonValues, false ) )
	{
		l_Status = ProcessJsonToCommandDataHolder( p_rDataHolder, p_rLabel, l_JsonValues );
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInputStreamManager.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:58:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 13:43:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Notated and fixed issue with Json Library iterator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:40:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:39:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new stream manager to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
