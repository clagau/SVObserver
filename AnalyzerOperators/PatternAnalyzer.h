//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PatternAnalyzer
//* .File Name       : $Workfile:   svpatternanalyzerclass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Jul 2014 06:38:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "ImageAnalyzer.h"
#include "ObjectInterfaces/IPatternAnalyzer.h"
#include "SVLibrary/SVTemplate.h"
#include "Definitions/StringTypeDef.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVFileNameValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class PatternAnalyzer : public ImageAnalyzer, public SvOi::IPatternAnalyzer  
{
	SV_DECLARE_CLASS

	friend class SVSetupDialogManager;

public:
	PatternAnalyzer (SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPATTERNANALYZER);

	virtual	~PatternAnalyzer();
	virtual bool CloseObject() override;
	
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool onRun(SvIe::RunStatus &rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	void CloseMIL();

	virtual bool UpdateModelFromInputImage(long posX, long posY, long modelWidth, long modelHeight) override;
	bool UpdateModelFromInputImage(long posX = 0, long posY = 0);
	bool UpdateModelFromBuffer();

	bool SetSearchParameters();
	void SetDefaultSearchValues();

	virtual bool IsPtOverResult(const POINT& rPoint) override;
	virtual	SvDef::StringVector getAnalyzerResult() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual SvDef::StringVector getSpecialImageList() const override;
	virtual bool getSpecialImage(const std::string& rName, SvOi::SVImageBufferHandlePtr& rImagePtr) const override;
	virtual SvStl::MessageContainerVector validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet, SvOi::ResetParameter* pPar = nullptr) override;
	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;
	
protected:
	/// Reload model from file and restore the pattern.
	/// \param rImageFile [in] Filename of the model.
	/// \param pErrorMessages [out] Error messages if error happens.
	/// \returns bool true means restore was succeeded.
	bool RestorePattern(const std::string& rImageFile, SvStl::MessageContainerVector *pErrorMessages=nullptr);

	/// Reset Pattern model. If filename is not empty, it restore pattern, else it create a new model.
	/// \param pErrorMessages [in,out] Error messages if error happens.
	/// \returns bool True if no error happens.
	bool ResetPattern(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	SVObjectClass* CreateResult();

	virtual void addOverlayGroups(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const override;
	virtual void addOverlayGroups(SvPb::Overlay& rOverlay) const override;
	
	void ResetResultValues();
	void ResizeResultValues(int nNum);

private:
	std::vector<SVExtentFigureStruct> GetResultExtentFigureList(long lOccurances, const SVImageExtentClass& rImageExtents) const;

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
	bool getNewUseDontCareValue(const SvOi::SetValueStructVector &rValueVector);

	/// Check if a new "dont care" filename is in the list and if the file correct.
	/// \param rValueVector [in] Vector of new values.
	/// \param rDontCareWidth [out] Width of the new image or the current image if this value is not in the vector.
	/// \param rDontCareHeight [out] Height of the new image or the current image if this value is not in the vector.
	/// \param rMessages [out] Vector of errors.
	/// \returns bool True if the value should be set and the image should reloaded from file.
	bool validateNewDontCareFileName(const SvOi::SetValueStructVector &rValueVector, long& rDontCareWidth, long& rDontCareHeight, SvStl::MessageContainerVector& rMessages);

	/// Check if a new model filename is in the list and if the file correct.
	/// \param rValueVector [in] Vector of new values.
	/// \param rDontCareWidth [out] Width of the new image or the current image if this value is not in the vector.
	/// \param rDontCareHeight [out] Height of the new image or the current image if this value is not in the vector.
	/// \param rMessages [out] Vector of errors.
	/// \returns bool True if the value should be set and the image should reloaded from file.
	bool validateNewModelFileName(const SvOi::SetValueStructVector &rValueVector, long& rModelWidth, long& rModelHeight, SvStl::MessageContainerVector &messages);

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
	bool CreateBuffer(long width, long height, SvOi::SVImageBufferHandlePtr& rBufferHandle);

	/// Load an image and copy it to a new created buffer.
	/// \param rImageFile [in] The file name.
	/// \param rWidthValueObject [in, out] A value object where the new width will be set.
	/// \param rHeightValueObject [in, out] A value object where the new height will be set.
	/// \param bufferHandle [in,out] The handle of the reloaded image.
	/// \param pErrorMessages [out] A vector of happened error during reloading.
	/// \returns bool true if reloading was successfully.
	bool ReloadImage(const std::string& rImageFile, SvVol::SVLongValueObjectClass& rWidthValueObject, SvVol::SVLongValueObjectClass& rHeightValueObject, SvOi::SVImageBufferHandlePtr& bufferHandle, SvStl::MessageContainerVector * pErrorMessages);

	HRESULT CreateModelHandle(long modelWidth, long modelHeight);
private:
	__int64 m_patContextHandle = M_NULL;		// Model Identifer Handle
	__int64 m_patResultHandle = M_NULL;	// Pattern matching result buffer identifier handle

	bool m_bAngleAccuracy;

	// Value Objects for Model Search Parameters.

	SvVol::SVLongValueObjectClass msv_lpatMaxOccurances;			// Max. number of occurrences to find.
	SvVol::SVDoubleValueObjectClass	msv_dpatAcceptanceThreshold;	// Minimum acceptance correlation.
	SvVol::SVDoubleValueObjectClass	msv_dpatCertaintyThreshold;		// Certainty level in percentage.
	SvVol::SVLongValueObjectClass msv_lpatSpeed;					// Search speed factor.		
	SvVol::SVLongValueObjectClass msv_lpatAccuracy;					// Required positional accuracy.

	SvVol::SVBoolValueObjectClass msv_bpatSearchAngleMode;			// Either Enabled or Disabled.
	SvVol::SVDoubleValueObjectClass	msv_dpatSearchAngle;
	SvVol::SVDoubleValueObjectClass	msv_dpatAngleDeltaNeg;
	SvVol::SVDoubleValueObjectClass	msv_dpatAngleDeltaPos;
	SvVol::SVDoubleValueObjectClass	msv_dpatAngleTolerance;
	SvVol::SVDoubleValueObjectClass	msv_dpatAngleAccuracy;
	SvVol::SVDoubleValueObjectClass	msv_dpatAngleInterpolation;

	SvVol::SVDoubleValueObjectClass	msv_dpatPreliminaryAcceptanceThreshold;
	SvVol::SVLongValueObjectClass		msv_lpatFastFind;
	SvVol::SVLongValueObjectClass		msv_lpatModelStep;
	SvVol::SVLongValueObjectClass		msv_lpatBeginningResolutionLevel;
	SvVol::SVLongValueObjectClass		msv_lpatFinalResolutionLevel;
	SvVol::SVLongValueObjectClass		msv_lpatAdditionalCandidates;
	SvVol::SVDoubleValueObjectClass	msv_dpatCandidateSpacingXMin;
	SvVol::SVDoubleValueObjectClass	msv_dpatCandidateSpacingYMin;

	// Value Objects for Model Results.
	SvVol::SVDoubleValueObjectClass	msv_dpatResultMatchScore;
	SvVol::SVDoubleValueObjectClass	msv_dpatResultX;
	SvVol::SVDoubleValueObjectClass	msv_dpatResultY;
	SvVol::SVDoubleValueObjectClass	msv_dpatResultAngle;

	SvVol::SVLongValueObjectClass		msv_lpatNumFoundOccurances; // define here or just reference result class ?

	vector2d<double>             m_vec2dPatResults;

	SvVol::SVLongValueObjectClass m_lpatModelWidth;		// Model Width.
	SvVol::SVLongValueObjectClass m_lpatModelHeight;		// Model Height.
	SvVol::SVLongValueObjectClass m_lpatModelCenterX;		// Model Center X.
	SvVol::SVLongValueObjectClass m_lpatModelCenterY;		// Model Center Y.
	
	SvVol::SVBoolValueObjectClass msv_bpatCircularOverscan;
	SvVol::SVFileNameValueObjectClass msv_szModelImageFile; // Name of the Model Image File
	SvOi::SVImageBufferHandlePtr m_patBufferHandlePtr;	// Model Image Buffer Handle

	SvVol::SVBoolValueObjectClass m_bpatDontCare;
	SvVol::SVFileNameValueObjectClass m_DontCareImageFile; // Name of the Model Image File
	SvVol::SVLongValueObjectClass m_dontCareWidth;		// Don't Care Width.
	SvVol::SVLongValueObjectClass m_dontCareHeight;	// Don't Care Height.
	SvOi::SVImageBufferHandlePtr m_DontCareBufferHandlePtr;
	bool m_bReloadModelFromFile;

	int m_nPatternIndex; // index of pattern moused over, for displaying results
};

} //namespace SvAo
