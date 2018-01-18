//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IODeviceBase.h
/// All Rights Reserved
//*****************************************************************************

/// contains classes needed by all trigger-generating DLLs
//******************************************************************************

#pragma once

#pragma region Includes
#include "TriggerHandling/TriggerDispatcherCollection.h"
#pragma endregion Includes

namespace SvTi
{
	/// the base class for all trigger-generating DLLs.
	/// not supposed to be instantiated itself
	class IODeviceBase
	{
	#pragma region constructor
	protected:
		IODeviceBase(){}
		virtual ~IODeviceBase(){}
	#pragma region constructor

	#pragma region public methods
	public:
		HRESULT AddDispatcher(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher);
		HRESULT RemoveDispatcher(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher);
		HRESULT RemoveAllDispatchers(unsigned long handle);
		HRESULT StartTrigger(unsigned long handle);
		HRESULT StopTrigger(unsigned long handle);
	#pragma endregion public methods

	#pragma region protected methods

	protected:
		virtual void lockIfRequired(){}
		virtual void unlockIfRequired(){}
		virtual void beforeStartTrigger(unsigned long handle){}
		virtual HRESULT afterStartTrigger(HRESULT hr){return hr;}
		virtual void beforeStopTrigger(unsigned long handle){}
		virtual HRESULT afterStopTrigger(HRESULT hr){return hr;}
	#pragma endregion protected methods

	#pragma region member variables
	protected:
		SvTh::TriggerDispatcherCollection m_TriggerDispatchers;
	#pragma endregion member variables

	};


} //namespace SvTi
