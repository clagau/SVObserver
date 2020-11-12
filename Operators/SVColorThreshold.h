//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorThreshold
//* .File Name       : $Workfile:   SVColorThreshold.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 09:47:24  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/BandEnums.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVOperator.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

//! This structure holds the values and images required for each of the bands
struct BandThreshold
{
	SvVol::SVLongValueObjectClass	m_UpperThreshold;		//! The upper threshold limit for the band
	SvVol::SVLongValueObjectClass	m_LowerThreshold;		//! The lower threshold limit for the band
	SvVol::SVBoolValueObjectClass	m_ThresholdExclude;		//! When this is set then the color is excluded
	SvVol::SVBoolValueObjectClass	m_ThresholdEnabled;		//! Enables or disables the threshold band

	SvOl::SVInObjectInfoStruct m_InputImage;		//! The structure to the input image (band image of the color tool)
	SvIe::SVImageClass	m_OutputImage;				//! The result image of the threshold
};

//! The class has the 3 bands from the color tool is input images
//! It has 3 Threshold images as an output
//! An extra output image which is a combined image of all 3 Threshold images as bitwise AND
class SVColorThreshold : public SVOperator
{
	SV_DECLARE_CLASS( SVColorThreshold );

#pragma region Constructor
public:
	SVColorThreshold( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCOLORTHRESHOLD );

	virtual ~SVColorThreshold();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SvIe::SVImageClass& GetOutputImage() { return m_OutputImage; };
	BandThreshold* GetBandThreshold(SvDef::BandEnum Band);
	SvVol::SVBoolValueObjectClass* GetOutputThresholdEnabled() { return &m_OutputThresholdEnabled; };
	SvIe::SVImageClass* GetBandInputImage(SvDef::BandEnum Band, bool bRunMode = false);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void LocalInitialize();

	bool createImages();
	bool createOutputImage(SvIe::SVImageClass* pInputImage, SvIe::SVImageClass* pOutputImage);

	bool Binarize(long Lower, long Upper, BOOL Exclude, SvOi::ITRCImagePtr pInputImage, SvOi::ITRCImagePtr pOutputImage);
	bool ValidateLocal() const;

	//! The Band should only use the values Band0 to Band2 never BandNumber as this is private this should never be a problem
	SvIe::SVImageClass& GetBandOutputImage(SvDef::BandEnum Band) { return m_BandThreshold[Band].m_OutputImage; };
#pragma endregion Private Methods

#pragma region Member Variables
private:
	// Contents the current pixel number, call
	// BOOL SVColorThreshold::Resize( SVExtentClass& RExtent );
	// to (re-)calculate...
	__int64 m_PixelNumber;

	// Embedded Objects for Train Color ROI Extents
	SvVol::SVDoubleValueObjectClass	m_ExtentLeft;
	SvVol::SVDoubleValueObjectClass	m_ExtentTop;
	SvVol::SVDoubleValueObjectClass	m_ExtentWidth;
	SvVol::SVDoubleValueObjectClass	m_ExtentHeight;
	
	SvVol::SVBoolValueObjectClass		m_OutputThresholdEnabled;

	SvIe::SVImageClass  m_OutputImage;
	BandThreshold m_BandThreshold[SvDef::BandEnum::BandNumber];
#pragma endregion Member Variables
};

} //namespace SvOp
