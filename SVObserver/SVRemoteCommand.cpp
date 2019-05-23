//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCommand
//* .File Name       : $Workfile:   SVRemoteCommand.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:42:46  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVObserver_i.h" //Note this header must be before the classes dependent on the UUIDs
#include "SVRemoteCommand.h"
#include "SVRemoteCommandData.h"
#include "SVCommandLibrary/SVCommandLibraryEnums.h"
#pragma endregion Includes

SVRemoteCommand::SVRemoteCommand()
: m_pCommand()
{
}

SVRemoteCommand::~SVRemoteCommand()
{
}

void SVRemoteCommand::SetCommand(SvOi::ICommandPtr pCommand)
{
	m_pCommand = pCommand;
}

STDMETHODIMP SVRemoteCommand::GetCommandType(long* pCommandType)
{
	HRESULT l_Status = S_OK;

	if( nullptr != pCommandType )
	{
		if(nullptr != m_pCommand)
		{
			*pCommandType = m_pCommand->GetCommandType();
		}
		else
		{
			*pCommandType = SV_COMMAND_TYPE_UNKNOWN;

			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::ConstructCommandData( ISVRemoteCommandData **ppCommandData )
{
	HRESULT l_Status = S_OK;

	if( nullptr != ppCommandData )
	{
		if( nullptr != *ppCommandData )
		{
			( *ppCommandData )->Release();

			*ppCommandData = nullptr;
		}

		CComPtr< ISVRemoteCommandData > l_RemoteCommandDataPtr;

		l_Status = l_RemoteCommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

		if( nullptr == l_RemoteCommandDataPtr.p )
		{
			if( S_OK == l_Status )
			{
				l_Status = E_FAIL;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = l_RemoteCommandDataPtr.QueryInterface( ppCommandData );
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::GetAttributes(ISVRemoteCommandData** ppAttributes)
{
	HRESULT l_Status = S_OK;

	if( nullptr != ppAttributes )
	{
		if( nullptr != *ppAttributes )
		{
			( *ppAttributes )->Release();

			*ppAttributes = nullptr;
		}

		if(nullptr != m_pCommand)
		{
			SVCommandDataHolder::SVCommandAttributes* l_Attr = dynamic_cast< SVCommandDataHolder::SVCommandAttributes* >( m_pCommand.get() );

			if( nullptr != l_Attr )
			{
				CComPtr< ISVRemoteCommandData > l_CommandDataPtr;

				l_Status = l_CommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

				if( nullptr == l_CommandDataPtr.p )
				{
					if( S_OK == l_Status )
					{
						l_Status = E_FAIL;
					}
				}

				if( S_OK == l_Status )
				{
					// This has issues when using _ATL_DEBUG_INTERFACES...
					SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( l_CommandDataPtr.p );

					if( nullptr != l_pCommandData )
					{
						l_Status = l_Attr->GetAttributes( l_pCommandData->m_Data );
					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				if( S_OK == l_Status )
				{
					l_Status = l_CommandDataPtr.QueryInterface( ppAttributes );
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
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::SetAttributes(ISVRemoteCommandData* pAttributes)
{
	HRESULT l_Status = S_OK;

	if( nullptr != pAttributes )
	{
		if(nullptr != m_pCommand)
		{
			SVCommandDataHolder::SVCommandAttributes* l_Attr = dynamic_cast< SVCommandDataHolder::SVCommandAttributes* >( m_pCommand.get() );

			if( nullptr != l_Attr )
			{
				// This has issues when using _ATL_DEBUG_INTERFACES...
				SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( pAttributes );

				if( nullptr != l_pCommandData )
				{
					l_Status = l_Attr->SetAttributes( l_pCommandData->m_Data );
				}
				else
				{
					l_Status = E_INVALIDARG;
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
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::Execute()
{
	HRESULT l_Status = S_OK;

	if(nullptr != m_pCommand)
	{
		l_Status = m_pCommand->Execute();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::GetResults(ISVRemoteCommandData** ppResults)
{
	HRESULT l_Status = S_OK;

	if( nullptr != ppResults )
	{
		if( nullptr != *ppResults )
		{
			( *ppResults )->Release();

			*ppResults = nullptr;
		}

		if(nullptr != m_pCommand)
		{
			SVCommandDataHolder::SVCommandResults* l_Results = dynamic_cast< SVCommandDataHolder::SVCommandResults* >( m_pCommand.get() );

			if( nullptr != l_Results )
			{
				CComPtr< ISVRemoteCommandData > l_CommandDataPtr;

				l_Status = l_CommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

				if( nullptr == l_CommandDataPtr.p )
				{
					if( S_OK == l_Status )
					{
						l_Status = E_FAIL;
					}
				}

				if( S_OK == l_Status )
				{
					// This has issues when using _ATL_DEBUG_INTERFACES...
					SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( l_CommandDataPtr.p );

					if( nullptr != l_pCommandData )
					{
						l_Status = l_Results->GetResults( l_pCommandData->m_Data );
					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				if( S_OK == l_Status )
				{
					l_Status = l_CommandDataPtr.QueryInterface( ppResults );
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
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

