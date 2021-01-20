//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IODeviceBase.h
/// All Rights Reserved
//*****************************************************************************

/// contains classes needed by all trigger-generating DLLs
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTriggerInfoStruct.h"
#pragma endregion Includes

namespace SvTrig
{
/// the base class for all trigger-generating DLLs.
/// not supposed to be instantiated itself
class IODeviceBase
{
#pragma region constructor
protected:
	IODeviceBase() = default;
	virtual ~IODeviceBase() = default;
#pragma region constructor

#pragma region public methods
public:
	HRESULT RegisterCallback(unsigned long triggerIndex, TriggerCallBack pTriggerCallback);
	HRESULT UnRegisterCallback(unsigned long triggerIndex);
	HRESULT StartTrigger(unsigned long triggerIndex);
	HRESULT StopTrigger(unsigned long triggerIndex);
#pragma endregion public methods

#pragma region protected methods

protected:
	virtual void beforeStartTrigger(unsigned long ){}
	virtual HRESULT afterStartTrigger(){return S_OK;}
	virtual void beforeStopTrigger(unsigned long ){}
	virtual HRESULT afterStopTrigger(){return S_OK;}
#pragma endregion protected methods

#pragma region member variables
protected:
	mutable std::mutex m_protectIO;		///Note do not call any IO device calls when this is set, can cause deadlock due to callback function!
	std::map<unsigned long, TriggerCallBack> m_triggerCallbackMap;
	std::atomic_bool m_moduleReady {false};
#pragma endregion member variables

};
} //namespace SvTrig
