//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSingleLineAnalyzer
//* .File Name       : $Workfile:   SVSingleLineAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:07:06  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVSINGLELINEANALYZER_H
#define SVSINGLELINEANALYZER_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVLineAnalyzer.h"
#include "SVValueObjectImpl.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVSingleLineAnalyzerClass : public SVLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVSingleLineAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVSingleLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVEDGECOUNTINGLINEANALYZER );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVSingleLineAnalyzerClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStructure);

	// Special to restrict outputs for the gage tool
	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual void DisableThresholdOutputs();

private:
	void init();

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	BOOL CalculateThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue );
	BOOL GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue );

protected:

	void SetCalculatedThresholdPrintableFlags();

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:	
	// New threshold flags and variables
	SVBoolValueObjectClass		m_voUseLowerThresholdSelectable;
	SVBoolValueObjectClass		m_voUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		m_voUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass		m_voUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass		m_voLowerThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		m_voLowerThresholdMaxMinusOffset;
	SVDWordValueObjectClass		m_voLowerThresholdMinPlusOffset;

	SVBoolValueObjectClass		m_voUseUpperThresholdSelectable;
	SVBoolValueObjectClass		m_voUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		m_voUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass		m_voUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass		m_voUpperThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		m_voUpperThresholdMaxMinusOffset;
	SVDWordValueObjectClass		m_voUpperThresholdMinPlusOffset;

	// Input: DWORD - Min Projection Threshold
	SVInObjectInfoStruct		inputMinThresholdObjectInfo;

	// Input: DWORD - Max Projection Threshold
	SVInObjectInfoStruct		inputMaxThresholdObjectInfo;

	//value objects for the edge parameters
	SVEnumerateValueObjectClass	m_voEdgeADirection;
	SVEnumerateValueObjectClass	m_voEdgeAPolarisation;
	SVEnumerateValueObjectClass	m_voEdgeAEdgeSelect;
	SVDoubleValueObjectClass	m_voEdgeAEdgeSelectThisValue;

	SVBoolValueObjectClass		m_voEdgeAIsFixedEdgeMarker;
	SVEnumerateValueObjectClass	m_voEdgeAPosition;
	SVDoubleValueObjectClass	m_voEdgeAPositionOffsetValue;

	SVDWordValueObjectClass		m_voEdgeALowerThresholdValue;
	SVDWordValueObjectClass		m_voEdgeAUpperThresholdValue;
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVSINGLELINEANALYZER_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSingleLineAnalyzer.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:07:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Aug 2005 08:11:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
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
 *    Rev 1.7   04 Dec 2003 12:15:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Oct 2003 16:23:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Oct 2003 11:10:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put value object for EdgeA parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 15:34:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 20:07:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added CreateObject method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2000 12:11:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration of init() method to be private and not virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Mar 2000 16:40:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed hideVariable method 
 * (now SVTaskObjectClass::hideEmbeddedObject)
 * Revised DisableThresholdOutput to call 
 * hideEmbeddedObject instead of
 * hideVariable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:14:36   sjones
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