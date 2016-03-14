//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSet
//* .File Name       : $Workfile:   SVToolSet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 04:24:36  $
//******************************************************************************

#ifndef SVTOOLSET_H
#define SVTOOLSET_H

#include "ObjectInterfaces/IToolSet.h"

#include "SVMainImageClass.h"
#include "SVResultList.h"
#include "SVTaskObjectList.h"
#include "SVValueObjectImpl.h"

class SVConditionalClass;

/*
This class capsules basic properties to handle and execute an Image Processing Tool Set
*/

class SVToolSetClass : virtual public SvOi::IToolSet,
		public SVTaskObjectListClass	
{
	SV_DECLARE_CLASS( SVToolSetClass );

	friend class SVInspectionProcess;

public:
	typedef std::deque< SVGUID > SVToolIdDeque;

	SVToolSetClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVTOOLSET );

	virtual ~SVToolSetClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual void SetInvalid();
	virtual HRESULT ResetObject();
	virtual BOOL OnValidate();
	virtual BOOL Validate();

	void Destroy();
	void DestroyAt( int I );

	bool IsEnabled() const;
	bool IsEnabled(long p_lIndex) const;

	bool WasEnabled() const;

	HRESULT ResetCounts();
	HRESULT ClearResetCounts();

	//BOOL setCurrent( SVToolClass* Tool );
	//BOOL setCurrent( int Index );
	//void ResetCurrent();

	void InsertToolAt( int nIndex, SVToolClass* newElement, int nCount = 1 );

	//************************************
	// Description: This moves a tool to the desired index
	// Parameter: NewIndex <in> The new index for the tool
	// Parameter: pTool <in> Pointer to the tool
	//************************************
	void moveTool( int NewIndex, SVToolClass* pTool );

	void SetDefaultInputs();
    void CheckForExistingName(CString& newText,	SVToolClass* pTool);
	
	//SVToolClass*		getFirst();
	//SVToolClass*		getLast();
	//SVToolClass*		getNext();
	//SVToolClass*		getPrevious();
	//SVToolClass*		getCurrent();
	//int					getCurrentIndex();
	SVResultListClass*	GetResultList();

	SVImageClass*		getCurrentImage();

	int					GetIndex( SVToolClass* PTool );

	SVToolClass*		GetToolAt( int nIndex ) const;

	SVEnumerateValueObjectClass* GetDrawFlagObject();

	bool getConditionalResult() const;
	bool getConditionalResult(long p_lIndex) const;

	SVConditionalClass* GetToolSetConditional() const;

	void GetToolIds( SVToolIdDeque& p_rToolIds ) const;

#pragma region virtual method (IToolSet)
	virtual bool IsToolPreviousToSelected( const SVGUID& rToolID ) const override;
#pragma region virtual method (IToolSet)

	SVBoolValueObjectClass RegressionTestMode;

	long		setNumber;

	SVClock::SVTimeStamp m_StartTime;
	SVClock::SVTimeStamp m_EndTime;
	SVClock::SVTimeStamp m_AverageTime;
	SVClock::SVTimeStamp m_ProcessTime;

	SVClock::SVTimeStamp m_TimeStampStart;
	SVClock::SVTimeStamp m_TimeStampEnd;

	SVBoolValueObjectClass enabled;
	SVBoolValueObjectClass m_bvoResetCounts;
	SVLongValueObjectClass m_lvoTriggerCount;

protected:
	virtual BOOL Run( SVRunStatusClass& RRunStatus );
	virtual BOOL RunWithNewDisable( SVRunStatusClass& RRunStatus );

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	// Sends SVM_CREATE_ALL_OBJECTS to the child object
	// and returns the result of this message.
	virtual DWORD_PTR createAllObjectsFromChild( SVObjectClass* pChildObject ) override;

	SVResultListClass	m_ResultList;

	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	explicitFailed;

	SVLongValueObjectClass passedCount;
	SVLongValueObjectClass failedCount;
	SVLongValueObjectClass warnedCount;

	SVLongValueObjectClass enabledCount;
	SVLongValueObjectClass processedCount;

	// Conditional input
	SVInObjectInfoStruct	inputConditionBoolObjectInfo;

	// Conditional tool set drawing flag.
	SVEnumerateValueObjectClass	drawFlag;
	SVTimerValueObjectClass	ToolTime;

	bool m_bResetMinMaxToolsetTime;

	SVTimerValueObjectClass m_svMinToolsetTime;
	SVTimerValueObjectClass m_svMaxToolsetTime;

	/* maybe later
	SVLongValueObjectClass	processesCompleted;
	SVDoubleValueObjectClass executionTime;
	*/

	// Embedded Object:
	SVMainImageClass		mainImageObject;	// Embedded

private:

	SVLongValueObjectClass m_latestCompletionPPQIndex; // the PPQ position at which the most recently completed product was located when it was completed
	SVTimerValueObjectClass m_TriggerDelta; ///< The time interval between the two most recent triggers
	SVTimerValueObjectClass m_LastTriggerToPPQCompletion; ///< for the most recently completed product: The time interval between the trigger that started the product and inspection completion in microsceconds
	SVTimerValueObjectClass m_LastTriggerToStart; ///< for the most recently completed product: The time interval between the trigger that started the product and the start of the inspection in microsceconds

	void init();

};

#endif

