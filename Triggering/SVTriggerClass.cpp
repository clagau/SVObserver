//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVIOTriggerLoadLibraryClass.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVTriggerConstants.h"
#pragma endregion Includes

namespace SvTrig
{
void PostponedInternalTrigger(SVDigitizerLoadLibraryClass* pAcqusitionDll, unsigned long digitizerHandle, DWORD sleepDuration)
{
	if (nullptr != pAcqusitionDll)
	{
		Sleep(sleepDuration);
		pAcqusitionDll->InternalTrigger(digitizerHandle, nullptr);
	}
}

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

void __stdcall SVTriggerClass::triggerCallback(SvTrig::TriggerData&& triggerData)
{
	SvTrig::SVTriggerInfoStruct triggerInfo;

	triggerInfo.bValid = true;
	triggerInfo.m_Data = std::move(triggerData);
	Notify(triggerInfo);
}

void SVTriggerClass::addAcquisitionTrigger(AcquisitionParameter&& acqParameter)
{
	auto it = std::find(m_acqTriggerParameters.begin(), m_acqTriggerParameters.end(), acqParameter);
	if (it != m_acqTriggerParameters.end())
	{
		*it = acqParameter;
	}
	else
	{
		m_acqTriggerParameters.emplace_back(acqParameter);
	}
}

void SVTriggerClass::clearAcquisitionTriggers()
{
	m_acqTriggerParameters.clear();
}
	
HRESULT SVTriggerClass::RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback)
{
	HRESULT result = TriggerDevice::RegisterCallback(pPpqTriggerCallback);

	if( nullptr != m_pDLLTrigger )
	{
		if ( S_OK == result)
		{
			auto triggerFunctor = [this](SvTrig::TriggerData&& triggerData) {return triggerCallback(std::move(triggerData)); };
			result = m_pDLLTrigger->Register(m_triggerChannel, triggerFunctor);
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
		getObjectIDParameters().m_currentObjectID = getObjectIDParameters().m_startObjectID;
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

void SVTriggerClass::setPause(bool pauseState)
{
	m_pause = pauseState;
	if (nullptr != m_pDLLTrigger)
	{
		_variant_t value;
		value = m_pause;
		m_pDLLTrigger->SetParameterValue(m_triggerChannel, SVIOParameterEnum::TriggerPause, value);
	}
}

void SVTriggerClass::preProcessTriggers(SvTrig::SVTriggerInfoStruct& rTriggerInfo)
{
	if (SvDef::TriggerType::SoftwareTrigger == m_type || SvDef::TriggerType::CameraTrigger == m_type)
	{
		++m_triggerIndex;
		const SvTrig::ObjectIDParameters& rObjIDParam = getObjectIDParameters();
		if (rObjIDParam.m_triggerPerObjectID < m_triggerIndex)
		{
			++rObjIDParam.m_currentObjectID;
			m_triggerIndex = 1L;
		}
		else if(0 == rObjIDParam.m_startObjectID && 0 == rObjIDParam.m_currentObjectID)
		{
			rObjIDParam.m_currentObjectID = 1L;
		}
		rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::ObjectID] = _variant_t(static_cast<uint32_t> (rObjIDParam.m_currentObjectID));
		rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerIndex] = _variant_t(static_cast<uint8_t> (m_triggerIndex));
		rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerPerObjectID] = _variant_t(static_cast<uint8_t> (rObjIDParam.m_triggerPerObjectID));
		long objectIDOffset = (0 == rObjIDParam.m_startObjectID) ? 0L : rObjIDParam.m_startObjectID - 1;
		bool objectIDCountReached {rObjIDParam.m_triggerPerObjectID == m_triggerIndex && rObjIDParam.m_objectIDCount > 0 && (rObjIDParam.m_currentObjectID >= objectIDOffset + rObjIDParam.m_objectIDCount)};
		if (objectIDCountReached)
		{
				setPause(true);
		}
	}
	else if(SvDef::TriggerType::HardwareTrigger == m_type && getObjectIDParameters().m_triggerPerObjectID != static_cast<long> (rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerPerObjectID]))
	{
		//Set the trigger per objectID value according to what the PLC sends
		SvTrig::ObjectIDParameters objectIDParam = getObjectIDParameters();
		objectIDParam.m_triggerPerObjectID = static_cast<long> (rTriggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerPerObjectID]);
		setObjectIDParameters(objectIDParam);
	}
}

void SVTriggerClass::postProcessTriggers(DWORD sleepDuration, bool softwareTrigger, LPCTSTR pAcquisitionFile)
{
	for (const auto& rAcquisitionParameter : m_acqTriggerParameters)
	{
		bool active = rAcquisitionParameter.m_active || softwareTrigger;
		if (nullptr != rAcquisitionParameter.m_pDllDigitizer && active)
		{
			if (0 == sleepDuration)
			{
				rAcquisitionParameter.m_pDllDigitizer->InternalTrigger(rAcquisitionParameter.m_triggerChannel, pAcquisitionFile);
			}
			else
			{
				std::thread(PostponedInternalTrigger, rAcquisitionParameter.m_pDllDigitizer, rAcquisitionParameter.m_triggerChannel, sleepDuration).detach();
			}
		}
	}
}
} //namespace SvTrig
