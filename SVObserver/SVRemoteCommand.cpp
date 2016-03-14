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

#include "stdafx.h"
#include "SVObserver.h"
#include "SVObserver_i.h"
#include "SVRemoteCommand.h"
#include "SVCommandLibrary/SVCommandLibraryEnums.h"
#include "SVRemoteCommandData.h"

SVRemoteCommand::SVRemoteCommand()
: m_Command()
{
}

SVRemoteCommand::~SVRemoteCommand()
{
	m_Command.clear();
}

void SVRemoteCommand::FinalRelease()
{
	m_Command.clear();
}

void SVRemoteCommand::SetCommand( SVCommandTemplatePtr p_Command )
{
	m_Command = p_Command;
}

STDMETHODIMP SVRemoteCommand::GetCommandType(long* p_CommandType)
{
	HRESULT l_Status = S_OK;

	if( p_CommandType != NULL )
	{
		if( ! m_Command.empty() )
		{
			*p_CommandType = m_Command->GetCommandType();
		}
		else
		{
			*p_CommandType = SV_COMMAND_TYPE_UNKNOWN;

			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::ConstructCommandData( ISVRemoteCommandData **p_ppCommandData )
{
	HRESULT l_Status = S_OK;

	if( p_ppCommandData != NULL )
	{
		if( *p_ppCommandData != NULL )
		{
			( *p_ppCommandData )->Release();

			*p_ppCommandData = NULL;
		}

		CComPtr< ISVRemoteCommandData > l_RemoteCommandDataPtr;

		l_Status = l_RemoteCommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

		if( l_RemoteCommandDataPtr.p == NULL )
		{
			if( l_Status == S_OK )
			{
				l_Status = E_FAIL;
			}
		}

		if( l_Status == S_OK )
		{
			l_Status = l_RemoteCommandDataPtr.QueryInterface( p_ppCommandData );
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::GetAttributes(ISVRemoteCommandData** p_ppAttributes)
{
	HRESULT l_Status = S_OK;

	if( p_ppAttributes != NULL )
	{
		if( *p_ppAttributes != NULL )
		{
			( *p_ppAttributes )->Release();

			*p_ppAttributes = NULL;
		}

		if( ! m_Command.empty() )
		{
			SVCommandDataHolder::SVCommandAttributes* l_Attr = dynamic_cast< SVCommandDataHolder::SVCommandAttributes* >( m_Command.get() );

			if( l_Attr != NULL )
			{
				CComPtr< ISVRemoteCommandData > l_CommandDataPtr;

				l_Status = l_CommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

				if( l_CommandDataPtr.p == NULL )
				{
					if( l_Status == S_OK )
					{
						l_Status = E_FAIL;
					}
				}

				if( l_Status == S_OK )
				{
					// This has issues when using _ATL_DEBUG_INTERFACES...
					SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( l_CommandDataPtr.p );

					if( l_pCommandData != NULL )
					{
						l_Status = l_Attr->GetAttributes( l_pCommandData->m_Data );
					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				if( l_Status == S_OK )
				{
					l_Status = l_CommandDataPtr.QueryInterface( p_ppAttributes );
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

STDMETHODIMP SVRemoteCommand::SetAttributes(ISVRemoteCommandData* p_pAttributes)
{
	HRESULT l_Status = S_OK;

	if( p_pAttributes != NULL )
	{
		if( ! m_Command.empty() )
		{
			SVCommandDataHolder::SVCommandAttributes* l_Attr = dynamic_cast< SVCommandDataHolder::SVCommandAttributes* >( m_Command.get() );

			if( l_Attr != NULL )
			{
				// This has issues when using _ATL_DEBUG_INTERFACES...
				SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( p_pAttributes );

				if( l_pCommandData != NULL )
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

	if( ! m_Command.empty() )
	{
		l_Status = m_Command->Execute();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommand::GetResults(ISVRemoteCommandData** p_ppResults)
{
	HRESULT l_Status = S_OK;

	if( p_ppResults != NULL )
	{
		if( *p_ppResults != NULL )
		{
			( *p_ppResults )->Release();

			*p_ppResults = NULL;
		}

		if( ! m_Command.empty() )
		{
			SVCommandDataHolder::SVCommandResults* l_Results = dynamic_cast< SVCommandDataHolder::SVCommandResults* >( m_Command.get() );

			if( l_Results != NULL )
			{
				CComPtr< ISVRemoteCommandData > l_CommandDataPtr;

				l_Status = l_CommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

				if( l_CommandDataPtr.p == NULL )
				{
					if( l_Status == S_OK )
					{
						l_Status = E_FAIL;
					}
				}

				if( l_Status == S_OK )
				{
					// This has issues when using _ATL_DEBUG_INTERFACES...
					SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( l_CommandDataPtr.p );

					if( l_pCommandData != NULL )
					{
						l_Status = l_Results->GetResults( l_pCommandData->m_Data );
					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				if( l_Status == S_OK )
				{
					l_Status = l_CommandDataPtr.QueryInterface( p_ppResults );
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

