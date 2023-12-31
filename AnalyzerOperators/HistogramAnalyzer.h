//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HistogramAnalyzer
//* .File Name       : $Workfile:   HistogramAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageAnalyzer.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVUtilityLibrary/SVHistogramBase.h"
#pragma endregion Includes

namespace SvAo
{

class HistogramAnalyzer : public ImageAnalyzer
{
	SV_DECLARE_CLASS

	friend class SVSetupDialogManager;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	explicit HistogramAnalyzer( LPCSTR ObjectName /* = "Histogram Analyzer" */ );
	HistogramAnalyzer( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVHISTOGRAMANALYZER );

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

	virtual        ~HistogramAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool CloseObject() override;

	SvOi::IObjectClass* GetResultObject(SvPb::EmbeddedIdEnum embeddedID);
   
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	HRESULT createHistogramImage();

	HRESULT paintHistogramImage(const SvOi::ITriggerRecordRWPtr& pTriggerRecord);

	const std::vector<long>& GetHistogram() const
	{
		return msvplHistValues;
	}

private:
   void init();
   
	SvUl::SVHistogramBase m_histogram;
	void AddResult(const std::pair<SvPb::EmbeddedIdEnum, DWORD> &);

protected:
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	__int64 m_HistResultID = 0LL;
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
