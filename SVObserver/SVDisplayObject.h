//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayObject
//* .File Name       : $Workfile:   SVDisplayObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVInfoStructs.h"
#pragma endregion Includes

class SVIPDoc;

class SVDisplayObject : 
	public SVObjectClass
{
public:
	explicit SVDisplayObject(LPCSTR ObjectName);
	SVDisplayObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDISPLAYOBJECT );
	virtual ~SVDisplayObject();

	HRESULT ObserverUpdate( const std::pair<long, SVInspectionInfoStruct>& p_rData );
	HRESULT ObserverUpdate( const SVInspectionNameUpdate& p_rData );
	HRESULT ObserverUpdate( const SVRemoveImages& p_rData );
	HRESULT ObserverUpdate( const SVRemoveValues& p_rData );
	HRESULT ObserverUpdate( const SVRemoveSubjectStruct& p_rData );

	void SetInspectionID(uint32_t inspectionID, SVIPDoc* pDoc );

	SVIPDoc* GetIPDoc() const;

	BOOL Create();
	BOOL Destroy();

	// Runtime Functions
	bool CanGoOnline();
	bool GoOnline();
	bool GoOffline();

	void SetIPDocDisplayComplete();

protected:
	static DWORD WINAPI SVDisplayThreadFunc( LPVOID lpParam );

	HRESULT ProcessInspectionComplete( bool& p_rProcessed );
	HRESULT ProcessNotifyIPDoc( bool& p_rProcessed );

	HRESULT FinishInspection( const std::pair<long, SVInspectionInfoStruct >& rData);

	void UpdateNextDisplayEvent();

	uint32_t m_InspectionID = SvDef::InvalidObjectId;

	// HANDLEs to the Display events that may be needed by the threads
	HANDLE m_hStartEvent;
	HANDLE m_hStopEvent;
	HANDLE m_hDisplayThread;

	std::mutex m_displayMutex;

	long m_PendingTrigger;
	long m_CurrentTrigger;
	long m_DisplayedTrigger;

	long m_IPDocDisplayComplete;

	long m_FrameRate;
	double m_NextDisplayEvent = 0;
	double m_LastUpdateTime = 0;
	SVIPDoc* m_pDoc = nullptr;
};
