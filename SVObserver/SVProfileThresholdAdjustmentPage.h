//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileThresholdAdjustmentPage
//* .File Name       : $Workfile:   SVProfileThresholdAdjustmentPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:37:24  $
//******************************************************************************

#ifndef SVPROFILETHRESHOLDADJUSTMENTPAGE_H
#define SVPROFILETHRESHOLDADJUSTMENTPAGE_H

#include "SVThresholdAdjustmentPageClass.h"
#include "SVLineAnalyzer.h"
#include "SVDlgImageGraph.h"

class SVImageToLineProjectClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVProfileThresholdAdjustmentPageClass 
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
//  :27.01.2000 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVProfileThresholdAdjustmentPageClass : public SVThresholdAdjustmentPageClass
{
	DECLARE_DYNCREATE(SVProfileThresholdAdjustmentPageClass)

public:
	virtual HRESULT SetInspectionData();

	//{{AFX_DATA(SVProfileThresholdAdjustmentPageClass)
	enum { IDD = IDD_PROFILE_GR_THRESHOLD_ADJUST_DIALOG };
	CEdit	m_LowerMinOffsetEditCtrl;
	CEdit	m_LowerMaxOffsetEditCtrl;
	CEdit	m_LowerMaxDiffEditCtrl;
	CEdit	m_UpperMinOffsetEditCtrl;
	CEdit	m_UpperMaxOffsetEditCtrl;
	CEdit	m_UpperMaxDiffEditCtrl;
	BYTE	m_lowerThresholdMaxOffset;
	BYTE	m_lowerThresholdMinOffset;
	BYTE	m_lowerThresholdMaxPercentDiff;
	BYTE	m_upperThresholdMaxOffset;
	BYTE	m_upperThresholdMinOffset;
	BYTE	m_upperThresholdMaxPercentDiff;
	int		m_lowerThresholdOption;
	int		m_upperThresholdOption;
	SVDlgImageGraphClass	dialogImage;
	//}}AFX_DATA

protected:
	//{{AFX_MSG(SVProfileThresholdAdjustmentPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpperUserSelectable();
	afx_msg void OnLowerUserSelectable();
	afx_msg void OnUpperUseMaxMinusDiff();
	afx_msg void OnUpperUseMaxMinusOffset();
	afx_msg void OnUpperUseMinPlusOffset();
	afx_msg void OnLowerUseMaxMinusDiff();
	afx_msg void OnLowerUseMaxMinusOffset();
	afx_msg void OnLowerUseMinPlusOffset();
	afx_msg void OnChangeUpperPercentDiff();
	afx_msg void OnChangeUpperMinOffset();
	afx_msg void OnChangeUpperMaxOffset();
	afx_msg void OnChangeLowerPercentDiff();
	afx_msg void OnChangeLowerMinOffset();
	afx_msg void OnChangeLowerMaxOffset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVProfileThresholdAdjustmentPageClass( UINT nIDCaption = 0 );

public:
	~SVProfileThresholdAdjustmentPageClass();

	//{{AFX_VIRTUAL(SVProfileThresholdAdjustmentPageClass)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

protected:
	void getThresholdParams();
	void setThresholdParams();
	void updateControls();
	void setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max );
	void setScrollPos( CSliderCtrl* pSliderCtrl, int pos );
	void updateGraphDisplay();
	void setInitialValues();

protected:
	SVImageToLineProjectClass* pProject;

	// Normalizer for Lower threshold
	SVValueBaseNormalizerClass LowerNormalizer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio 

#endif	

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVProfileThresholdAdjustmentPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:37:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Aug 2005 09:53:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Aug 2005 08:08:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Dec 2003 13:13:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 14:19:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Nov 2002 11:07:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jun 2000 13:57:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  158
 * SCR Title:  Profile Tool Lower Threshold Setup problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVValueBaseNormalizer for the lower threshold.
 * Revised OnInitDialog to use both normalizer variables.
 * Revised OnUpperUserSelectable to update the display.
 * Revised OnLowerUserSelectable to update the display.
 * Revised OnHScroll to use both Normalizers.
 * Revised SetScrollRange to use both Normalizers.
 * Revised SetScrollPos to use both Normalizers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Apr 2000 19:01:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added include for SVDlgImageGraph.h.
 * Added SVDlgImageGraph control to the dialog.
 * Revised OnInitDialog to set points (figures) for the 
 * SVDlgImageGraph control.
 * Revised updateGraphDisplay to call
 * SVDlgImageGraph::RefreshPoints method.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:11:52   sjones
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