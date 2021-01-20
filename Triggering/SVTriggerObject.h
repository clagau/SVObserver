//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerObject
//* .File Name       : $Workfile:   SVTriggerObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:29:22  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "Definitions/TriggerType.h"
#include "SVOResource/resource.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVOLibrary/TriggerDevice.h"
#pragma endregion Includes

//Namespace used only for forward declaration
namespace SvTrig
{
	class SVTriggerClass;
} //namespace SvTrig

namespace SvTrig
{
	class SVTriggerObject : public SVObjectClass
	{
	public:
		explicit SVTriggerObject(LPCSTR ObjectName);
		SVTriggerObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRIGGEROBJECT );
		virtual ~SVTriggerObject();

		bool Create( SvTrig::SVTriggerClass* pTrigger );
		bool Destroy();

		// Runtime Functions
		bool CanGoOnline();
		bool GoOnline();
		bool GoOffline();

		bool Start();

		bool RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback);
		bool UnregisterCallback();

		long getStartObjectID() const;
		long getTriggerPerObjectID() const;
		void setObjectIDParameters(long startObjectID, long triggerPerObjectID);

		///Fires a trigger with given time stamp
		void Fire(double timeStamp);

		long GetSoftwareTriggerPeriod() const;
		void SetSoftwareTriggerPeriod(long period, bool setTimer = false);

		SvTrig::SVTriggerClass* getDevice() { return m_pTriggerDevice; }

		SvDef::TriggerType getType() const;
		long getTriggerCount() const;

	private:
		long m_timerPeriod{0L};
		SvTrig::SVTriggerClass* m_pTriggerDevice{nullptr};
	};

	typedef std::vector<SVTriggerObject*> SVTriggerObjectPtrVector;
} //namespace SvTrig
