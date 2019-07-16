//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerClass
//* .File Name       : $Workfile:   SVSoftwareTriggerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:08:46  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerHandling/SVAcquisitionInitiator.h"
#pragma endregion Includes

namespace SvTi
{
	class SVSoftwareTriggerClass : public SvTh::SVTriggerClass
	{
	private:
		SvTh::SVAcquisitionInitiator m_acquisitionInitiator;

	public:
		SVSoftwareTriggerClass(LPCTSTR deviceName);
		virtual ~SVSoftwareTriggerClass();

		virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller ) override;
		virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller ) override;
	
		static HRESULT CALLBACK TriggerCallback(const SvTh::TriggerParameters& rTriggerData);
		static HRESULT CALLBACK TriggerCompleteCallback(const SvTh::TriggerParameters& rTriggerData);

		void RegisterAcquistionInitiator(SvTh::SVAcquisitionInitiator& rFunc);

		virtual HRESULT EnableInternalTrigger() override;
		HRESULT FireAcquisitionTrigger() const;
	};
} //namespace SvTi
