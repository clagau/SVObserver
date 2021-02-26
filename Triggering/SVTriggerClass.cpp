//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerClass
//* .File Name       : $Workfile:   SVTriggerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVDigitizerLoadLibraryClass.h"
#include "SVTriggerClass.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "Triggering/SVTriggerConstants.h"
#pragma endregion Includes

namespace SvTrig
{
SVTriggerClass::SVTriggerClass(LPCTSTR deviceName) : TriggerDevice(deviceName)
{
	std::string name{deviceName};
	if (0 == name.compare(0, strlen(SvTrig::SoftwareTriggerName), SvTrig::SoftwareTriggerName))
	{
		m_type = SvDef::TriggerType::SoftwareTrigger;
	}
	else if (0 == name.compare(0, strlen(SvTrig::CameraTriggerName), SvTrig::CameraTriggerName))
	{
		m_type = SvDef::TriggerType::CameraTrigger;
	}
}

void __stdcall SVTriggerClass::triggerCallback(const SvTrig::IntVariantMap& rTriggerData)
{
	SvTrig::SVTriggerInfoStruct triggerInfo;

	triggerInfo.bValid = true;
	triggerInfo.m_Data = rTriggerData;
	Notify(triggerInfo);
}

void SVTriggerClass::addAcquisitionTrigger(SVDigitizerLoadLibraryClass* pDllDigitizer, unsigned long triggerChannel)
{
	AcquisitionParameter acqParameter;
	acqParameter.m_pDllDigitizer = pDllDigitizer;
	acqParameter.m_triggerChannel = triggerChannel;
	auto it = std::find(m_acqTriggerParameters.begin(), m_acqTriggerParameters.end(), acqParameter);
	if(it == m_acqTriggerParameters.end())
	{
		m_acqTriggerParameters.emplace_back(acqParameter);
	}
}

void SVTriggerClass::clearAcquisitionTriggers()
{
	m_acqTriggerParameters.clear();
}
	
void SVTriggerClass::enableInternalTrigger() const
{
	///When software trigger then we need to enable the internal trigger
	for (const auto& rAcquisitionParameter : m_acqTriggerParameters)
	{
		if (nullptr != rAcquisitionParameter.m_pDllDigitizer)
		{
			rAcquisitionParameter.m_pDllDigitizer->InternalTriggerEnable(rAcquisitionParameter.m_triggerChannel);
		}
	}
}

HRESULT SVTriggerClass::RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback)
{
	HRESULT result = TriggerDevice::RegisterCallback(pPpqTriggerCallback);

	if( nullptr != m_pDLLTrigger )
	{
		if ( S_OK == result)
		{
			result = m_pDLLTrigger->Register(m_triggerChannel, std::bind(&SVTriggerClass::triggerCallback, this, std::placeholders::_1));
		}

		if ( S_OK != result )
		{
			m_pDLLTrigger->Unregister(m_triggerChannel);
		}
	}
	else
	{
		if (S_OK == result)
		{
			result = E_FAIL;
		}
	}

	return result;
}

HRESULT SVTriggerClass::UnregisterCallback()
{
	HRESULT result = (nullptr != m_pDLLTrigger) ? m_pDLLTrigger->Unregister(m_triggerChannel) : E_FAIL;

	HRESULT temp = TriggerDevice::UnregisterCallback();

	if( S_OK != temp && S_OK == result )
	{
		result = temp;
	}

	return result;
}

HRESULT SVTriggerClass::Start()
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pDLLTrigger )
	{
		l_hrOk = m_pDLLTrigger->Start( m_triggerChannel );
		m_currentObjectID = getStartObjectID();
		m_triggerIndex = 0L;
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = TriggerDevice::Start();
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::Stop()
{
	HRESULT l_hrOk = TriggerDevice::Stop();

	if ( nullptr != m_pDLLTrigger )
	{
		HRESULT l_Temp = m_pDLLTrigger->Stop( m_triggerChannel );

		if( S_OK != l_Temp && S_OK == l_hrOk )
		{
			l_hrOk = l_Temp;
		}
	}
  
	return l_hrOk;
}

void SVTriggerClass::preProcessTriggers(SvTrig::SVTriggerInfoStruct& rTriggerInfo)
{
	if (SvDef::TriggerType::SoftwareTrigger == m_type || SvDef::TriggerType::CameraTrigger == m_type)
	{
		++m_triggerIndex;
		if (getTriggerPerObjectID() < m_triggerIndex)
		{
			++m_currentObjectID;
			m_triggerIndex = 1L;
		}
		else if(0 == getStartObjectID())
		{
			m_currentObjectID = (0 == m_currentObjectID) ? 1 : m_currentObjectID;
		}
		rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::ObjectID] = _variant_t(m_currentObjectID);
		rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerIndex] = _variant_t(m_triggerIndex);
		rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerPerObjectID] = _variant_t(getTriggerPerObjectID());
	}
}

void SVTriggerClass::postProcessTriggers()
{
	for (const auto& rAcquisitionParameter : m_acqTriggerParameters)
	{
		if (nullptr != rAcquisitionParameter.m_pDllDigitizer)
		{
			rAcquisitionParameter.m_pDllDigitizer->InternalTrigger(rAcquisitionParameter.m_triggerChannel);
		}
	}
}
} //namespace SvTrig
