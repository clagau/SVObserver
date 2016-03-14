//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgCharLabel
//* .File Name       : $Workfile:   SVDlgCharLabel.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:36:08  $
//******************************************************************************

#if !defined(AFX_SVDLGCHARLABEL_H__0A491F85_F1A7_11D2_A7B5_00106F000C7D__INCLUDED_)
#define AFX_SVDLGCHARLABEL_H__0A491F85_F1A7_11D2_A7B5_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// SVDlgCharLabel dialog

class SVDlgCharLabel : public CDialog
{
// Construction
public:
	SVDlgCharLabel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVDlgCharLabel)
	enum { IDD = IDD_CHAR_LABEL };
	CString	m_csCharLabel;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDlgCharLabel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVDlgCharLabel)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVDLGCHARLABEL_H__0A491F85_F1A7_11D2_A7B5_00106F000C7D__INCLUDED_)

