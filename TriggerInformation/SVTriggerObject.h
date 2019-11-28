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
#pragma endregion Includes

typedef HRESULT (CALLBACK *LPSVFINISHPROC)(void*,void*,void*);

class SVOResponseClass; //defined in SVOLibrary/SVOResponseClass.cpp

//Namespace used only for forward declaration
namespace SvTh
{
	class SVTriggerClass;
} //namespace SvTh

namespace SvTi
{
	class SVTriggerObject : public SVObjectClass
	{
	public:
		SVTriggerObject( LPCSTR ObjectName );
		SVTriggerObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRIGGEROBJECT );
		virtual ~SVTriggerObject();

		bool Create( SvTh::SVTriggerClass* pTrigger );
		bool Destroy();

		// Runtime Functions
		bool CanGoOnline();
		bool GoOnline();
		bool GoOffline();

		bool Start();

		bool RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );
		bool UnregisterFinishProcess( void *pOwner );

		void FinishProcess( SVOResponseClass *pResponse );
		///Fires a trigger with given time stamp
		void Fire(double timeStamp);

		long GetSoftwareTriggerPeriod() const;
		void SetSoftwareTriggerPeriod(long period, bool setTimer = false);

		SvTh::SVTriggerClass* getDevice() { return m_pTriggerDevice; }

		SvDef::TriggerType getType() const;
		void* getOwner() { return m_pOwner; }
		long getTriggerCount() { return m_triggerCount; }

	private:
		long m_timerPeriod{0L};
		void* m_pOwner {nullptr};

		LPSVFINISHPROC m_pFinishProc{nullptr};

		long m_triggerCount{0L};
		SvTh::SVTriggerClass* m_pTriggerDevice{nullptr};

		#ifdef SV_LOG_STATUS_INFO
			std::vector<std::string> m_StatusLog;
		#endif
	};
	typedef std::vector<SVTriggerObject*> SVTriggerObjectPtrVector;

} //namespace SvTi
