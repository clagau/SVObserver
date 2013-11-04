//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileEdgeMarkerAdjustmentPage
//* .File Name       : $Workfile:   SVProfileEdgeMarkerAdjustmentPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:21:08  $
//******************************************************************************

#ifndef SVPROFILEEDGEMARKERADJUSTMENTPAGE_H
#define SVPROFILEEDGEMARKERADJUSTMENTPAGE_H

#include "SVEdgeMarkerAdjustmentPageClass.h"
#include "SVLineAnalyzer.h"
#include "SVDlgImageGraph.h"

class SVImageToLineProjectClass;
class SVLinearEdgeProcessingClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVProfileEdgeMarkerAdjustmentPageClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :06.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVProfileEdgeMarkerAdjustmentPageClass : public SVEdgeMarkerAdjustmentPageClass
{
	DECLARE_DYNCREATE(SVProfileEdgeMarkerAdjustmentPageClass)

public:
	SVProfileEdgeMarkerAdjustmentPageClass( UINT nIDCaption = 0 );
	~SVProfileEdgeMarkerAdjustmentPageClass();

	virtual HRESULT GetInspectionData();
	virtual HRESULT SetInspectionData();

	//{{AFX_DATA(SVProfileEdgeMarkerAdjustmentPageClass)
	enum { IDD = IDD_PROFILE_GR_EDGE_MARKER_ADJUST_DIALOG };
	CEdit	m_LowerMinOffsetEditCtrl;
	CEdit	m_LowerMaxOffsetEditCtrl;
	CEdit	m_LowerMaxDiffEditCtrl;
	CEdit	m_UpperMinOffsetEditCtrl;
	CEdit	m_UpperMaxOffsetEditCtrl;
	CEdit	m_UpperMaxDiffEditCtrl;
	BYTE	m_lowerThresholdMaxOffset;
	BYTE	m_lowerThresholdMinOffset;
	int		m_lowerThresholdMaxPercentDiff;
	BYTE	m_upperThresholdMaxOffset;
	BYTE	m_upperThresholdMinOffset;
	int 	m_upperThresholdMaxPercentDiff;
	int		m_lowerThresholdOption;
	int		m_upperThresholdOption;
	SVDlgImageGraphClass	dialogImage;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SVProfileEdgeMarkerAdjustmentPageClass)
	public:
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

protected:
	virtual HRESULT GetSliderData( double p_dLower, double p_dUpper );

	//{{AFX_MSG(SVProfileEdgeMarkerAdjustmentPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpperUserSelectable();
	afx_msg void OnLowerUserSelectable();
	afx_msg void OnUpperUseMaxMinusDiff();
	afx_msg void OnUpperUseMaxMinusOffset();
	afx_msg void OnUpperUseMinPlusOffset();
	afx_msg void OnLowerUseMaxMinusDiff();
	afx_msg void OnLowerUseMaxMinusOffset();
	afx_msg void OnLowerUseMinPlusOffset();
	afx_msg void OnChangeUpperPercentDiff();
	afx_msg void OnChangeUpperMaxOffset();
	afx_msg void OnChangeUpperMinOffset();
	afx_msg void OnChangeLowerPercentDiff();
	afx_msg void OnChangeLowerMaxOffset();
	afx_msg void OnChangeLowerMinOffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void updateControls();
	void updateGraphDisplay();
	void setInitialValues();

	SVImageToLineProjectClass *pProject;
	SVLinearEdgeProcessingClass	*m_pEdge;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio 

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVProfileEdgeMarkerAdjustmentPage.h_v  $
 * 
 *    Rev 1.1   07 May 2013 08:21:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:37:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Aug 2005 13:53:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to fix issues with data synchrization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   09 Aug 2005 08:08:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 15:22:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Dec 2003 13:13:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 14:19:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2002 11:07:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Sep 2000 16:34:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jun 2000 13:46:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  158
 * SCR Title:  Profile Tool Lower Threshold Setup problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added a seperate SVValueBAseNormalizerClass for the Lower Threshold.
 * Revised OnInitDialog to call initEdgeParameters.
 * Revised OnHScroll to use LowerNormalizer
 * Revised OnUpperUserSelectable to update the display.
 * Revised OnLowerUserSelectable to update the display.
 * Revised SetScrollRange to use the uper or lower normalizer.
 * Revised SetScrollPos to use the uper or lower normalizer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Apr 2000 19:00:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added include for SVDlgImageGraph.h.
 * Added SVDlgImageGraph control to the dialog.
 * Revised OnInitDialog to set points (figures) for the 
 * SVDlgImageGraph control.
 * Revised OnSetActive to call 
 * SVDlgImageGraph::RefreshPoints method.
 * Revised UpdateGraphDisplay to call
 * SVDlgImageGraph::RefreshPoints method.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:11:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. New Dialog class for setup of thresholding
 * options for the line analyzers in the profile tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/