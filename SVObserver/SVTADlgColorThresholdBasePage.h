//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVTADlgColorThresholdBasePage
//* .File Name       : $Workfile:   SVTADlgColorThresholdBasePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:19:56  $
//******************************************************************************

#ifndef SVTADLGCOLORTHRESHOLDBASEPAGE_H_
#define SVTADLGCOLORTHRESHOLDBASEPAGE_H_
#pragma once


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdBasePage dialog

#include "SVDlgImageEditor.h"
#include "SVROIFigureEditor.h"

class SVTADlgColorThresholdSheet;
class SVColorToolClass;
class SVColorThresholdClass;
class SVDoubleValueObjectClass;

class SVTADlgColorThresholdBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdBasePage)

// Construction
public:
	SVTADlgColorThresholdBasePage() {}	// dummy for DYNCREATE macros
	SVTADlgColorThresholdBasePage(UINT nIDTemplate);
	~SVTADlgColorThresholdBasePage();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdBasePage)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ROI };
	CComboBox	m_cbZoom;
	//}}AFX_DATA
	SVDlgImageEditor  m_svDlgImage;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdBasePage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	SVTADlgColorThresholdSheet* m_pSheet;
protected:

	SVColorToolClass*         mpTool;
	SVColorThresholdClass*    mpThreshold;
	SVDoubleValueObjectClass* mpExtentLeft;
	SVDoubleValueObjectClass* mpExtentTop;
	SVDoubleValueObjectClass* mpExtentWidth;
	SVDoubleValueObjectClass* mpExtentHeight;

	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdBasePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //SVTADLGCOLORTHRESHOLDBASEPAGE_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgColorThresholdBasePage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:19:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Aug 2006 15:48:20   ebeyeler
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
 *    Rev 1.0   18 Aug 2006 15:59:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/