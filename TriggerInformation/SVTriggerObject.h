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


typedef HRESULT (CALLBACK *LPSVFINISHPROC)(void*,void*,void*);

#include "SVOLibrary/SVQueueObject.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVTriggerInfoStruct.h"

#pragma endregion Includes

class SVOResponseClass; //defined in SVOLibrary/SVOResponseClass.cpp

namespace Seidenader { namespace TriggerHandling {//AB namespace used only for forward declaration

class SVTriggerClass;

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;

namespace Seidenader { namespace TriggerInformation {

	class SVTriggerObject : public SVObjectClass
	{
	public:
		SVTriggerObject( LPCSTR ObjectName );
		SVTriggerObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRIGGEROBJECT );
		virtual ~SVTriggerObject();

		BOOL Create( SvTh::SVTriggerClass *psvDevice );
		BOOL Destroy();

		// Runtime Functions
		BOOL CanGoOnline();
		BOOL GoOnline();
		BOOL GoOffline();

		BOOL RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );
		BOOL UnregisterFinishProcess( void *pOwner );

		virtual void FinishProcess( SVOResponseClass *pResponse );

		HRESULT EnableInternalTrigger();
		bool IsSoftwareTrigger() const;
		void SetSoftwareTrigger(bool bSoftwareTrigger);
		long GetSoftwareTriggerPeriod() const;
		void SetSoftwareTriggerPeriod(long period, bool setTimer = false);

		bool IsAcquisitionTrigger() const;
		void SetAcquisitionTriggered(bool bAcquisitionTriggered);

		LPSVFINISHPROC m_pFinishProc;
		void* m_pOwner;

		long m_lTriggerCount;

		SvTh::SVTriggerClass* mpsvDevice;

	private:
		bool m_bSoftwareTrigger;
		long m_timerPeriod;

		#ifdef SV_LOG_STATUS_INFO
			SVStatusDeque m_StatusLog;
		#endif
	};
	typedef SVVector< SVTriggerObject* > SVTriggerObjectArray;
} /* namespace TriggerInformation */ } /* namespace Seidenader */

namespace SvTi = Seidenader::TriggerInformation;
