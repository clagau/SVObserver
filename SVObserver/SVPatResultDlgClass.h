//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatResultDlgClass
//* .File Name       : $Workfile:   SVPatResultDlgClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:48  $
//******************************************************************************

#if !defined(AFX_SVPATRESULTDLGCLASS_H__33CECE3C_FF27_11D3_A8A2_00106F021742__INCLUDED_)
#define AFX_SVPATRESULTDLGCLASS_H__33CECE3C_FF27_11D3_A8A2_00106F021742__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVPatResultDlgClass.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SVPatResultDlgClass dialog

class SVPatResultDlgClass : public CDialog
{
// Construction
public:
	SVPatResultDlgClass(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVPatResultDlgClass)
	enum { IDD = IDD_PAT_RESULT_DIALOG };
	CString	m_strAngle;
	CString	m_strScore;
	CString	m_strXPos;
	CString	m_strYPos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPatResultDlgClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVPatResultDlgClass)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPATRESULTDLGCLASS_H__33CECE3C_FF27_11D3_A8A2_00106F021742__INCLUDED_)

