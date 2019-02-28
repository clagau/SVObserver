//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVInfoStructs.h"
#pragma endregion Includes

class SVIPDoc;

class SVDisplayObject : 
	public SVObjectClass, 
	public SVObserverTemplate< std::pair<SVInspectionInfoStruct, long> >,
	public SVObserverTemplate< SVInspectionNameUpdate >,
	public SVObserverTemplate< SVRemoveImages >,
	public SVObserverTemplate< SVRemoveValues >,
	public SVObserverTemplate< SVRemoveSubjectStruct >
{
public:
	SVDisplayObject( LPCSTR ObjectName );
	SVDisplayObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDISPLAYOBJECT );
	virtual ~SVDisplayObject();

	virtual HRESULT ObserverUpdate( const std::pair<SVInspectionInfoStruct, long>& p_rData ) override;
	virtual HRESULT ObserverUpdate( const SVInspectionNameUpdate& p_rData ) override;
	virtual HRESULT ObserverUpdate( const SVRemoveImages& p_rData ) override;
	virtual HRESULT ObserverUpdate( const SVRemoveValues& p_rData ) override;
	virtual HRESULT ObserverUpdate( const SVRemoveSubjectStruct& p_rData ) override;

	void SetInspectionID( const SVGUID& p_rInspectionID, SVIPDoc* pDoc );

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

	HRESULT FinishInspection( const std::pair<SVInspectionInfoStruct, long>& p_rProduct );

	void UpdateNextDisplayEvent();

	SVGUID m_InspectionID;

	// HANDLEs to the Display events that may be needed by the threads
	HANDLE m_hStartEvent;
	HANDLE m_hStopEvent;
	HANDLE m_hDisplayThread;

	bool m_CriticalSectionCreated;
	CRITICAL_SECTION m_CriticalSection;

	long m_PendingTrigger;
	long m_CurrentTrigger;
	long m_DisplayedTrigger;

	long m_IPDocDisplayComplete;

	long m_FrameRate;
	SvTl::SVTimeStamp m_NextDisplayEvent;
	SvTl::SVTimeStamp m_LastUpdateTime;
	SVIPDoc* m_pDoc;
};
