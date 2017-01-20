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
	virtual	BOOL CloseObject() override;
	
	virtual BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStructure) override;
	virtual BOOL OnValidate() override;
	virtual BOOL onRun(SVRunStatusClass &RRunStatus) override;

	//@TODO[MZA][7.40][22.11.2016] should be move to private, now it is needed public because SVPatModelPageClass use it
	SVSmartHandlePointer m_patBufferHandlePtr;	// Model Image Buffer Handle

	void CloseMIL();

	bool CreateModelBuffer();

	bool UpdateModelFromInputImage(long modelPosX = 0, long modelPosY = 0);
	bool UpdateModelFromBuffer();

	bool RestorePattern(const SVString& rImageFile, SvOi::MessageTextEnum *ErrMsgId = nullptr);
	bool SetSearchParameters();
	void SetDefaultSearchValues();

	virtual bool IsPtOverResult(const POINT& rPoint) override;

	virtual	void DisplayAnalyzerResult() override;

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;
	virtual HRESULT ResetObject() override;
	bool IsValidSize();

protected:
	void CreateResult();

	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray ) override;
	
	void ResetResultValues();
	void ResizeResultValues(int nNum);

private:
	bool ResetImageFile();

	std::vector<SVExtentFigureStruct> GetResultExtentFigureList( long lOccurances );

	/// Execute the matrox pattern command and set the result to the value-objects.
	/// \param ImageBufId [in] Input image for the excute command.
	/// \param index [in] Index position in which bucket in the value-objects the result should be stored.
	/// \returns SVMatroxPatternInterface::SVStatusCode Status of the excution or the get results.
	SVMatroxPatternInterface::SVStatusCode executePatternAndSetResults( SVMatroxBuffer ImageBufId, long index = 1 );

private:
	SVImageInfoClass m_patBuffer;	// Model Image Buffer
	SVImageInfoClass m_patModel;	// Model Identifier.
	SVImageInfoClass m_patResult; 	// Pattern matching result buffer identifier.

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

	SVLongValueObjectClass m_lpatModelWidth;		// Model Width.
	SVLongValueObjectClass m_lpatModelHeight;		// Model Height.
	SVLongValueObjectClass m_lpatModelCenterX;		// Model Center X.
	SVLongValueObjectClass m_lpatModelCenterY;		// Model Center Y.
	
	SVBoolValueObjectClass msv_bpatCircularOverscan;
	SVFileNameValueObjectClass msv_szModelImageFile; // Name of the Model Image File

	int m_nPatternIndex; // index of pattern moused over, for displaying results
};


