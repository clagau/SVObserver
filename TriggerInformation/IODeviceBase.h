//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IODeviceBase.h
/// All Rights Reserved
//*****************************************************************************

/// contains classes needed by all trigger-generating DLLs
//******************************************************************************

#pragma once

#pragma region Includes
#include "TriggerHandling/CallbackStructContainers.h"
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
		HRESULT AddTriggerCallback(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher);
		HRESULT RemoveTriggerCallback(unsigned long handle, SvTh::SVTriggerCallbackPtr pCallback);
		HRESULT RemoveAllTriggerCallbacks(unsigned long handle);
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
		SvTh::TriggerDispatcherMap m_triggerDispatcherMap;
	#pragma endregion member variables

	};


	/// a functor that sets the trigger callback pointer in a TriggerDispatcher struct
	class TriggerFinder : public std::binary_function<SvTh::TriggerDispatcher, SvTh::SVTriggerCallbackPtr, bool>
	{
	public:
		bool operator()(const SvTh::TriggerDispatcher& rDispatcher, const SvTh::SVTriggerCallbackPtr& pCallback) const
		{
			return (rDispatcher.getCallback()== pCallback);
		}
	};
} //namespace SvTi