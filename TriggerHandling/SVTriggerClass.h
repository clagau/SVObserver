//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerClass
//* .File Name       : $Workfile:   SVTriggerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOLibrary/SVODeviceClass.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

#pragma endregion Includes

namespace Seidenader { namespace TriggerHandling {

	class SVTriggerClass : public SVODeviceClass  
	{
	public:
		SVTriggerClass(LPCTSTR deviceName);
		virtual ~SVTriggerClass();

		virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, 
										  void *pvOwner, void *pvCaller ) override;
		virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback,
											void *pvOwner, void *pvCaller ) override;
		virtual HRESULT UnregisterAllCallbacks() override;

		virtual HRESULT Start() override;
		virtual HRESULT Stop() override;

		virtual HRESULT EnableInternalTrigger();

		int miChannelNumber;  // &&& var needs to be set...

		SVIOTriggerLoadLibraryClass *m_pDLLTrigger;
		unsigned long m_triggerchannel;

	protected:
		#ifdef SV_LOG_STATUS_INFO
			SVStatusDeque m_StatusLog;
		#endif
	};

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
