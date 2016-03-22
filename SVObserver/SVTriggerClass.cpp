//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerClass
//* .File Name       : $Workfile:   SVTriggerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:56  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVTriggerClass.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

HRESULT CALLBACK SVTriggerClass::SVTriggerCallback( void *p_pvOwner, void *p_pvData )
{
	HRESULT hrOk = S_OK;

	if ( p_pvOwner )
	{
		SVOResponseClass l_Response;

		SVTriggerClass *pDevice = (SVTriggerClass *)p_pvOwner;

		l_Response.Reset();
		l_Response.SetIsValid( TRUE );
		l_Response.SetIsComplete( TRUE );

		HRESULT hr = pDevice->Notify( l_Response );

		//TRACE( "Process Trigger Callback - %s\n", pDevice->GetDeviceName() );

		#ifdef SV_LOG_STATUS_INFO
			SVString l_String;

			l_String.Format( _T( "FinishProcess %s - HR = 0x%X" ), pDevice->GetDeviceName(), hr );

			pDevice->m_StatusLog.push_back( l_String );
		#endif
	}

	return hrOk;
}

SVTriggerClass::SVTriggerClass(LPCTSTR deviceName)
: SVODeviceClass(deviceName)
, m_pDLLTrigger(NULL)
, m_ulHandle(NULL)
, miChannelNumber(-1)
{
	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.Create();
	#endif
}

SVTriggerClass::~SVTriggerClass()
{
}

HRESULT SVTriggerClass::RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
{
	HRESULT l_hrOk = S_OK;

	SVCallbackStruct l_Callback;

	l_Callback.m_pCallback = SVTriggerClass::SVTriggerCallback;
	l_Callback.m_pOwner = this;
	l_Callback.m_pData = pvOwner;

	l_hrOk = SVODeviceClass::RegisterCallback( pCallback, pvOwner, pvCaller );

	if( m_pDLLTrigger != NULL )
	{
		if ( l_hrOk == S_OK )
		{
			l_hrOk = m_pDLLTrigger->Register( m_ulHandle, l_Callback );
		}

		if ( l_hrOk != S_OK )
		{
			m_pDLLTrigger->Unregister( m_ulHandle, l_Callback );
		}
	}
	else
	{
		if( l_hrOk == S_OK )
		{
			l_hrOk = E_FAIL;
		}
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
{
	HRESULT l_hrOk = S_OK;

	if ( m_pDLLTrigger != NULL )
	{
		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = SVTriggerCallback;
		l_Callback.m_pOwner = this;
		l_Callback.m_pData = pvOwner;

		l_hrOk = m_pDLLTrigger->Unregister( m_ulHandle, l_Callback );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	HRESULT l_Temp = SVODeviceClass::UnregisterCallback( pCallback, pvOwner, pvCaller );

	if( l_Temp != S_OK && l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::UnregisterAllCallbacks()
{
	HRESULT l_hrOk = SVODeviceClass::UnregisterAllCallbacks();

	if ( m_pDLLTrigger != NULL )
	{
		HRESULT l_Temp = m_pDLLTrigger->UnregisterAll( m_ulHandle );
		
		if( l_Temp != S_OK && l_hrOk == S_OK )
		{
			l_hrOk = l_Temp;
		}
	}
	else
	{
		if( l_hrOk == S_OK )
		{
			l_hrOk = E_FAIL;
		}
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::Start()
{
	HRESULT l_hrOk = S_FALSE;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.clear();
	#endif

	if ( m_pDLLTrigger != NULL )
	{
		l_hrOk = m_pDLLTrigger->Start( m_ulHandle );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = SVODeviceClass::Start();
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::Stop()
{
	HRESULT l_hrOk = SVODeviceClass::Stop();

	if ( m_pDLLTrigger != NULL )
	{
		HRESULT l_Temp = m_pDLLTrigger->Stop( m_ulHandle );

		if( l_Temp != S_OK && l_hrOk == S_OK )
		{
			l_hrOk = l_Temp;
		}
	}
  
	#ifdef SV_LOG_STATUS_INFO
		SVString l_FileName;

		l_FileName.Format( _T( "C:\\SVObserver\\%s.log" ), GetDeviceName() );

		std::fstream l_Stream( l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_Stream.is_open() )
		{
			for( int i = 0; i < m_StatusLog.GetCount(); ++i )
			{
				SVString l_String;

				m_StatusLog.GetAt( i, &l_String );

				l_Stream << l_String.ToString() << std::endl;
			}

			l_Stream.close();

			m_StatusLog.clear();
		}
	#endif

	return l_hrOk;
}

HRESULT SVTriggerClass::EnableInternalTrigger()
{
	return E_NOTIMPL;
}

