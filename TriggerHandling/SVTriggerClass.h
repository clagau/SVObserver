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
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#pragma endregion Includes

namespace SvTh
{
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

		#ifdef SV_LOG_STATUS_INFO
			std::vector<std::string> m_StatusLog;
		#endif
	};
} //namespace SvTh