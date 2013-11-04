//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDualLineAnalyzer
//* .File Name       : $Workfile:   SVDualLineAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:16  $
//******************************************************************************

#ifndef SVDUALLINEANALYZER_H
#define SVDUALLINEANALYZER_H

#include "SVLineAnalyzer.h"
#include "SVValueObjectImpl.h"

class SVDualLineAnalyzerClass : public SVLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVDualLineAnalyzerClass );

public:
	SVDualLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARMEASUREMENTANALYZER );

	virtual ~SVDualLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL OnValidate();

	// Special to restrict outputs for the gage tool
	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual void DisableThresholdOutputs();

	virtual HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	BOOL CalculateThresholdValues( DWORD& DWUpperThresholdValueA, DWORD& DWLowerThresholdValueA, DWORD& DWUpperThresholdValueB, DWORD& DWLowerThresholdValueB );
	BOOL GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue );

protected:

	void SetCalculatedThresholdPrintableFlags();

	SVPointValueObjectClass edgeA;	// Point
	SVPointValueObjectClass edgeB;	// Point

	// New threshold flags and variables
	// Edge A
	SVBoolValueObjectClass	m_voUseLowerThresholdSelectableA;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusPercentDiffA;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusOffsetA;
	SVBoolValueObjectClass	m_voUseLowerThresholdMinPlusOffsetA;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusPercentDiffA;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusOffsetA;
	SVDWordValueObjectClass	m_voLowerThresholdMinPlusOffsetA;

	SVBoolValueObjectClass	m_voUseUpperThresholdSelectableA;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusPercentDiffA;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusOffsetA;
	SVBoolValueObjectClass	m_voUseUpperThresholdMinPlusOffsetA;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusPercentDiffA;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusOffsetA;
	SVDWordValueObjectClass	m_voUpperThresholdMinPlusOffsetA;

	// Edge B
	SVBoolValueObjectClass	m_voUseLowerThresholdSelectableB;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusPercentDiffB;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusOffsetB;
	SVBoolValueObjectClass	m_voUseLowerThresholdMinPlusOffsetB;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusPercentDiffB;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusOffsetB;
	SVDWordValueObjectClass	m_voLowerThresholdMinPlusOffsetB;

	SVBoolValueObjectClass	m_voUseUpperThresholdSelectableB;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusPercentDiffB;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusOffsetB;
	SVBoolValueObjectClass	m_voUseUpperThresholdMinPlusOffsetB;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusPercentDiffB;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusOffsetB;
	SVDWordValueObjectClass	m_voUpperThresholdMinPlusOffsetB;

	// Input: DWORD - Min Projection Threshold
	SVInObjectInfoStruct		inputMinThresholdObjectInfo;

	// Input: DWORD - Max Projection Threshold
	SVInObjectInfoStruct		inputMaxThresholdObjectInfo;

	//Value objects for Edge Parameters
	SVEnumerateValueObjectClass	m_voEdgeADirection;
	SVEnumerateValueObjectClass	m_voEdgeAPolarisation;
	SVEnumerateValueObjectClass	m_voEdgeAEdgeSelect;
	SVDoubleValueObjectClass	m_voEdgeAEdgeSelectThisValue;

	SVBoolValueObjectClass		m_voEdgeAIsFixedEdgeMarker;
	SVEnumerateValueObjectClass	m_voEdgeAPosition;
	SVDoubleValueObjectClass	m_voEdgeAPositionOffsetValue;

	SVDWordValueObjectClass		m_voEdgeALowerThresholdValue;
	SVDWordValueObjectClass		m_voEdgeAUpperThresholdValue;

	SVEnumerateValueObjectClass	m_voEdgeBDirection;
	SVEnumerateValueObjectClass	m_voEdgeBPolarisation;
	SVEnumerateValueObjectClass	m_voEdgeBEdgeSelect;
	SVDoubleValueObjectClass	m_voEdgeBEdgeSelectThisValue;

	SVBoolValueObjectClass		m_voEdgeBIsFixedEdgeMarker;
	SVEnumerateValueObjectClass	m_voEdgeBPosition;
	SVDoubleValueObjectClass	m_voEdgeBPositionOffsetValue;

	SVDWordValueObjectClass		m_voEdgeBLowerThresholdValue;
	SVDWordValueObjectClass		m_voEdgeBUpperThresholdValue;

private:
	void init();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDualLineAnalyzer.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:23:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Aug 2012 12:49:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   25 Jul 2012 14:04:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Jul 2012 13:31:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Aug 2005 15:42:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   17 Feb 2005 13:59:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Apr 2004 12:07:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method SetCalculatedThresholdPrintableFlags()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Dec 2003 12:15:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Oct 2003 16:23:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Oct 2003 10:53:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added value objects for EdgeA and EdgeB
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:16:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 19:17:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added CreateObject method to class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2000 12:09:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration of init() method to be private and not virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Mar 2000 16:40:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed hideVariable method 
 * (now SVTaskObjectClass::hideEmbeddedObject)
 * Revised DisableThresholdOutput to call 
 * hideEmbeddedObject instead of
 * hideVariable
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:14:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. 
 * New Intermediate base class for Line Analyzers, contains new thresholding options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
