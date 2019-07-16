//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionInitiator
//* .File Name       : $Workfile:   SVFileAcquisitionInitiator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:35:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "TriggerHandling/SVAcquisitionInitiator.h"
#pragma region Includes

namespace SvTi
{
	class SVFileAcquisitionInitiator
	{
	private:
		SvTh::SVAcquisitionInitiator  m_acquisitionInitiator;

	public:
		SVFileAcquisitionInitiator();
		SVFileAcquisitionInitiator(SvTh::SVAcquisitionInitiator & rAcquisitionInitiator);
		~SVFileAcquisitionInitiator();

		void Create(SvTh::SVAcquisitionInitiator & rFunc );
		void Destroy();

		HRESULT FireAcquisitionTrigger() const;

		static HRESULT CALLBACK TriggerCallback(const SvTh::TriggerParameters& rTriggerData);
	};
} //namespace SvTi