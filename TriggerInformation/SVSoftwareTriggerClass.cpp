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
#include "TriggerHandling/TriggerBasics.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

namespace SvTi
{
	SVSoftwareTriggerClass::SVSoftwareTriggerClass(LPCTSTR deviceName)
	: SVTriggerClass(deviceName)
	{
	}

	SVSoftwareTriggerClass::~SVSoftwareTriggerClass()
	{
	}

	HRESULT CALLBACK SVSoftwareTriggerClass::TriggerCallback(SvTh::TriggerParameters triggerparams)
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

	HRESULT CALLBACK SVSoftwareTriggerClass::TriggerCompleteCallback(SvTh::TriggerParameters triggerparams)
	{
		HRESULT hrOk = S_OK;

		if ( nullptr != triggerparams.m_pOwner )
		{
			try
			{
				SVSoftwareTriggerClass* pDevice = (SVSoftwareTriggerClass *)(triggerparams.m_pOwner);

				SVOResponseClass response;

				response.reset();
				response.setIsValid(true);
				response.setIsComplete(true);

				pDevice->Notify( response );
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

		SvTh::TriggerParameters tp(this, pvOwner);

		if ( nullptr != m_pDLLTrigger )
		{
			SvTh::TriggerDispatcher localCallback(SVSoftwareTriggerClass::TriggerCallback, tp);

			SvTh::TriggerDispatcher dispatcher(SVSoftwareTriggerClass::TriggerCompleteCallback, tp);

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
			SvTh::TriggerDispatcher localCallback(SVSoftwareTriggerClass::TriggerCallback, SvTh::TriggerParameters(this, pvOwner));

			SvTh::TriggerDispatcher dispatcher(SVSoftwareTriggerClass::TriggerCompleteCallback, SvTh::TriggerParameters(this, pvOwner));

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

	void SVSoftwareTriggerClass::RegisterAcquistionInitiator(SvTh::SVAcquisitionInitiator& rFunc)
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

} //namespace SvTi
