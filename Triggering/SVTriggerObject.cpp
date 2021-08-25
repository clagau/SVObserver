//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerObject
//* .File Name       : $Workfile:   SVTriggerObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:44:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVTriggerClass.h"
#include "SVTriggerConstants.h"
#include "SVTriggerInfoStruct.h"
#include "SVTriggerObject.h"
#include "SVIOTriggerLoadLibraryClass.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

namespace SvTrig
{
	SVTriggerObject::SVTriggerObject( LPCSTR ObjectName )
	: SVObjectClass( ObjectName )
	, m_timerPeriod(TimerPeriod) 
	{
	}

	SVTriggerObject::SVTriggerObject( SVObjectClass* POwner, int StringResourceID )
	: SVObjectClass( POwner, StringResourceID )
	, m_timerPeriod(TimerPeriod) 
	{
	}

	SVTriggerObject::~SVTriggerObject()
	{
		Destroy();
	}

	bool SVTriggerObject::Create(SvTrig::SVTriggerClass* pTrigger, SvTrig::SVTriggerClass* pSoftwareTrigger)
	{
		bool result{ false };

		if ( nullptr != pTrigger )
		{
			m_pMainTrigger = pTrigger;
			m_pCurrentTrigger = m_pMainTrigger;
			m_pMainTrigger->setTriggerCount();
			m_ObjectTypeInfo.m_ObjectType = SvPb::SVTriggerObjectType;
			result = true;
		}
		m_pSoftwareTrigger = pSoftwareTrigger;
		if (nullptr != m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->setTriggerCount();
		}
		return result;
	}

	bool SVTriggerObject::Destroy()
	{
		if (nullptr != m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->Destroy();
			m_pSoftwareTrigger = nullptr;
		}
		if ( nullptr != m_pMainTrigger )
		{
			m_pMainTrigger->Destroy();
			m_pMainTrigger = nullptr;
			return true;
		}
		return false;
	}

	bool SVTriggerObject::CanGoOnline(bool isTestMode)
	{
		if (isTestMode && nullptr != m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->clearAcquisitionTriggers();
			if (nullptr != m_pMainTrigger)
			{
				for (const auto& rAcquisitionParam : m_pMainTrigger->getAcquisitionTriggers())
				{
					AcquisitionParameter acquisitionParam{ rAcquisitionParam };
					acquisitionParam.m_active = true;
					m_pSoftwareTrigger->addAcquisitionTrigger(std::move(acquisitionParam));
				}
				m_pSoftwareTrigger->setObjectIDParameters(m_pMainTrigger->getObjectIDParameters());
				m_pSoftwareTrigger->setTriggerCount(m_pMainTrigger->getTriggerCount());
			}
			m_pCurrentTrigger = m_pSoftwareTrigger;
			SetSoftwareTriggerPeriod(m_timerPeriod, true);
		}
		///This needs to be done before the cameras are started
		if (nullptr != m_pCurrentTrigger)
		{
			m_pCurrentTrigger->setTriggerType(isTestMode);
			m_pCurrentTrigger->setPause(false);
		}
		return nullptr != m_pCurrentTrigger;
	}

	bool SVTriggerObject::GoOnline()
	{
		return (nullptr != m_pCurrentTrigger);
	}

	bool SVTriggerObject::GoOffline()
	{
		bool result{ false };
		if ( nullptr != m_pCurrentTrigger)
		{
			result = (S_OK == m_pCurrentTrigger->Stop());
			if (m_pCurrentTrigger == m_pSoftwareTrigger)
			{
				//The trigger count from the software triggers need to be set in the main trigger
				m_pMainTrigger->setTriggerCount(m_pSoftwareTrigger->getTriggerCount());
			}
			m_pCurrentTrigger = m_pMainTrigger;
		}
		return result;
	}

	bool SVTriggerObject::Start()
	{ 
		return (nullptr != m_pCurrentTrigger) && (S_OK == m_pCurrentTrigger->Start());
	}

	bool SVTriggerObject::RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback)
	{
		if (nullptr != m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->RegisterCallback(pPpqTriggerCallback);
		}
		if (nullptr != m_pMainTrigger)
		{
			return S_OK == m_pMainTrigger->RegisterCallback(pPpqTriggerCallback);
		}
		return false;
	}

	bool SVTriggerObject::UnregisterCallback()
	{
		if (nullptr != m_pSoftwareTrigger)
		{
			m_pSoftwareTrigger->UnregisterCallback();
		}
		if (nullptr != m_pMainTrigger)
		{
			return S_OK == m_pMainTrigger->UnregisterCallback();
		}
		return false;
	}

	const ObjectIDParameters& SVTriggerObject::getObjectIDParameters() const
	{
		static ObjectIDParameters emptyParams {};
		return (nullptr != m_pCurrentTrigger) ? m_pCurrentTrigger->getObjectIDParameters() : emptyParams; 
	}

	void SVTriggerObject::setObjectIDParameters(const ObjectIDParameters& rObjectIDParams)
	{
		if (nullptr != m_pMainTrigger)
		{
			m_pMainTrigger->setObjectIDParameters(rObjectIDParams);
		}
	}

	void SVTriggerObject::Fire(double triggerTime)
	{
		if (nullptr != m_pCurrentTrigger)
		{
			SvTrig::SVTriggerInfoStruct triggerInfo;
			if (0.0 == triggerTime)
			{
				triggerInfo.m_Data[SvTrig::TriggerDataEnum::TimeStamp] = _variant_t(SvUl::GetTimeStamp());
				triggerInfo.m_Data[SvTrig::TriggerDataEnum::SoftwareTrigger] = true;
			}
			else
			{
				triggerInfo.m_Data[SvTrig::TriggerDataEnum::TimeStamp] = _variant_t(triggerTime);
			}
			triggerInfo.bValid = true;
			///Trigger channel 0 based
			triggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerChannel] = _variant_t(m_pCurrentTrigger->getTriggerChannel());
			m_pCurrentTrigger->Notify(triggerInfo);
		}
	}

	long SVTriggerObject::GetSoftwareTriggerPeriod() const
	{
		return m_timerPeriod;
	}

	void SVTriggerObject::SetSoftwareTriggerPeriod(long period, bool setTimer /*= false*/)
	{
		m_timerPeriod = period;

		if( setTimer && nullptr != m_pCurrentTrigger)
		{
			SVIOTriggerLoadLibraryClass* pDllTrigger = m_pCurrentTrigger->getDLLTrigger();

			if( nullptr != pDllTrigger )
			{
				unsigned long triggerHandle = pDllTrigger->GetHandle(m_pCurrentTrigger->getDigitizerNumber());
				_variant_t value = period;
				pDllTrigger->SetParameterValue(triggerHandle, SVIOParameterEnum::TriggerPeriod, value);
			}
		}
	}

	SvDef::TriggerType SVTriggerObject::getType() const
	{
		SvDef::TriggerType result{SvDef::TriggerType::HardwareTrigger};
		if(nullptr != m_pCurrentTrigger)
		{
			result = m_pCurrentTrigger->getType();
		}
		
		return result;
	}
	
	long SVTriggerObject::getTriggerCount() const
	{ 
		return (nullptr != m_pCurrentTrigger) ? m_pCurrentTrigger->getTriggerCount() : -1;
	}
} //namespace SvTrig
