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
#include "SVTriggerObject.h"
#include "SVTriggerConstants.h"
#include "SVTriggerInfoStruct.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "Triggering/SVTriggerClass.h"
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

	bool SVTriggerObject::Create( SvTrig::SVTriggerClass* pTrigger )
	{
		bool bOk = true;

		if ( nullptr != pTrigger )
		{
			m_pTriggerDevice = pTrigger;
			m_pTriggerDevice->resetTriggerCount();
			m_ObjectTypeInfo.m_ObjectType = SvPb::SVTriggerObjectType;
		}
		else 
		{
			bOk = false;
		}
		return bOk;
	}

	bool SVTriggerObject::Destroy()
	{
		if ( nullptr != m_pTriggerDevice )
		{
			m_pTriggerDevice->Destroy();

			m_pTriggerDevice = nullptr;

			return true;
		}
		return false;
	}

	bool SVTriggerObject::CanGoOnline()
	{
		///This needs to be done before the cameras are started
		if (nullptr != m_pTriggerDevice)
		{
			m_pTriggerDevice->enableInternalTrigger();
		}
		return nullptr != m_pTriggerDevice;
	}

	bool SVTriggerObject::GoOnline()
	{
		return (nullptr != m_pTriggerDevice);
	}

	bool SVTriggerObject::GoOffline()
	{
		if ( nullptr != m_pTriggerDevice )
		{
			return (S_OK == m_pTriggerDevice->Stop());
		}
		return false;
	}

	bool SVTriggerObject::Start()
	{ 
		return (nullptr != m_pTriggerDevice) && (S_OK == m_pTriggerDevice->Start());
	}

	bool SVTriggerObject::RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback)
	{
		if ( nullptr != m_pTriggerDevice )
		{
			m_pTriggerDevice->RegisterCallback(pPpqTriggerCallback);
		}
		return false;
	}

	bool SVTriggerObject::UnregisterCallback()
	{
		if (nullptr != m_pTriggerDevice)
		{
			m_pTriggerDevice->UnregisterCallback();
		}
		return false;
	}

	long SVTriggerObject::getStartObjectID() const
	{
		if (nullptr != m_pTriggerDevice)
		{
			return m_pTriggerDevice->getStartObjectID();
		}
		return -1;
	}

	long SVTriggerObject::getTriggerPerObjectID() const
	{
		if (nullptr != m_pTriggerDevice)
		{
			return m_pTriggerDevice->getTriggerPerObjectID();
		}
		return -1;
	}

	void SVTriggerObject::setObjectIDParameters(long startObjectID, long triggerPerObjectID)
	{
		if (nullptr != m_pTriggerDevice)
		{
			m_pTriggerDevice->setObjectIDParameters(startObjectID, triggerPerObjectID);
		}
	}

	void SVTriggerObject::Fire(double timeStamp)
	{
		if (nullptr != m_pTriggerDevice)
		{
			SvTrig::SVTriggerInfoStruct triggerInfo;
			triggerInfo.bValid = true;
			triggerInfo.m_Data[SvTrig::TriggerDataEnum::TimeStamp] = _variant_t(timeStamp);
			///Trigger channel 0 based
			triggerInfo.m_Data[SvTrig::TriggerDataEnum::TriggerChannel] = _variant_t(m_pTriggerDevice->getTriggerChannel());
			m_pTriggerDevice->Notify(triggerInfo);
		}
	}

	long SVTriggerObject::GetSoftwareTriggerPeriod() const
	{
		return m_timerPeriod;
	}

	void SVTriggerObject::SetSoftwareTriggerPeriod(long period, bool setTimer)
	{
		m_timerPeriod = period;

		if( setTimer && nullptr != m_pTriggerDevice )
		{
			SVIOTriggerLoadLibraryClass* pDllTrigger = m_pTriggerDevice->getDLLTrigger();

			if( nullptr != pDllTrigger )
			{
				unsigned long triggerHandle = pDllTrigger->GetHandle(m_pTriggerDevice->getDigitizerNumber());
				_variant_t value = period;
				pDllTrigger->SetParameterValue(triggerHandle, 0, value);
			}
		}
	}

	SvDef::TriggerType SVTriggerObject::getType() const
	{
		SvDef::TriggerType result{SvDef::TriggerType::HardwareTrigger};
		if(nullptr != m_pTriggerDevice)
		{
			result = m_pTriggerDevice->getType();
		}
		
		return result;
	}
	
	long SVTriggerObject::getTriggerCount() const
	{ 
		return (nullptr != m_pTriggerDevice) ? m_pTriggerDevice->getTriggerCount() : -1; 
	}
} //namespace SvTrig
