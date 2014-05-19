// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearAnalyzerClass
// * .File Name       : $Workfile:   SVLinearAnalyzerClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   15 May 2014 13:14:06  $
// ******************************************************************************

#ifndef _SVLINEARANALYZERCLASS_H
#define _SVLINEARANALYZERCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVAnalyzer.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVValueObjectImpl.h"

class SVLinearEdgeProcessingClass;

class SVLinearAnalyzerClass : public SVAnalyzerClass  
{
	SV_DECLARE_CLASS( SVLinearAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVLinearAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARANALYZER );
	virtual ~SVLinearAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	virtual HRESULT GetThresholdBarsOverlay( SVLinearEdgeProcessingClass *p_psvEdge, SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetHistogramOverlay( SVLinearEdgeProcessingClass *p_psvEdge, SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetEdgesOverlay( SVLinearEdgeProcessingClass *p_psvEdge, SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent );

	SVLinearEdgeProcessingClass *GetEdgeA();
	SVLinearEdgeProcessingClass *GetEdgeB();

	SVImageClass *GetInputImage();

	HRESULT GetPixelDepth();

	SVBoolValueObjectClass m_svShowAllEdgeAOverlays;
	SVBoolValueObjectClass m_svShowAllEdgeBOverlays;

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	HRESULT GetProjectedExtent( long p_lIndex, long &p_rlBottom, SVImageExtentClass &p_rsvImageExtent );

	SVEnumerateValueObjectClass *GetInputProfileOrientation();
	SVBoolValueObjectClass *GetInputUseRotationAngle();

	SVInObjectInfoStruct m_svInputImageObjectInfo;
	SVInObjectInfoStruct m_svInputProfileOrientation;
	SVInObjectInfoStruct m_svInputUseRotationAngle;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;
	DWORD m_dwColorNumber;

	BOOL m_bEnableDirection;
	BOOL m_bEnableEdgeSelect;
	BOOL m_bEnablePolarisation;
	BOOL m_bEnablePosition;
	BOOL m_bEnableThreshold;

	SVValueBaseNormalizerClass m_svNormalizer;

};

#endif // _SVLINEARANALYZERCLASS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLinearAnalyzerClass.h_v  $
 * 
 *    Rev 1.2   15 May 2014 13:14:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revsied processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 11:13:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:45:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Aug 2012 12:53:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Jul 2012 14:27:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Jul 2012 13:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 17:15:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 16:07:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Mar 2005 11:19:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added processmessage so that the ResetObject would get called.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
