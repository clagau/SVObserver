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
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IToolSet.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMainImageClass.h"
#include "SVResultList.h"
#include "SVTaskObjectList.h"
#pragma region Includes

class SVConditionalClass;

// This class encapsules basic properties to handle and execute an Image Processing Tool Set

class SVToolSetClass : public SVTaskObjectListClass, public SvOi::IToolSet 
{
	SV_DECLARE_CLASS( SVToolSetClass );

	friend class SVInspectionProcess; // for access to Run()

#pragma region Public Methods
public:
	typedef std::deque< SVGUID > SVToolIdDeque;

	SVToolSetClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOLSET );

	virtual ~SVToolSetClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual void SetInvalid();
	virtual HRESULT ResetObject();
	virtual BOOL OnValidate();
	virtual BOOL Validate();

	void Destroy();

	bool IsEnabled() const;
	bool IsEnabled(long p_lIndex) const;

	bool WasEnabled() const;

	HRESULT ResetCounts();
	HRESULT ClearResetCounts();

	//************************************
	// Description: This moves a tool to the desired index
	// Parameter: NewIndex <in> The new index for the tool
	// Parameter: pTool <in> Pointer to the tool
	//************************************
	void moveTool( int NewIndex, SVToolClass* pTool );

	void SetDefaultInputs();
	
	SVResultListClass* GetResultList();

	SVImageClass* getCurrentImage();

	int GetIndex( SVToolClass* PTool );

	SVEnumerateValueObjectClass* GetDrawFlagObject();

	bool getConditionalResult() const;
	bool getConditionalResult(long p_lIndex) const;

	SVConditionalClass* GetToolSetConditional() const;

	void GetToolIds( SVToolIdDeque& p_rToolIds ) const;

	HRESULT getResetCounts( bool& rResetCounts );
#pragma endregion Public Methods

#pragma region virtual method (IToolSet)
	virtual bool IsToolPreviousToSelected( const SVGUID& rToolID ) const override;
#pragma endregion virtual method (IToolSet)

#pragma region Protected Methods
protected:
	virtual BOOL Run( SVRunStatusClass& RRunStatus );
	virtual BOOL RunWithNewDisable( SVRunStatusClass& RRunStatus );

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	// Sends SVM_CREATE_ALL_OBJECTS to the child object
	// and returns the result of this message.
	virtual DWORD_PTR createAllObjectsFromChild( SVObjectClass* pChildObject ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void init();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SVBoolValueObjectClass m_RegressionTestMode;

	long m_SetNumber;

	SVClock::SVTimeStamp m_StartTime;
	SVClock::SVTimeStamp m_EndTime;
	SVClock::SVTimeStamp m_AverageTime;
	SVClock::SVTimeStamp m_ProcessTime;

	SVClock::SVTimeStamp m_TimeStampStart;
	SVClock::SVTimeStamp m_TimeStampEnd;

	SVBoolValueObjectClass m_Enabled;
	SVBoolValueObjectClass m_ResetCounts;
	SVLongValueObjectClass m_TriggerCount;

	SVResultListClass m_ResultList;

	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass m_Passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass m_Warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass m_Failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass m_ExplicitFailed;

	SVLongValueObjectClass m_PassedCount;
	SVLongValueObjectClass m_FailedCount;
	SVLongValueObjectClass m_WarnedCount;

	SVLongValueObjectClass m_EnabledCount;
	SVLongValueObjectClass m_ProcessedCount;

	// Conditional input
	SVInObjectInfoStruct inputConditionBoolObjectInfo;

	// Conditional tool set drawing flag.
	SVEnumerateValueObjectClass	m_DrawFlag;
	SVTimerValueObjectClass	m_ToolTime;

	bool m_bResetMinMaxToolsetTime;

	SVTimerValueObjectClass m_MinToolsetTime;
	SVTimerValueObjectClass m_MaxToolsetTime;

private:
	// Embedded Object:
	SVMainImageClass m_MainImageObject;	// Main toolset image
	SVLongValueObjectClass m_latestCompletionPPQIndex; // the PPQ position at which the most recently completed product was located when it was completed
	SVTimerValueObjectClass m_TriggerDelta; ///< The time interval between the two most recent triggers
	SVTimerValueObjectClass m_LastTriggerToPPQCompletion; ///< for the most recently completed product: The time interval between the trigger that started the product and inspection completion in microsceconds
	SVTimerValueObjectClass m_LastTriggerToStart; ///< for the most recently completed product: The time interval between the trigger that started the product and the start of the inspection in microsceconds
	SVDoubleValueObjectClass m_Width;	//! The toolset image width			
	SVDoubleValueObjectClass m_Height;	//! The toolset image height
#pragma endregion Member Variables
};

