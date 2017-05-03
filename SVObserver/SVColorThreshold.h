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
#include "SVColorTool.h"
#include "SVDrawObject.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVOperator.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxImageResult.h"
#pragma endregion Includes


//! This structure holds the values and images required for each of the bands
struct BandThreshold
{
	SVLongValueObjectClass	m_UpperThreshold;		//! The upper threshold limit for the band
	SVLongValueObjectClass	m_LowerThreshold;		//! The lower threshold limit for the band
	SVBoolValueObjectClass	m_ThresholdExclude;		//! When this is set then the color is excluded

	SVInObjectInfoStruct	m_InputImage;			//! The structure to the input image (band image of the color tool)
	SVImageClass			m_OutputImage;			//! The result image of the threshold
	SVImageClass			m_HistogramImage;		//! The Histogram image
};

//! The class has the 3 bands from the color tool is input images
//! It has 3 Threshold images as an output
//! An extra output image which is a combined image of all 3 Threshold images as bitwise AND
class SVColorThresholdClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVColorThresholdClass );

#pragma region Constructor
public:
	SVColorThresholdClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCOLORTHRESHOLD );

	virtual ~SVColorThresholdClass();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVDrawObjectClass* GetGraphFigure(BandEnum Band);
	SVDrawObjectListClass* GetThresholdBarsFigure(BandEnum Band);
	SVImageClass& GetOutputImage() { return m_OutputImage; };
	BandThreshold* GetBandThreshold(BandEnum Band);

	void setShowHistogram(bool Show) { m_ShowHistogram = Show; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void LocalInitialize();

	bool createImages();
	bool createOutputImage(SVImageClass* pInputImage, SVImageClass* pOutputImage);
	bool createHistogramImage(SVImageClass* pInputImage, SVImageClass* pOutputImage);

	bool Binarize(long lower, long upper, BOOL bExclude, BandEnum Band);
	bool getHistogram(BandEnum Band);
	bool updateThresholdBars(long lMinThresholdValue, long lMaxThresholdValue, BandEnum Band);
	bool ValidateLocal() const;

	SVImageClass* GetBandInputImage(BandEnum Band);
	//! The Band should only use the values Band0 to Band2 never BandNumber as this is private this should never be a problem
	SVImageClass& GetBandHistogramImage(BandEnum Band) { return m_BandThreshold[Band].m_HistogramImage; };
	SVImageClass& GetBandOutputImage(BandEnum Band) { return m_BandThreshold[Band].m_OutputImage; };
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVMatroxImageResult m_HistogramResultID;
	SVMatroxLongArray  m_HistogramValueArray[BandEnum::BandNumber];
	int m_HistogramValueArraySize;

	// Contents the current pixel number, call
	// BOOL SVColorThresholdClass::Resize( SVExtentClass& RExtent );
	// to (re-)calculate...
	__int64 m_PixelNumber;

	// Embedded Objects for Train Color ROI Extents
	SVDoubleValueObjectClass	m_ExtentLeft;
	SVDoubleValueObjectClass	m_ExtentTop;
	SVDoubleValueObjectClass	m_ExtentWidth;
	SVDoubleValueObjectClass	m_ExtentHeight;

	SVImageClass  m_OutputImage;
	BandThreshold m_BandThreshold[BandEnum::BandNumber];

	SVDrawObjectClass	m_GraphFigures[BandEnum::BandNumber];
	SVDrawObjectListClass m_ThresholdBarFigures[BandEnum::BandNumber];

	bool m_ShowHistogram;
#pragma endregion Member Variables
};

