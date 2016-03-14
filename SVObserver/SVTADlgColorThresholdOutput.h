//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdOutput
//* .File Name       : $Workfile:   SVTADlgColorThresholdOutput.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:58  $
//******************************************************************************

#if !defined(SVTADLGCOLORTHRESHOLDOUTPUT_H)
#define SVTADLGCOLORTHRESHOLDOUTPUT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVTADlgColorThresholdOutput.h : header file
//

#include "SVTADlgColorThresholdBasePage.h"

class SVColorThresholdClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdOutput dialog

class SVTADlgColorThresholdOutput : public SVTADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdOutput)

// Construction
public:
	SVTADlgColorThresholdOutput();
	~SVTADlgColorThresholdOutput();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdOutput)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_OUTPUT_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdOutput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdOutput)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTADLGCOLORTHRESHOLDOUTPUT_H)

