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

namespace SvTi
{
	SVFileAcquisitionInitiator::SVFileAcquisitionInitiator()
	{
	}

	SVFileAcquisitionInitiator::~SVFileAcquisitionInitiator()
	{
	}

	SVFileAcquisitionInitiator::SVFileAcquisitionInitiator(SvTh::SVAcquisitionInitiator & rAcquisitionInitiator)
	: m_acquisitionInitiator(rAcquisitionInitiator)
	{
	}

	void SVFileAcquisitionInitiator::Create(SvTh::SVAcquisitionInitiator & rFunc )
	{
		m_acquisitionInitiator = rFunc;
	}

	void SVFileAcquisitionInitiator::Destroy()
	{
		m_acquisitionInitiator = SvTh::SVAcquisitionInitiator ();
	}

	HRESULT SVFileAcquisitionInitiator::FireAcquisitionTrigger() const
	{
		HRESULT hr = m_acquisitionInitiator.Exec();
		return hr;
	}

	HRESULT CALLBACK SVFileAcquisitionInitiator::TriggerCallback(const SvTh::TriggerParameters& rTriggerData)
	{
		HRESULT hrOk = S_OK;

		if (nullptr != rTriggerData.m_pOwner)
		{
			try
			{
				const SVFileAcquisitionInitiator* pDevice = reinterpret_cast<const SVFileAcquisitionInitiator*> (rTriggerData.m_pOwner);
				hrOk = pDevice->FireAcquisitionTrigger();
			}
			catch ( ... )
			{
			}
		}
		return hrOk;
	}
} //namespace SvTi
