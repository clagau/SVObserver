//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraTriggerClass
//* .File Name       : $Workfile:   SVCameraTriggerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:50:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVCameraTriggerClass.h"
#include "SVCameraTriggerData.h"
#include "TriggerBasics.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

namespace Seidenader { namespace TriggerHandling {

	SVCameraTriggerClass::SVCameraTriggerClass(LPCTSTR deviceName)
	: SVTriggerClass(deviceName)
	, m_pSoftwareTrigger(nullptr)
	{
	}

	SVCameraTriggerClass::~SVCameraTriggerClass()
	{
		Destroy();
	}

	void SVCameraTriggerClass::SetSoftwareTriggerDevice(SVSoftwareTriggerClass* pSoftwareTrigger)
	{
		m_pSoftwareTrigger = pSoftwareTrigger;
	}

	HRESULT CALLBACK SVCameraTriggerClass::SoftwareTriggerCompleteCallback(void* pvOwner, void* pvCaller, void* pvResponse)
	{
		HRESULT hr = S_OK;
		return hr;
	}

	HRESULT CALLBACK SVCameraTriggerClass::TriggerCallback(TriggerParameters triggerparams)
	{
		HRESULT hr = S_OK;
		if (nullptr != triggerparams.m_pOwner)
		{
			SVOResponseClass l_Response;

			SVCameraTriggerClass *pDevice = reinterpret_cast<SVCameraTriggerClass *>(triggerparams.m_pOwner);
			SVCameraTriggerData::NameVariantMap* pSettings = reinterpret_cast<SVCameraTriggerData::NameVariantMap*> (triggerparams.m_pData);

			_variant_t startVal;
			SVCameraTriggerData::NameVariantMap::const_iterator Iter( pSettings->end() );
			Iter = pSettings->find( _T("StartFrameTimestamp") );
			if( pSettings->end() != Iter )
			{
				startVal = Iter->second;
			}
			l_Response.Reset();
			l_Response.SetIsValid( TRUE );
			l_Response.SetIsComplete( TRUE );
			l_Response.SetStartTick(startVal);

			boost::any holder(*pSettings);
			l_Response.SetExtraData(holder);
			hr = pDevice->Notify( l_Response );


			#ifdef SV_LOG_STATUS_INFO
				SVString l_String;
				l_String.Format( _T( "FinishProcess %s - HR = 0x%X" ), pDevice->GetDeviceName(), hr );
				pDevice->m_StatusLog.push_back( l_String );
			#endif
		}
		return hr;
	}

	// Owner is typically SVTriggerObject...
	HRESULT SVCameraTriggerClass::RegisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller)
	{
		HRESULT hr = SVODeviceClass::RegisterCallback(pCallback, pvOwner, pvCaller);

		if (nullptr != m_pDLLTrigger)
		{
			TriggerDispatcher dispatcher (SVCameraTriggerClass::TriggerCallback, TriggerParameters(this));

			if (S_OK == hr)
			{
				hr = m_pDLLTrigger->Register(m_triggerchannel, dispatcher);
			}
			if (S_OK != hr)
			{
				m_pDLLTrigger->Unregister(m_triggerchannel, dispatcher);
			}
			if (S_OK == hr)
			{
				if (m_pSoftwareTrigger)
				{
					m_pSoftwareTrigger->RegisterCallback(SVCameraTriggerClass::SoftwareTriggerCompleteCallback, pvOwner, pvCaller);
				}
			}
		}
		else
		{
			hr = S_FALSE;
		}
		return hr;
	}

	HRESULT SVCameraTriggerClass::UnregisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller)
	{
		HRESULT hr = S_OK;

		if (S_OK != SVODeviceClass::UnregisterCallback(pCallback, pvOwner, pvCaller))
		{
			hr = S_FALSE;
		}
		if (m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->UnregisterCallback(SVCameraTriggerClass::SoftwareTriggerCompleteCallback, pvOwner, pvCaller);
		}
		if (nullptr != m_pDLLTrigger)
		{
			TriggerDispatcher dispatcher(SVCameraTriggerClass::TriggerCallback, TriggerParameters(this, nullptr));

			if (S_OK != m_pDLLTrigger->Unregister( m_triggerchannel, dispatcher))
			{
				hr = S_FALSE;
			}
		}
		else
		{
			hr = S_FALSE;
		}
		return hr;
	}

	HRESULT SVCameraTriggerClass::Destroy()
	{
		HRESULT hr = SVODeviceClass::Destroy();
	
		if (m_pSoftwareTrigger)
		{
			hr = m_pSoftwareTrigger->Destroy();
		}
		return hr;
	}

	HRESULT SVCameraTriggerClass::Start()
	{
		if (m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->Start();
		}
		return SVTriggerClass::Start();
	}

	HRESULT SVCameraTriggerClass::Stop()
	{
		if (m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->Stop();
		}
		return SVTriggerClass::Stop();
	}

	void SVCameraTriggerClass::RegisterAcquistionInitiator(SVAcquisitionInitiator& rFunc)
	{
		if (m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->RegisterAcquistionInitiator(rFunc);
		}
	}

	HRESULT SVCameraTriggerClass::EnableInternalTrigger()
	{
		if (m_pSoftwareTrigger)
		{
			return m_pSoftwareTrigger->EnableInternalTrigger();
		}
		return S_FALSE;
	}

	HRESULT SVCameraTriggerClass::FireAcquisitionTrigger()
	{
		if (m_pSoftwareTrigger)
		{
			return m_pSoftwareTrigger->FireAcquisitionTrigger();
		}
		return S_FALSE;
	}
		
} /* namespace TriggerHandling */ } /* namespace Seidenader */
