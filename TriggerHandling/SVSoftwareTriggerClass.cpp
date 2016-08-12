//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerClass
//* .File Name       : $Workfile:   SVSoftwareTriggerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:08:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVSoftwareTriggerClass.h"
#include "TriggerBasics.h"
#include "SVIOTriggerLoadLibraryClass.h"

namespace Seidenader { namespace TriggerHandling {

	SVSoftwareTriggerClass::SVSoftwareTriggerClass(LPCTSTR deviceName)
	: SVTriggerClass(deviceName)
	{
	}

	SVSoftwareTriggerClass::~SVSoftwareTriggerClass()
	{
	}

	HRESULT CALLBACK SVSoftwareTriggerClass::TriggerCallback(TriggerParameters triggerparams)
	{
		HRESULT hrOk = S_OK;

		if ( nullptr != triggerparams.m_pOwner )
		{
			try
			{
				SVSoftwareTriggerClass* pDevice = (SVSoftwareTriggerClass *)(triggerparams.m_pOwner);
				hrOk = pDevice->FireAcquisitionTrigger();
			}
			catch ( ... )
			{
			}
		}
		return hrOk;
	}

	HRESULT CALLBACK SVSoftwareTriggerClass::TriggerCompleteCallback(TriggerParameters triggerparams)
	{
		HRESULT hrOk = S_OK;

		if ( nullptr != triggerparams.m_pOwner )
		{
			try
			{
				SVSoftwareTriggerClass* pDevice = (SVSoftwareTriggerClass *)(triggerparams.m_pOwner);

				SVOResponseClass l_Response;

				l_Response.Reset();
				l_Response.SetIsValid( TRUE );
				l_Response.SetIsComplete( TRUE );

				pDevice->Notify( l_Response );
			}
			catch ( ... )
			{
			}
		}
		return hrOk;
	}

	HRESULT SVSoftwareTriggerClass::RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
	{
		HRESULT l_hrOk = S_OK;

		l_hrOk = SVODeviceClass::RegisterCallback( pCallback, pvOwner, pvCaller );

		if ( nullptr != m_pDLLTrigger )
		{
			TriggerCallbackInformation localCallback;
			localCallback.m_pCallback = SVSoftwareTriggerClass::TriggerCallback;
			localCallback.m_TriggerParameters = TriggerParameters(this, pvOwner);

			TriggerCallbackInformation triggerCallbackInfo;
			triggerCallbackInfo.m_pCallback = SVSoftwareTriggerClass::TriggerCompleteCallback;
			triggerCallbackInfo.m_TriggerParameters = TriggerParameters(this, pvOwner);

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_pDLLTrigger->Register( m_ulHandle, localCallback );
			}

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_acquisitionInitiator.RegisterCallback( triggerCallbackInfo );
			}

			if ( S_OK != l_hrOk )
			{
				m_pDLLTrigger->Unregister( m_ulHandle, localCallback );
				m_acquisitionInitiator.UnRegisterCallback(triggerCallbackInfo);
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		return l_hrOk;
	}

	HRESULT SVSoftwareTriggerClass::UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
	{
		HRESULT l_hrOk = S_OK;

		if ( S_OK != SVODeviceClass::UnregisterCallback( pCallback, pvOwner, pvCaller ) )
		{
			l_hrOk = S_FALSE;
		}

		if ( nullptr != m_pDLLTrigger )
		{
			TriggerCallbackInformation localCallback;

			localCallback.m_pCallback = SVSoftwareTriggerClass::TriggerCallback;
			localCallback.m_TriggerParameters = TriggerParameters(this, pvOwner);

			TriggerCallbackInformation triggerCallbackInfo;
			triggerCallbackInfo.m_pCallback = SVSoftwareTriggerClass::TriggerCompleteCallback;
			localCallback.m_TriggerParameters = TriggerParameters(this, pvOwner);

			if ( S_OK != m_pDLLTrigger->Unregister( m_ulHandle, localCallback ) )
			{
				l_hrOk = S_FALSE;
			}
			m_acquisitionInitiator.UnRegisterCallback(triggerCallbackInfo);
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		return l_hrOk;
	}

	void SVSoftwareTriggerClass::RegisterAcquistionInitiator( SVAcquisitionInitiator& rFunc)
	{
		m_acquisitionInitiator = rFunc;
	}

	HRESULT SVSoftwareTriggerClass::EnableInternalTrigger()
	{
		return m_acquisitionInitiator.EnableInternalTrigger();
	}

	HRESULT SVSoftwareTriggerClass::FireAcquisitionTrigger()
	{
		return m_acquisitionInitiator.Exec();
	}

} /* namespace TriggerHandling */ } /* namespace Seidenader */
