//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerObj
//* .File Name       : $Workfile:   SVOTriggerObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:15:04  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVTriggerConstants.h"
#include "Definitions/TriggerType.h"
#pragma endregion Includes


namespace SvTi
{
	class SVOTriggerObj  
	{
	public:
		SVOTriggerObj(const std::string& name, int iDig) : m_sTriggerDisplayName(name)
			, m_iDigNumber(iDig)
			, m_timerPeriod(SvTi::TimerPeriod)
			, m_triggerType(SvDef::TriggerType::HardwareTrigger){}

		virtual ~SVOTriggerObj() = default;

		LPCTSTR GetTriggerDisplayName() const { return m_sTriggerDisplayName.c_str(); }
		int GetTriggerDigNumber() const { return m_iDigNumber; }

		long GetTimerPeriod() const { return m_timerPeriod; }
		void SetTimerPeriod(long period) { m_timerPeriod = period; }

		long getStartObjectID() const { return m_startObjectID; }
		void setStartObjectID(long startObjectID) { m_startObjectID = startObjectID; }

		long getTriggerPerObjectID() const { return m_triggerPerObjectID; }
		void setTriggerPerObjectID(long triggerPerObjectID) { m_triggerPerObjectID = triggerPerObjectID; }

		SvDef::TriggerType getTriggerType() const { return m_triggerType; }
		void setTriggerType(SvDef::TriggerType triggerType) { m_triggerType = triggerType; }

	private:  //data members
		std::string m_sTriggerDisplayName;
		SvDef::TriggerType m_triggerType;

		int m_iDigNumber{ 0 };
		long m_timerPeriod{ 0L };
		long m_startObjectID{ 1L };
		long m_triggerPerObjectID{ 1L };
	};

	typedef std::shared_ptr<SVOTriggerObj> SVOTriggerObjPtr;
} //namespace SvTi
