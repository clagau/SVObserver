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
#include "SVStatusLibrary/MessageTextEnum.h"
#include "ObjectInterfaces/IPatternAnalyzer.h"
#pragma endregion Includes

class SVPatternAnalyzerClass : public SVImageAnalyzerClass, public SvOi::IPatternAnalyzer  
{
	SV_DECLARE_CLASS (SVPatternAnalyzerClass);

	friend class SVSetupDialogManager;

public:
	SVPatternAnalyzerClass (SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPATTERNANALYZER);

	virtual	~SVPatternAnalyzerClass();
	virtual bool CloseObject() override;
	
	virtual bool CreateObject(SVObjectLevelCreateStruct* pCreateStructure) override;
	virtual bool onRun(SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	void CloseMIL();

	virtual bool UpdateModelFromInputImage(long posX, long posY, long modelWidth, long modelHeight) override;
	bool UpdateModelFromInputImage(long posX = 0, long posY = 0);
	bool UpdateModelFromBuffer();

	bool SetSearchParameters();
	void SetDefaultSearchValues();

	virtual bool IsPtOverResult(const POINT& rPoint) override;

	virtual	void DisplayAnalyzerResult() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual void getSpecialImageList(std::vector<SVString>& rList) const override;
	virtual bool getSpecialImage(const SVString& rName, SvOi::MatroxImageSmartHandlePtr& rImagePtr) const override;
	virtual SvStl::MessageContainerVector validateAndSetEmmeddedValues(const SvOi::SetValueObjectPairVector& rValueVector, bool shouldSet) override;
	
protected:
	/// Reload model from file and restore the pattern.
	/// \param rImageFile [in] Filename of the model.
	/// \param pErrorMessages [out] Error messages if error happens.
	/// \returns bool true means restore was succeeded.
	bool RestorePattern(const SVString& rImageFile, SvStl::MessageContainerVector *pErrorMessages=nullptr);

	/// Reset Pattern model. If filename is not empty, it restore pattern, else it create a new model.
	/// \param pErrorMessages [in,out] Error messages if error happens.
	/// \returns bool True if no error happens.
	bool ResetPattern(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	void CreateResult();

	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;
	
	void ResetResultValues();
	void ResizeResultValues(int nNum);

private:
	std::vector<SVExtentFigureStruct> GetResultExtentFigureList( long lOccurances );

	/// Execute the matrox pattern command and set the result to the value-objects.
	/// \param ImageBufId [in] Input image for the excute command.
	/// \returns Status of the execution or the get results.
	HRESULT executePatternAndSetResults( SVMatroxBuffer ImageBufId);

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	/// Reload dont care image from file. If filename invalid it return an error.
	/// \param pErrorMessages [in,out] Error messages if error happens.
	/// \returns bool True if no error happens.
	bool RestoreDontCareImage(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	/// Get new Value for "use dont Care" if in ValueVector or get the current value.
	/// \param rValueVector [in] Vector of new values.
	/// \returns bool "Use dont Care"-Value
	bool getNewUseDontCareValue(const SvOi::SetValueObjectPairVector &rValueVector);

	/// Check if a new "dont care" filename is in the list and if the file correct.
	/// \param rValueVector [in] Vector of new values.
	/// \param rDontCareWidth [out] Width of the new image or the current image if this value is not in the vector.
	/// \param rDontCareHeight [out] Height of the new image or the current image if this value is not in the vector.
	/// \param rMessages [out] Vector of errors.
	/// \returns bool True if the value should be set and the image should reloaded from file.
	bool validateNewDontCareFileName(const SvOi::SetValueObjectPairVector &rValueVector, long& rDontCareWidth, long& rDontCareHeight, SvStl::MessageContainerVector& rMessages);

	/// Check if a new model filename is in the list and if the file correct.
	/// \param rValueVector [in] Vector of new values.
	/// \param rDontCareWidth [out] Width of the new image or the current image if this value is not in the vector.
	/// \param rDontCareHeight [out] Height of the new image or the current image if this value is not in the vector.
	/// \param rMessages [out] Vector of errors.
	/// \returns bool True if the value should be set and the image should reloaded from file.
	bool validateNewModelFileName(const SvOi::SetValueObjectPairVector &rValueVector, long& rModelWidth, long& rModelHeight, SvStl::MessageContainerVector &messages);

	/// Check if the size of the model, and if dont care is use also the size of this, is valid.
	/// \param pErrorMessages [in, out] Fill the error message if invalid data found.
	/// \returns bool true if size is valid.
	bool IsValidSize(SvStl::MessageContainerVector *pErrorMessages = nullptr) const;

	/// Check if the size of the model, and if dont care is use also the size of this, is valid.
	/// \param modelWidth [in] Width of the model.
	/// \param modelHeight [in] Height of the model.
	/// \param useDontCare [in] Flag if "dont care" should be used.
	/// \param dontCareWidth [in] Width of the dont care width. Only used if useDontCare is true.
	/// \param dontCareHeight [in] Height of the dont care width. Only used if useDontCare is true.
	/// \param pErrorMessages [in,out] Fill the error message if invalid data found.
	/// \returns bool true if size is valid.
	bool IsValidSize(long modelWidth, long modelHeight, bool useDontCare, long dontCareWidth, long dontCareHeight, SvStl::MessageContainerVector *pErrorMessages) const;

	/// Create a new model buffer with the model width and height and save it in the model handle
	/// \returns bool true if creation was successfully.
	bool CreateModelBuffer();

	/// Create a new buffer with the required width and height
	/// \param width [in] Required width.
	/// \param height [in] Required height.
	/// \param rBufferHandle [in,out] Smart pointer to the buffer handle. It will be cleared at the begin.
	/// \returns bool true if creation was successfully.
	bool CreateBuffer(long width, long height, SVSmartHandlePointer& rBufferHandle);

	/// Load an image and copy it to a new created buffer.
	/// \param rImageFile [in] The file name.
	/// \param rWidthValueObject [in, out] A value object where the new width will be set.
	/// \param rHeightValueObject [in, out] A value object where the new height will be set.
	/// \param bufferHandle [in,out] The handle of the reloaded image.
	/// \param pErrorMessages [out] A vector of happened error during reloading.
	/// \returns bool true if reloading was successfully.
	bool ReloadImage(const SVString& rImageFile, SVLongValueObjectClass& rWidthValueObject, SVLongValueObjectClass& rHeightValueObject, SVSmartHandlePointer& bufferHandle, SvStl::MessageContainerVector * pErrorMessages);

	HRESULT CreateModelHandle(long modelWidth, long modelHeight);
private:
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
	SVSmartHandlePointer m_patBufferHandlePtr;	// Model Image Buffer Handle

	SVBoolValueObjectClass m_bpatDontCare;
	SVFileNameValueObjectClass m_DontCareImageFile; // Name of the Model Image File
	SVLongValueObjectClass m_dontCareWidth;		// Don't Care Width.
	SVLongValueObjectClass m_dontCareHeight;	// Don't Care Height.
	SVSmartHandlePointer m_DontCareBufferHandlePtr;
	bool m_bReloadModelFromFile;

	int m_nPatternIndex; // index of pattern moused over, for displaying results
};


