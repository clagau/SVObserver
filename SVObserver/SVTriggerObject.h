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

#ifndef SVTRIGGEROBJECT_H
#define SVTRIGGEROBJECT_H

#include "SVInfoStructs.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVObjectLibrary/SVObjectClass.h"

class SVOResponseClass;
class SVTriggerClass;

class SVTriggerObject : public SVObjectClass
{
public:
	SVTriggerObject( LPCSTR ObjectName );
	SVTriggerObject( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVTRIGGEROBJECT );
	virtual ~SVTriggerObject();

	BOOL Create( SVTriggerClass *psvDevice );
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

	SVTriggerClass* mpsvDevice;

private:
	bool m_bSoftwareTrigger;
	long m_timerPeriod;

	#ifdef SV_LOG_STATUS_INFO
		SVStatusDeque m_StatusLog;
	#endif

};
typedef SVVector< SVTriggerObject* > SVTriggerObjectArray;

#endif /* _INC_SVTRIGGEROBJECT_INCLUDED */

