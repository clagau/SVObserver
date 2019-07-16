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
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTh
{
	HRESULT CALLBACK TriggerCallbackSvtc(const SvTh::TriggerParameters& rTriggerData)
	{
		HRESULT hrOk = S_OK;

		if ( nullptr != rTriggerData.m_pOwner)
		{
			SVOResponseClass response;

			const SVTriggerClass *pDevice = reinterpret_cast<const SVTriggerClass*> (rTriggerData.m_pOwner);

			response.reset();

			response.setIsValid(true);
			response.setIsComplete(true);
			response.setData(rTriggerData.m_Data);

			HRESULT hr = pDevice->Notify( response );

			#ifdef SV_LOG_STATUS_INFO
				std::string LogEntry = SvUl::Format( _T( "FinishProcess %s - HR = 0x%X" ), pDevice->GetDeviceName(), hr );

				pDevice->m_StatusLog.push_back( LogEntry );
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
	}

	SVTriggerClass::~SVTriggerClass()
	{
	}

	HRESULT SVTriggerClass::RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
	{
		HRESULT l_hrOk = S_OK;

		TriggerDispatcher dispatcher(TriggerCallbackSvtc, TriggerParameters(this));

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
			TriggerDispatcher dispatcher(TriggerCallbackSvtc, TriggerParameters(this));

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
		std::string FileName = SvUl::Format(_T("C:\\SVObserver\\%s.log"), GetDeviceName());

			std::fstream FileStream( FileName.c_str(), std::ios_base::trunc | std::ios_base::out );

			if( FileStream.is_open() )
			{
				for( auto const& rEntry : m_StatusLog )
				{
					FileStream << rEntry.c_str() << std::endl;
				}

				FileStream.close();

				m_StatusLog.clear();
			}
		#endif

		return l_hrOk;
	}

	HRESULT SVTriggerClass::EnableInternalTrigger()
	{
		return E_NOTIMPL;
	}
} //namespace SvTh
