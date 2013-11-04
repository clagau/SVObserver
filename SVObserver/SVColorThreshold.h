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

#ifndef SVCOLORTHRESHOLD_H
#define SVCOLORTHRESHOLD_H

#include "SVDrawObject.h"
#include "SVImageClass.h"
#include "SVOperator.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxImageResult.h"

class SVColorThresholdClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVColorThresholdClass );

public:
	SVColorThresholdClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCOLORTHRESHOLD );

	virtual ~SVColorThresholdClass();

private:
	void init();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	void GetTrainColorROIExtent( CRect &p_roRect );

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

	virtual BOOL OnValidate();

	BOOL bShowHistogram;

protected:
	BOOL createImages();
	BOOL createOutputImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage );
	BOOL createHistogramImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage );

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVColorThreshold.h_v  $
 * 
 *    Rev 1.1   13 Aug 2013 09:47:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Matrox includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:54:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Mar 2011 10:13:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   24 Jul 2007 11:26:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Feb 2007 11:44:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.  Also removed the un-necessary string attribute to hold the last error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Aug 2005 15:31:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove unused method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 13:24:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Apr 2003 17:16:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jan 2003 12:44:56   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, createOutputImage, createHistogramImage, onRun, binarize, getHistogram and updateThresholdBars to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Apr 17 2001 15:22:20   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:39:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
