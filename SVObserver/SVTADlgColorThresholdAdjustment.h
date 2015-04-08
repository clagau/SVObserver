//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdAdjustment
//* .File Name       : $Workfile:   SVTADlgColorThresholdAdjustment.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:56  $
//******************************************************************************

#if !defined(SVTADLGCOLORTHRESHOLDADJUSTMENT_H)
#define SVTADLGCOLORTHRESHOLDADJUSTMENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTADlgColorThresholdBasePage.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVView.h"

#include "SVTaskObjectInterfaceClass.h"
#include "SVMFCControls/SVSliderCtrl.h"

class SVIPDoc;
class SVColorToolClass;
class SVColorThresholdClass;
class SVLongValueObjectClass;
class SVBoolValueObjectClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdAdjustment dialog

class SVTADlgColorThresholdAdjustment : public SVTADlgColorThresholdBasePage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdAdjustment)

// Construction
public:
	SVTADlgColorThresholdAdjustment();
	~SVTADlgColorThresholdAdjustment();

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdAdjustment)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ADJUST_DIALOG };
	CComboBox	m_cbZoom;
	SVSliderCtrl	UpperSliderCtrl;
	SVSliderCtrl	LowerSliderCtrl;
	CString	StrUpper;
	CString	StrLower;
	BYTE	m_lowerThreshold;
	BYTE	m_upperThreshold;
	BOOL	m_exclude;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdAdjustment)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdAdjustment)
	virtual BOOL OnInitDialog();
	afx_msg void OnExcludeColorThreshold();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void getThresholdParams();
	void setThresholdParams();
	void updateControls();
	void setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max );
	void setScrollPos( CSliderCtrl* pSliderCtrl, int pos );
	void updateGraphDisplay();
	void setInitialValues();
	void RefreshProperties();

public:
	int mBandNumber;

protected:
	SVIPDoc*                mpCurrentDocument;
	SVColorThresholdClass*  mpThreshold;
	SVLongValueObjectClass* mpUpperThreshold;
	SVLongValueObjectClass* mpLowerThreshold;
	SVBoolValueObjectClass* mpExclude;

	SVValueBaseNormalizerClass m_Normalizer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTADLGCOLORTHRESHOLDADJUSTMENT_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgColorThresholdAdjustment.h_v  $
 * 
 *    Rev 1.1   26 Jun 2014 18:17:56   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   remove unused SVDlgImageGraph.h and SVDlgImageEditor.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:19:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Aug 2006 15:48:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Zoom / overlay bug
 * changed histogram scaling
 * added ROI feedback on threshold adjust pages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Aug 2006 16:03:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Aug 2005 08:15:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 15:43:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:24:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:35:28   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:41:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/