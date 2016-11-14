//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatternAnalyzerClass
//* .File Name       : $Workfile:   svpatternanalyzerclass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Jul 2014 06:38:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageAnalyzerClass.h"
#include "ObjectInterfaces/MessageTextEnum.h"
#pragma endregion Includes

class SVPatternAnalyzerClass : public SVImageAnalyzerClass  
{
	SV_DECLARE_CLASS (SVPatternAnalyzerClass);

	friend class SVSetupDialogManager;

public:
	SVPatternAnalyzerClass (BOOL BCreateDefaultTaskList = TRUE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPATTERNANALYZER);

	virtual	~SVPatternAnalyzerClass();
	virtual	BOOL CloseObject();
	
	virtual BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStructure);
	virtual BOOL OnValidate();
	virtual BOOL onRun(SVRunStatusClass &RRunStatus);

	SVImageInfoClass m_patBuffer;	// Model Image Buffer
	SVImageInfoClass m_patModel;	// Model Identifier.
	SVImageInfoClass m_patResult; 	// Pattern matching result buffer identifier.

	SVSmartHandlePointer m_patBufferHandlePtr;	// Model Image Buffer Handle
	SVMatroxPatternModel m_patModelHandle;		// Model Identifer Handle
	SVMatroxPatResult m_patResultHandle;	// Pattern matching result buffer identifier handle

	BOOL m_bAngleAccuracy;

// Value Objects for Model Search Parameters.

	SVLongValueObjectClass		msv_lpatMaxOccurances;			// Max. number of occurrences to find.
	SVDoubleValueObjectClass	msv_dpatAcceptanceThreshold;	// Minimum acceptance correlation.
	SVDoubleValueObjectClass	msv_dpatCertaintyThreshold;		// Certainty level in percentage.
	SVLongValueObjectClass		msv_lpatSpeed;					// Search speed factor.		
	SVLongValueObjectClass		msv_lpatAccuracy;				// Required positional accuracy.

	SVBoolValueObjectClass		msv_bpatSearchAngleMode;		// Either Enabled or Disabled.
	SVDoubleValueObjectClass	msv_dpatSearchAngle;
	SVDoubleValueObjectClass	msv_dpatAngleDeltaNeg;
	SVDoubleValueObjectClass	msv_dpatAngleDeltaPos;
	SVDoubleValueObjectClass	msv_dpatAngleTolerance;
	SVDoubleValueObjectClass	msv_dpatAngleAccuracy;
	SVDoubleValueObjectClass	msv_dpatAngleInterpolation;

	SVDoubleValueObjectClass	msv_dpatPreliminaryAcceptanceThreshold;
	SVLongValueObjectClass		msv_lpatFastFind;
	SVLongValueObjectClass		msv_lpatModelStep;
	SVLongValueObjectClass		msv_lpatBeginningResolutionLevel;
	SVLongValueObjectClass		msv_lpatFinalResolutionLevel;
	SVLongValueObjectClass		msv_lpatAdditionalCandidates;
	SVDoubleValueObjectClass	msv_dpatCandidateSpacingXMin;
	SVDoubleValueObjectClass	msv_dpatCandidateSpacingYMin;

// Value Objects for Model Results.
	SVDoubleValueObjectClass	msv_dpatResultMatchScore;
	SVDoubleValueObjectClass	msv_dpatResultX;
	SVDoubleValueObjectClass	msv_dpatResultY;
	SVDoubleValueObjectClass	msv_dpatResultAngle;

	SVLongValueObjectClass		msv_lpatNumFoundOccurances; // define here or just reference result class ?

	vector2d<double>             m_vec2dPatResults;

	void CloseMIL();

	BOOL CreateModelBuffer();
	BOOL DestroyModelBuffer();

	BOOL UpdateModelFromInputImage();
	BOOL UpdateModelFromBuffer();

	BOOL RestorePattern(CString strImageFile, SvOi::MessageTextEnum *ErrMsgId = nullptr);
	BOOL SetSearchParameters();
	void SetDefaultSearchValues();

	void GetModelExtents(long& xPos, long& yPos, long& width, long& height);
	void SetModelExtents(long xPos, long yPos, long width, long height);
	void SetCircularOverscan(bool bCircularOverscan);

	virtual BOOL IsPtOverResult(CPoint point);
	virtual	void DisplayAnalyzerResult();

	BOOL GetModelImageFileName(CString &csFileName);

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;
	virtual HRESULT ResetObject();
	bool IsValidSize();

protected:
	void CreateResult();

	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray );
	
	void ResetResultValues();
	void ResizeResultValues(int nNum);

	int m_nPatternIndex; // index of pattern moused over, for displaying results
	
private:
	BOOL ResetImageFile();

	long m_lpatModelWidth;		// Model Width.
	long m_lpatModelHeight;		// Model Height.
	long m_lpatModelOriginX;	// X coordinate of search region origin with in target image.
	long m_lpatModelOriginY;	// Y coordinate of search region origin with in target image.
	
	SVBoolValueObjectClass msv_bpatCircularOverscan;
	SVFileNameValueObjectClass msv_szModelImageFile; // Name of the Model Image File
};


