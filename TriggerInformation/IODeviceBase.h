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
		HRESULT AddDispatcher(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher);
		HRESULT RemoveDispatcher(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher);
		HRESULT RemoveAllDispatchers(unsigned long triggerIndex);
		HRESULT StartTrigger(unsigned long triggerIndex);
		HRESULT StopTrigger(unsigned long triggerIndex);
	#pragma endregion public methods

	#pragma region protected methods

	protected:
		virtual void beforeStartTrigger(unsigned long triggerIndex){}
		virtual HRESULT afterStartTrigger(HRESULT hr){return hr;}
		virtual void beforeStopTrigger(unsigned long triggerIndex){}
		virtual HRESULT afterStopTrigger(HRESULT hr){return hr;}
	#pragma endregion protected methods

	#pragma region member variables
	protected:
		SvTh::TriggerDispatcherCollection m_TriggerDispatchers;
		std::mutex m_protectIO;		///Note do not call any IO device calls when this is set, can cause deadlock due to callback function!
		std::atomic_bool m_moduleReady {false};
#pragma endregion member variables

	};


} //namespace SvTi
