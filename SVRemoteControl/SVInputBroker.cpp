//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputBroker
//* .File Name       : $Workfile:   SVInputBroker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   16 Oct 2014 10:11:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVInputBroker.h"
#include "JsonLib/include/json.h"
#include "SVInputBrokerJsonBuilder.h"
#include "SVMatchStruct.h"
#include "SVStringConversions.h"
#include "Logging.h"
#pragma endregion Includes
using namespace  SvSol;

SVInputBroker::SVInputBroker()
:	m_RemoteCtrl( nullptr ),
	m_TimeoutInSeconds( 5 ),
	m_ServerAddress(),
	m_ServerPort( 32100 ),
	m_ClientSocket(),
	m_Command()
{
	m_ClientSocket.SetConnectionStatusCallback( boost::bind( &SVInputBroker::OnConnectionStatus, this, _1 ) );
	m_ClientSocket.SetDataReceivedCallback( boost::bind( &SVInputBroker::OnControlDataReceived, this, _1 ) );

	HKEY l_Key = nullptr;

	if( ERROR_SUCCESS == ::RegOpenKeyExW( HKEY_LOCAL_MACHINE, L"SOFTWARE\\Seidenader\\SVInputBroker", 0, KEY_READ, &l_Key ) )
	{
		DWORD l_Type = 0;
		DWORD l_Value = 0;
		DWORD l_Size = sizeof( l_Value );
		
		if( ERROR_SUCCESS == ::RegQueryValueExW( l_Key, L"CommandPort", nullptr, &l_Type, reinterpret_cast< BYTE* >( &l_Value ), &l_Size ) )
		{
			m_ServerPort = static_cast< unsigned short >( l_Value );
		}
	}
}

HRESULT SVInputBroker::FinalConstruct()
{
	return S_OK;
}

void SVInputBroker::FinalRelease()
{
	ClearRemoteControl();
}


STDMETHODIMP SVInputBroker::get_TimeoutInSeconds(long* pVal)
{
	HRESULT l_Status = S_OK;

	*pVal = m_TimeoutInSeconds;

	return l_Status;
}

STDMETHODIMP SVInputBroker::put_TimeoutInSeconds(long Val)
{
	HRESULT l_Status = S_OK;

	m_TimeoutInSeconds = Val;

	return l_Status;
}

