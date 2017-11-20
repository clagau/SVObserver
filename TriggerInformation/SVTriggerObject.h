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
#include "SVOResource/resource.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVTriggerInfoStruct.h"
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

		bool Create( SvTh::SVTriggerClass *psvDevice );
		bool Destroy();

		// Runtime Functions
		bool CanGoOnline();
		bool GoOnline();
		bool GoOffline();

		bool RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );
		bool UnregisterFinishProcess( void *pOwner );

		void FinishProcess( SVOResponseClass *pResponse );

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
	typedef std::vector<SVTriggerObject*> SVTriggerObjectPtrVector;

} //namespace SvTi
