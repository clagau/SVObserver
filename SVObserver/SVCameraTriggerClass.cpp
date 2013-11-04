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
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVMaterialsLibrary/SVMaterials.h"

SVCameraTriggerClass::SVCameraTriggerClass(LPCTSTR deviceName)
: SVTriggerClass(deviceName)
, m_pSoftwareTrigger(NULL)
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

HRESULT CALLBACK SVCameraTriggerClass::TriggerCallback(void* p_pvOwner, void* p_pvData)
{
	HRESULT hr = S_OK;
	if (p_pvOwner)
	{
		SVOResponseClass l_Response;

		SVCameraTriggerClass *pDevice = reinterpret_cast<SVCameraTriggerClass *>(p_pvOwner);
		SVMaterials* pMaterials = reinterpret_cast<SVMaterials *>(p_pvData);

		_variant_t startVal;
		pMaterials->GetMaterial("StartFrameTimestamp", startVal);
		l_Response.Reset();
		l_Response.SetIsValid( TRUE );
		l_Response.SetIsComplete( TRUE );
		l_Response.SetStartTick(startVal);

		boost::any holder(*pMaterials);
		l_Response.SetExtraData(holder);
		hr = pDevice->Notify( l_Response );

		//TRACE( "Process Trigger Callback - %s\n", pDevice->GetDeviceName() );

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

	if (m_pDLLTrigger != NULL)
	{
		SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = SVCameraTriggerClass::TriggerCallback;
		l_Callback.m_pOwner = this;
		l_Callback.m_pData = NULL;

		if (hr == S_OK)
		{
			hr = m_pDLLTrigger->Register(m_ulHandle, l_Callback);
		}
		if (hr != S_OK)
		{
			m_pDLLTrigger->Unregister(m_ulHandle, l_Callback);
		}
		if (hr == S_OK)
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

	if (SVODeviceClass::UnregisterCallback(pCallback, pvOwner, pvCaller) != S_OK)
	{
		hr = S_FALSE;
	}
	if (m_pSoftwareTrigger)
	{
		m_pSoftwareTrigger->UnregisterCallback(SVCameraTriggerClass::SoftwareTriggerCompleteCallback, pvOwner, pvCaller);
	}
	if (m_pDLLTrigger != NULL)
	{
		SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = SVCameraTriggerClass::TriggerCallback;
		l_Callback.m_pOwner = this;
		l_Callback.m_pData = NULL;

		if (m_pDLLTrigger->Unregister( m_ulHandle, l_Callback) != S_OK)
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
		
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraTriggerClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:50:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jan 2013 10:59:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
