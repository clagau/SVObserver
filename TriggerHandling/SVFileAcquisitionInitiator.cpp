//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionInitiator
//* .File Name       : $Workfile:   SVFileAcquisitionInitiator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:35:46  $
//******************************************************************************
#include "stdafx.h"
#include "SVFileAcquisitionInitiator.h"

namespace Seidenader { namespace TriggerHandling {
	SVFileAcquisitionInitiator::SVFileAcquisitionInitiator()
	{
	}

	SVFileAcquisitionInitiator::~SVFileAcquisitionInitiator()
	{
	}

	SVFileAcquisitionInitiator::SVFileAcquisitionInitiator(SVAcquisitionInitiator & rAcquisitionInitiator)
	: m_acquisitionInitiator(rAcquisitionInitiator)
	{
	}

	void SVFileAcquisitionInitiator::Create( SVAcquisitionInitiator & rFunc )
	{
		m_acquisitionInitiator = rFunc;
	}

	void SVFileAcquisitionInitiator::Destroy()
	{
		m_acquisitionInitiator = SVAcquisitionInitiator ();
	}

	HRESULT SVFileAcquisitionInitiator::FireAcquisitionTrigger()
	{
		HRESULT hr = m_acquisitionInitiator.Exec();
		return hr;
	}

	HRESULT CALLBACK SVFileAcquisitionInitiator::TriggerCallback( SvTh::TriggerParameters triggerparams )
	{
		HRESULT hrOk = S_OK;

		if (nullptr != triggerparams.m_pOwner)
		{
			try
			{
				SVFileAcquisitionInitiator* pDevice = (SVFileAcquisitionInitiator *)(triggerparams.m_pOwner);
				hrOk = pDevice->FireAcquisitionTrigger();
			}
			catch ( ... )
			{
			}
		}
		return hrOk;
	}

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
