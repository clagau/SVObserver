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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>

#include "TriggerBasics.h"
#include "SVTriggerClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader { namespace TriggerHandling {
	HRESULT CALLBACK TriggerCallbackSvtc(SvTh::TriggerParameters triggerparams)
	{
		HRESULT hrOk = S_OK;

		if ( nullptr != triggerparams.m_pOwner)
		{
			SVOResponseClass l_Response;

			SVTriggerClass *pDevice = (SVTriggerClass *)(triggerparams.m_pOwner);

			l_Response.Reset();

			l_Response.SetIsValid( TRUE );
			l_Response.SetIsComplete( TRUE );

			HRESULT hr = pDevice->Notify( l_Response );

			#ifdef SV_LOG_STATUS_INFO
				SVString Text.Format( _T( "FinishProcess %s - HR = 0x%X" ), pDevice->GetDeviceName(), hr );

				pDevice->m_StatusLog.push_back( Text );
			#endif
		}

		return hrOk;
	}

	SVTriggerClass::SVTriggerClass(LPCTSTR deviceName)
	: SVODeviceClass(deviceName)
	, m_pDLLTrigger(nullptr)
	, m_triggerchannel(0)
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

		SvTh::TriggerDispatcher dispatcher(TriggerCallbackSvtc,SvTh::TriggerParameters(this, pvOwner));

		l_hrOk = SVODeviceClass::RegisterCallback( pCallback, pvOwner, pvCaller );

		if( nullptr != m_pDLLTrigger )
		{
			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_pDLLTrigger->Register( m_triggerchannel, dispatcher );
			}

			if ( S_OK != l_hrOk )
			{
				m_pDLLTrigger->Unregister( m_triggerchannel, dispatcher );
			}
		}
		else
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = E_FAIL;
			}
		}

		return l_hrOk;
	}

	HRESULT SVTriggerClass::UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
	{
		HRESULT l_hrOk = S_OK;

		if ( nullptr != m_pDLLTrigger )
		{
			SvTh::TriggerDispatcher dispatcher(TriggerCallbackSvtc, SvTh::TriggerParameters(this, pvOwner));

			l_hrOk = m_pDLLTrigger->Unregister( m_triggerchannel, dispatcher );
		}
		else
		{
			l_hrOk = E_FAIL;
		}

		HRESULT l_Temp = SVODeviceClass::UnregisterCallback( pCallback, pvOwner, pvCaller );

		if( S_OK != l_Temp && S_OK == l_hrOk )
		{
			l_hrOk = l_Temp;
		}

		return l_hrOk;
	}

	HRESULT SVTriggerClass::UnregisterAllCallbacks()
	{
		HRESULT l_hrOk = SVODeviceClass::UnregisterAllCallbacks();

		if ( nullptr != m_pDLLTrigger )
		{
			HRESULT l_Temp = m_pDLLTrigger->UnregisterAll( m_triggerchannel );
		
			if( S_OK != l_Temp && S_OK == l_hrOk )
			{
				l_hrOk = l_Temp;
			}
		}
		else
		{
			if( S_OK == l_hrOk )
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

		if ( nullptr != m_pDLLTrigger )
		{
			l_hrOk = m_pDLLTrigger->Start( m_triggerchannel );
		}
		else
		{
			l_hrOk = E_FAIL;
		}

		if ( S_OK == l_hrOk )
		{
			l_hrOk = SVODeviceClass::Start();
		}

		return l_hrOk;
	}

	HRESULT SVTriggerClass::Stop()
	{
		HRESULT l_hrOk = SVODeviceClass::Stop();

		if ( nullptr != m_pDLLTrigger )
		{
			HRESULT l_Temp = m_pDLLTrigger->Stop( m_triggerchannel );

			if( S_OK != l_Temp && S_OK == l_hrOk )
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
} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
