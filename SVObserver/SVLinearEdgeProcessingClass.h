// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeProcessingClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   15 May 2014 13:12:48  $
// ******************************************************************************

#ifndef SVLINEAREDGEPROCESSINGCLASS_H
#define SVLINEAREDGEPROCESSINGCLASS_H

#include "SVTaskObject.h"
#include "SVValueObjectImpl.h"

class SVLinearEdgeProcessingClass : public SVTaskObjectClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeProcessingClass );

public:
	SVLinearEdgeProcessingClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPROCESSING );
	virtual ~SVLinearEdgeProcessingClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct *PCreateStructure);

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();
	virtual BOOL onRun( SVRunStatusClass &p_rsvRunStatus );

	SVImageClass*             GetInputImage();
	SVDoubleValueObjectClass* GetInputMinThreshold();
	SVDoubleValueObjectClass* GetInputMaxThreshold();
	SVDoubleValueObjectClass* GetInputLinearData();

	HRESULT GetPixelDepth();

	HRESULT GetPointFromDistance( double p_dDistance, SVExtentPointStruct &p_rsvPoint );
	HRESULT GetEdgeOverlayFromDistance( double p_dDistance, SVExtentLineStruct &p_rsvLine );

	HRESULT GetOutputEdgeDistance( double &p_rdValue );
	HRESULT GetOutputEdgePoint( SVExtentPointStruct &p_rsvPoint );

	HRESULT GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	HRESULT GetHistogramOverlay( SVExtentLineStruct &p_rsvLine );
	HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	HRESULT GetSelectedEdgeOverlay( SVExtentLineStruct &p_rsvLine );

	SVEnumerateValueObjectClass m_svDirection;
	SVEnumerateValueObjectClass m_svPolarisation;
	SVEnumerateValueObjectClass m_svEdgeSelect;
	SVDoubleValueObjectClass m_svEdgeSelectThisValue;

	SVBoolValueObjectClass m_svIsFixedEdgeMarker;
	SVEnumerateValueObjectClass m_svPosition;
	SVDoubleValueObjectClass m_svPositionOffsetValue;

	SVBoolValueObjectClass	m_svUseLowerThresholdSelectable;
	SVBoolValueObjectClass	m_svUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass	m_svUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass	m_svUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass m_svLowerThresholdValue;
	SVDWordValueObjectClass m_svLowerMaxMinusPercentDiffValue;
	SVDWordValueObjectClass m_svLowerMaxMinusOffsetValue;
	SVDWordValueObjectClass m_svLowerMinPlusOffsetValue;

	SVBoolValueObjectClass	m_svUseUpperThresholdSelectable;
	SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass	m_svUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass m_svUpperThresholdValue;
	SVDWordValueObjectClass m_svUpperMaxMinusPercentDiffValue;
	SVDWordValueObjectClass m_svUpperMaxMinusOffsetValue;
	SVDWordValueObjectClass m_svUpperMinPlusOffsetValue;

	SVDoubleValueObjectClass m_svLinearEdges;

	DWORD m_dwColorNumber;

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	HRESULT UpdateUpperThresholdValues( long p_lIndex );
	HRESULT UpdateLowerThresholdValues( long p_lIndex );
	HRESULT UpdateEdgeList( long p_lIndex );
	HRESULT IsEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation );
	HRESULT GetBlackWhiteEdgeValue( double p_dCurrent, DWORD p_dwUpper, DWORD p_dwLower, double &l_rdValue );
	HRESULT CalculateSubPixelEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation, double &p_rdDistance );

	SVInObjectInfoStruct m_svInputImageObjectInfo;
	SVInObjectInfoStruct m_svInputMinThreshold;
	SVInObjectInfoStruct m_svInputMaxThreshold;
	SVInObjectInfoStruct m_svInputLinearData;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;

	COLORREF m_cfThresholds;
	COLORREF m_cfHistogram;
	COLORREF m_cfEdges;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLinearEdgeProcessingClass.h_v  $
 * 
 *    Rev 1.2   15 May 2014 13:12:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 11:18:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:53:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Aug 2012 12:53:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Aug 2005 16:07:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Aug 2005 11:59:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Mar 2005 11:19:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added processmessage so that the ResetObject would get called.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Mar 2005 14:47:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated methods to fix problems with edge calculation methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
