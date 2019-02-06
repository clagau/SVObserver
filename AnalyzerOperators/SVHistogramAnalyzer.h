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
#include "InspectionEngine/SVImageClass.h"
#include "SVUtilityLibrary/SVHistogramBase.h"
#pragma endregion Includes

namespace SvAo
{

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

   SvVol::SVLongValueObjectClass		msvHistogramValue;

   SvVol::SVDoubleValueObjectClass	msvStdDevValue;
   SvVol::SVDoubleValueObjectClass	msvMeanValue;

   SvVol::SVLongValueObjectClass msvRangeStartValue;
   SvVol::SVLongValueObjectClass msvRangeEndValue;
   SvVol::SVLongValueObjectClass msvMaxPixelValue;
   SvVol::SVLongValueObjectClass msvMinPixelValue;
   SvVol::SVLongValueObjectClass msvPixelCountValue;
   SvVol::SVLongValueObjectClass msvRangeSizeValue;
   SvVol::SVLongValueObjectClass msvHighPeak;
   SvVol::SVLongValueObjectClass msvLowPeak;
   SvVol::SVLongValueObjectClass msvValley;
   SvVol::SVLongValueObjectClass msvPeakThreshold;
   SvVol::SVLongValueObjectClass msvDefaultPeak;
   SvVol::SVLongValueObjectClass msvScaleValue;
   SvVol::SVLongValueObjectClass msvValleyLowerBound;
   SvVol::SVLongValueObjectClass msvValleyUpperBound;
   SvVol::SVLongValueObjectClass msvValleyDefault;

	SvVol::SVDoubleValueObjectClass		msvMinPeakHeight;
	SvVol::SVDoubleValueObjectClass		msvFixedHeightValue;

	SvVol::SVBoolValueObjectClass		msvAccumulateCounts;
	SvVol::SVBoolValueObjectClass		msvDynamicHeight;

	virtual        ~SVHistogramAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool CloseObject() override;

	SvOi::IObjectClass* GetResultObject(const GUID & guid);
   
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	HRESULT createHistogramImage();

	HRESULT paintHistogramImage(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord);

	const std::vector<long>& GetHistogram() const
	{
		return msvplHistValues;
	}

private:
   void init();
   
	SvUl::SVHistogramBase m_histogram;
	void AddResult(const std::pair<GUID, DWORD> &);
	std::map<GUID, GUID> m_resultGuids;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVMatroxIdentifier m_HistResultID = M_NULL;
	std::vector<long>   msvplHistValues;
	SvIe::SVImageClass	m_histogramImage;

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

} //namespace SvAo
