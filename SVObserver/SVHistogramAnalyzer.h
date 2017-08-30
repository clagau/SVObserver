//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramAnalyzerClass
//* .File Name       : $Workfile:   SVHistogramAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageAnalyzerClass.h"
#include "SVMFCControls\SVHistogram.h"
#include "SVOCore/SVImageClass.h"
#pragma endregion Includes

class SVHistogramAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVHistogramAnalyzerClass );

	friend class SVSetupDialogManager;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVHistogramAnalyzerClass( LPCSTR ObjectName /* = "Histogram Analyzer" */ );
	SVHistogramAnalyzerClass( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVHISTOGRAMANALYZER );

public:
   long             msvlHistValueArraySize;

   SVLongValueObjectClass		msvHistogramValue;

	SVDoubleValueObjectClass	msvStdDevValue;
	SVDoubleValueObjectClass	msvMeanValue;

	SVLongValueObjectClass		msvRangeStartValue;
	SVLongValueObjectClass		msvRangeEndValue;
	SVLongValueObjectClass		msvMaxPixelValue;
	SVLongValueObjectClass		msvMinPixelValue;
	SVLongValueObjectClass		msvPixelCountValue;
	SVLongValueObjectClass		msvRangeSizeValue;
	SVLongValueObjectClass		msvHighPeak;
	SVLongValueObjectClass		msvLowPeak;
	SVLongValueObjectClass		msvValley;
	SVLongValueObjectClass		msvPeakThreshold;
	SVLongValueObjectClass		msvDefaultPeak;
	SVLongValueObjectClass		msvScaleValue;
	SVLongValueObjectClass		msvValleyLowerBound;
	SVLongValueObjectClass		msvValleyUpperBound;
	SVLongValueObjectClass		msvValleyDefault;

	SVDoubleValueObjectClass		msvMinPeakHeight;
	SVDoubleValueObjectClass		msvFixedHeightValue;

	SVBoolValueObjectClass		msvAccumulateCounts;
	SVBoolValueObjectClass		msvDynamicHeight;

	virtual        ~SVHistogramAnalyzerClass();

	virtual bool CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;

	virtual bool CloseObject() override;

	SvOi::IObjectClass* GetResultObject(const GUID & guid);
   
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	HRESULT createHistogramImage();

	HRESULT paintHistogramImage();

	const SVMatroxLongArray & GetHistogram() const
	{
		return msvplHistValues;
	}

	bool calcHistogram();
	void DisconnectImages();

private:
   void init();
   
	SvUl::SVHistogramBase m_histogram;
	void AddResult(const std::pair<GUID, DWORD> &);
	std::map<GUID, GUID> m_resultGuids;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVMatroxImageResult msvHistResultID;
	SVMatroxLongArray   msvplHistValues;
	SVImageClass		m_histogramImage;

	long m_rangeStart;
	long m_rangeEnd;
	long m_scale;
	long m_threshold;
	long m_default;
	long m_lowValley;
	long m_highValley;
	long m_valleyDefault;

	double m_minHeight;
	double m_fixedHeight;

	bool m_useDefault;
	BOOL m_accumulate;
	BOOL m_dynamicHeight;
};

