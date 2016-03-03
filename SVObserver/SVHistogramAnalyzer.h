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

#ifndef _SVANALYZERHISTOGRAM_H
#define _SVANALYZERHISTOGRAM_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"
#include "SVMFCControls\SVHistogram.h"

//class SVHistogramAnalyzerSetupClass;

class SVHistogramAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVHistogramAnalyzerClass );

	friend class SVSetupDialogManager;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVHistogramAnalyzerClass( LPCSTR ObjectName /* = "Histogram Analyzer" */ );
	SVHistogramAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVHISTOGRAMANALYZER );

public:
   long             msvlHistValueArraySize;

   SVLongValueObjectClass		msvHistogramValue;

	//SVDoubleValueObjectClass	msvVarianceValue;
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

	virtual BOOL   CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL   CloseObject();

   SVResultClass* GetResultObject(const GUID & guid);
   
	virtual BOOL   OnValidate ();

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
   
	//void setHistogramOptions(SVHistogramAnalyzerSetupClass &) const;
	//void getHistogramOptions(const SVHistogramAnalyzerSetupClass &);

	SvUl::SVHistogramBase m_histogram;
	void AddResult(const std::pair<GUID, DWORD> &);
	std::map<GUID, GUID> m_resultGuids;

protected:
   virtual BOOL   onRun( SVRunStatusClass& RRunStatus );

   SVMatroxImageResult        msvHistResultID;
	SVMatroxLongArray          msvplHistValues;
	SVImageClass					m_histogramImage;

	//SVHistogramStats	m_stats;
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
	bool m_accumulate;
	bool m_dynamicHeight;
};

#endif   // _SVANALYZERHISTOGRAM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHistogramAnalyzer.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:46:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Jul 2012 13:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 16:54:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