STDMETHODIMP SVInputBroker::QueryListNames(SAFEARRAY** ppNames)
{
	HRESULT l_Status = S_OK;

	if( ppNames != nullptr )
	{
		if( *ppNames != nullptr )
		{
			::SafeArrayDestroy( *ppNames );

			*ppNames = nullptr;
		}
		
		CComSafeArrayBound l_Bound( 0, 1 );
		CComSafeArray< BSTR > l_Array( l_Bound );

		std::string l_JsonCommand;
		Json::FastWriter l_CommandWriter;
		Json::Value l_Command(Json::objectValue);

		l_Command[ "Command" ] = "QueryStreamNames";

		l_JsonCommand = l_CommandWriter.write( l_Command );

		l_Status = m_Command.SetJsonCommand( l_JsonCommand, m_TimeoutInSeconds * 1000 );

		if( l_Status == S_OK )
		{
			l_Status = WriteToSocket( l_JsonCommand );

			if( l_Status == S_OK )
			{
				l_Status = m_Command.WaitForRequest( m_TimeoutInSeconds * 1000 );
			}
			else
			{
				SVLOG(l_Status);
			}

			if( l_Status == S_OK )
			{
				const Json::Value& l_JsonValues = m_Command.GetJsonResults();
				if (!l_JsonValues.empty() && l_JsonValues.isObject())
				{
					Json::Value l_JsonDataItems = l_JsonValues[ "StreamNames" ];

					if( l_JsonDataItems.isArray() )
					{
						Json::Value::iterator l_Iter = l_JsonDataItems.begin();

						while( l_Iter != l_JsonDataItems.end() )
						{
							HRESULT l_Temp = S_OK;

							Json::Value l_JsonItem = ( *l_Iter );

							if( l_JsonItem.isString() )
							{
								CComBSTR l_Value = SVStringConversions::to_utf16( l_JsonItem.asString() ).c_str();

								l_Temp = l_Array.Add( l_Value.Detach() );
							}
							else
							{
								l_Temp = E_INVALIDARG;
								SVLOG(l_Temp);
							}

							if( l_Status == S_OK )
							{
								l_Status = l_Temp;
							}

							++l_Iter;
						}
					}
					else
					{
						l_Status = E_INVALIDARG;
						SVLOG(l_Status);
					}
				}
				else
				{
					l_Status = INET_E_INVALID_REQUEST;
					SVLOG(l_Status);
				}
			}

			m_Command.ClearJsonCommand();
		}

		*ppNames = l_Array.Detach();
	}
	else
	{
		l_Status = E_INVALIDARG;
		SVLOG(l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVInputBroker::QueryListItems(BSTR ListName, ISVInputMatchList** ppItems)
{
	HRESULT l_Status = S_OK;

	_bstr_t l_ListName = ListName;

	if( ppItems != nullptr )
	{
		if( *ppItems != nullptr )
		{
			( *ppItems )->Release();

			*ppItems = nullptr;
		}
		
		if (l_ListName.length() > 0)
		{
			CComPtr<ISVInputMatchList> l_MatchListPtr;

			l_Status = l_MatchListPtr.CoCreateInstance(__uuidof(SVInputMatchList));

			if( l_Status == S_OK && l_MatchListPtr.p != nullptr )
			{
				std::string l_JsonCommand;
				Json::FastWriter l_CommandWriter;
				Json::Value l_Command(Json::objectValue);

				l_Command[ "Command" ] = "QueryDataItems";
				l_Command[ "StreamName" ] = SVStringConversions::to_utf8( l_ListName );

				l_JsonCommand = l_CommandWriter.write( l_Command );

				l_Status = m_Command.SetJsonCommand( l_JsonCommand, m_TimeoutInSeconds * 1000 );

				if( l_Status == S_OK )
				{
					l_Status = WriteToSocket( l_JsonCommand );

					if( l_Status == S_OK )
					{
						l_Status = m_Command.WaitForRequest( m_TimeoutInSeconds * 1000 );

						if( l_Status == S_OK )
						{
							const Json::Value& l_JsonValues = m_Command.GetJsonResults();
							if (!l_JsonValues.empty() && l_JsonValues.isObject())
							{
								Json::Value l_JsonDataItems = l_JsonValues[ "DataItems" ];

								if( l_JsonDataItems.isArray() )
								{
									Json::Value::iterator l_Iter = l_JsonDataItems.begin();

									while( l_Iter != l_JsonDataItems.end() )
									{
										HRESULT l_Temp = S_OK;

										Json::Value l_JsonItem = ( *l_Iter );

										if( l_JsonItem.isObject() )
										{
											Json::Value l_JsonMatchString = l_JsonItem[ "MatchString" ];
											Json::Value l_JsonMatchId = l_JsonItem[ "MatchStringId" ];

											CComBSTR l_MatchString = SVStringConversions::to_utf16( l_JsonMatchString.asString() ).c_str();

											l_Temp = l_MatchListPtr->AddItem( l_MatchString.Detach(), l_JsonMatchId.asInt() );
										}
										else
										{
											l_Temp = E_INVALIDARG;
											SVLOG(l_Temp);
										}

										if( l_Status == S_OK )
										{
											l_Status = l_Temp;
										}

										++l_Iter;
									}
								}
								else
								{
									l_Status = E_INVALIDARG;
									SVLOG(l_Status);
								}
							}
							else
							{
								l_Status = INET_E_INVALID_REQUEST;
								SVLOG(l_Status);
							}
						}

						m_Command.ClearJsonCommand();
					}
				}
				*ppItems = l_MatchListPtr.Detach();
			}
			else
			{
				if( l_Status == S_OK )
				{
					l_Status = E_NOINTERFACE;
					SVLOG(l_Status);
				}
			}

		}
		else
		{
			l_Status = E_INVALIDARG;
			SVLOG(l_Status);
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
		SVLOG(l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVInputBroker::Clear(BSTR ListName)
{
	HRESULT l_Status = S_OK;

	_bstr_t l_ListName = ListName;

	if (l_ListName.length() > 0)
	{
		if( m_Command.IsJsonCommandEmpty() )
		{
			const std::string& l_JsonCommand = SVInputBrokerJsonBuilder::BuildClearDataItemsCommand(static_cast< const wchar_t* >( l_ListName ) );
			if (!l_JsonCommand.empty())
			{
				l_Status = m_Command.SetJsonCommand( l_JsonCommand, m_TimeoutInSeconds * 1000 );

				if( l_Status == S_OK )
				{
					l_Status = WriteToSocket( l_JsonCommand );

					m_Command.ClearJsonCommand();
				}
				else
				{
					SVLOG(l_Status);
				}
			}
			else
			{
				l_Status = INET_E_INVALID_REQUEST;
				SVLOG(l_Status);
			}
		}
		else
		{
			l_Status = INET_E_INVALID_REQUEST;
			SVLOG(l_Status);
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
		SVLOG(l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVInputBroker::Add(BSTR ListName, ISVInputMatchList* pItems)
{
	HRESULT l_Status = S_OK;

	CComPtr< ISVInputMatchList > l_ListPtr( pItems );

	_bstr_t l_ListName = ListName;

	if (l_ListName.length() > 0)
	{
		SVMatchList l_MatchList;

		long l_Count = 0;

		l_Status = l_ListPtr->get_Count( &l_Count );

		for( long i = 0; ( l_Status == S_OK ) && ( i < l_Count ); ++i )
		{
			_variant_t l_VariantDispatch;

			HRESULT l_Temp = l_ListPtr->get_Item( _variant_t( i ), l_VariantDispatch.GetAddress() );

			if( l_Temp == S_OK )
			{
				LPDISPATCH l_pDispatch = l_VariantDispatch;

				if( l_pDispatch != nullptr )
				{
					ISVInputMatchItem* l_pItem = nullptr;

					l_Temp = l_pDispatch->QueryInterface( IID_ISVInputMatchItem, reinterpret_cast< void** >( &l_pItem ) );

					if( l_Temp == S_OK )
					{
						if( l_pItem != nullptr )
						{
							_bstr_t l_MatchString;
							long l_MatchId;

							l_Temp = l_pItem->get_MatchString( l_MatchString.GetAddress() );

							if( l_Temp == S_OK )
							{
								l_Temp = l_pItem->get_TxnIdentifier( &l_MatchId );
							}

							if( l_Temp == S_OK )
							{
								l_MatchList.push_back(SVMatchStruct(static_cast< const wchar_t* >( l_MatchString ), l_MatchId));
							}

							l_pItem->Release();
						}
						else
						{
							l_Temp = E_NOINTERFACE;
						}
					}

					l_pDispatch->Release();
				}
				else
				{
					l_Temp = E_NOINTERFACE;
				}
			}

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
				SVLOG(l_Status);
			}
		}
		if( l_Status == S_OK )
		{
			const std::string& l_JsonCommand = SVInputBrokerJsonBuilder::BuildAddDataItemsCommand(static_cast< const wchar_t* >( l_ListName ), l_MatchList);

			if (!l_JsonCommand.empty())
			{
				l_Status = m_Command.SetJsonCommand( l_JsonCommand, m_TimeoutInSeconds * 1000 );

				if( l_Status == S_OK )
				{
					l_Status = WriteToSocket( l_JsonCommand );

					m_Command.ClearJsonCommand();
				}
			}
			else
			{
				l_Status = E_UNEXPECTED;
				SVLOG(l_Status);
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
		SVLOG(l_Status);
	}

	return l_Status;
}

HRESULT SVInputBroker::AssignRemoteControl( ISVRemoteCtrl* newVal )
{
	HRESULT l_Status = S_OK;

	ClearRemoteControl();

	if( newVal != nullptr )
	{
		m_RemoteCtrl = newVal;

		l_Status = CheckSocketConnection();
	}

	return l_Status;
}

void SVInputBroker::ClearRemoteControl()
{
	m_ClientSocket.Disconnect();

	m_RemoteCtrl.Release();
}

HRESULT SVInputBroker::CheckSocketConnection()
{
	HRESULT l_Status = S_OK;

	if( m_RemoteCtrl.p != nullptr )
	{
		_bstr_t l_Addr;

		l_Status = m_RemoteCtrl->get_VPAddress( l_Addr.GetAddress() );

		if( l_Status == S_OK )
		{
			if( l_Addr != m_ServerAddress || !m_ClientSocket.IsConnected() )
			{
				SvSol::SVSocketError::ErrorEnum err = SvSol::SVSocketError::Success;
				if(	(err = m_ClientSocket.BuildConnection( l_Addr, m_ServerPort, ( m_TimeoutInSeconds * 1000 ) ) ) == SVSocketError::Success)
				{
					m_ServerAddress = l_Addr;
				}
				else
				{
					l_Status = SvSol::SVSocketError::HrFromSocketError(err);
					SVLOG(l_Status);
				}
			}
		}
	}
	else
	{
		l_Status = E_NOINTERFACE;
		SVLOG(l_Status);
	}

	return l_Status;
}

HRESULT SVInputBroker::WriteToSocket( const std::string& data )
{
	HRESULT l_Status = CheckSocketConnection();

	if( l_Status == S_OK )
	{
			l_Status = SvSol::SVSocketError::HrFromSocketError(m_ClientSocket.Write( data ));
			SVLOG(l_Status);
	}
	else
	{
		l_Status = E_NOINTERFACE;
		SVLOG(l_Status);
	}

	return l_Status;
}

void SVInputBroker::OnConnectionStatus(SvSol::ConnectionState state )
{
}

void SVInputBroker::OnControlDataReceived( std::istream& is )
{
	bool l_Notify = false;

	std::string l_JsonCommand;

	if( m_Command.GetJsonCommand( l_JsonCommand, INFINITE ) == S_OK )
	{
		Json::Reader l_Reader;
		Json::Value l_JsonCommandValues;
		Json::Value l_JsonResponseValues;

		if( ( l_Reader.parse( l_JsonCommand, l_JsonCommandValues, false ) ) &&
			( l_Reader.parse( is, l_JsonResponseValues, false ) ) )
		{
			std::string l_Command;
			std::string l_Response;

			if( l_JsonCommandValues.isObject() )
			{
				Json::Value l_JsonValue = l_JsonCommandValues[ "Command" ];

				if( l_JsonValue.isString() )
				{
					l_Command = l_JsonValue.asString();
				}
			}

			if( l_JsonResponseValues.isObject() )
			{
				Json::Value l_JsonValue = l_JsonResponseValues[ "Response" ];

				if( l_JsonValue.isString() )
				{
					l_Response = l_JsonValue.asString();
				}
			}

			l_Notify = ( !( l_Command.empty() ) && ( l_Command == l_Response ) );

			if( l_Notify )
			{
				l_Notify = ( m_Command.SetJsonResults( l_JsonResponseValues ) == S_OK );
			}
		}
	}

	if( l_Notify )
	{
		m_Command.NotifyRequestComplete();
	}
}
