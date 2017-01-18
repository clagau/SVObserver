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
#include "SVDrawObject.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVOperator.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxImageResult.h"
#pragma endregion Includes

class SVColorThresholdClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVColorThresholdClass );

public:
	SVColorThresholdClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCOLORTHRESHOLD );

	virtual ~SVColorThresholdClass();

private:
	void init();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual HRESULT ResetObject() override;

	SVImageClass* GetBand0InputImage();
	SVImageClass* GetBand1InputImage();
	SVImageClass* GetBand2InputImage();

	SVImageClass* GetBand0OutputImage();
	SVImageClass* GetBand1OutputImage();
	SVImageClass* GetBand2OutputImage();

	SVImageClass* GetOutputImage();

	SVImageClass* GetBand0HistogramImage();
	SVImageClass* GetBand1HistogramImage();
	SVImageClass* GetBand2HistogramImage();

	SVDrawObjectClass* GetGraphFigure( int bandNumber );	
	SVDrawObjectListClass* GetThresholdBarsFigure( int bandNumber );

	virtual BOOL OnValidate() override;

	BOOL bShowHistogram;

protected:
	BOOL createImages();
	BOOL createOutputImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage );
	BOOL createHistogramImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage );

	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;
	BOOL binarize( long lower, long upper, BOOL bExclude, SVImageClass* pInputImage, SVImageClass* pOutputImage );
	BOOL getHistogram( SVImageClass* pInputImage, SVMatroxLongArray& pHistValues, SVDrawObjectClass* pGraphFigure );
	BOOL updateThresholdBars( SVImageClass* pImage, SVDrawObjectListClass* pThresholdBarFigure, long lMinThresholdValue, long lMaxThresholdValue );

	SVMatroxImageResult histResultID;
	SVMatroxLongArray  aHistValueArray[3];
	int    histValueArraySize;

	// Contents the current pixel number, call
	// BOOL SVColorThresholdClass::Resize( SVExtentClass& RExtent );
	// to (re-)calculate...
	__int64 pixelNumber;

	// Embedded Objects for Band 0 Thresholds
	SVLongValueObjectClass	band0UpperThreshold;
	SVLongValueObjectClass	band0LowerThreshold;
	SVBoolValueObjectClass	band0ThresholdExclude;

	// Embedded Objects for Band 1 Thresholds
	SVLongValueObjectClass	band1UpperThreshold;
	SVLongValueObjectClass	band1LowerThreshold;
	SVBoolValueObjectClass	band1ThresholdExclude;

	// Embedded Objects for Band 2 Thresholds
	SVLongValueObjectClass	band2UpperThreshold;
	SVLongValueObjectClass	band2LowerThreshold;
	SVBoolValueObjectClass	band2ThresholdExclude;
	
	// Embedded Objects for Train Color ROI Extents
	SVDoubleValueObjectClass	extentLeft;
	SVDoubleValueObjectClass	extentTop;
	SVDoubleValueObjectClass	extentWidth;
	SVDoubleValueObjectClass	extentHeight;

	// Inputs...
	SVInObjectInfoStruct	inputBand0Image;
	SVInObjectInfoStruct	inputBand1Image;
	SVInObjectInfoStruct	inputBand2Image;
	
	// 4 Output Images
	SVImageClass			band0OutputImage;
	SVImageClass			band1OutputImage;
	SVImageClass			band2OutputImage;
	SVImageClass			outputImage;

	// 3 images for the Histograms
	SVImageClass			band0HistogramImage;
	SVImageClass			band1HistogramImage;
	SVImageClass			band2HistogramImage;

private:
	SVDrawObjectClass	graphFigures[3];
	SVDrawObjectListClass thresholdBarFigures[3];
};

