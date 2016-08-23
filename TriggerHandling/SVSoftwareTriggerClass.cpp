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
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

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

		TriggerParameters tp(this, pvOwner);

		if ( nullptr != m_pDLLTrigger )
		{
			TriggerDispatcher localCallback(SVSoftwareTriggerClass::TriggerCallback, tp);

			TriggerDispatcher dispatcher(SVSoftwareTriggerClass::TriggerCompleteCallback, tp);

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_pDLLTrigger->Register( m_triggerchannel, localCallback );
			}

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_acquisitionInitiator.RegisterCallback( dispatcher );
			}

			if ( S_OK != l_hrOk )
			{
				m_pDLLTrigger->Unregister( m_triggerchannel, localCallback );
				m_acquisitionInitiator.UnRegisterCallback(dispatcher);
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
			TriggerDispatcher localCallback(SVSoftwareTriggerClass::TriggerCallback, TriggerParameters(this, pvOwner));

			TriggerDispatcher dispatcher(SVSoftwareTriggerClass::TriggerCompleteCallback, TriggerParameters(this, pvOwner));

			if ( S_OK != m_pDLLTrigger->Unregister( m_triggerchannel, localCallback ) )
			{
				l_hrOk = S_FALSE;
			}
			m_acquisitionInitiator.UnRegisterCallback(dispatcher);
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
