// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFillBlobDlg.h
// * .File Name       : $Workfile:   SVFillBlobDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   12 Nov 2014 07:05:36  $
// ******************************************************************************

#if !defined(AFX_SVFILLBLOBDLG_H__96BD7895_131C_4186_8793_AFD7A234D0B0__INCLUDED_)
#define AFX_SVFILLBLOBDLG_H__96BD7895_131C_4186_8793_AFD7A234D0B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVFillBlobDlg.h : header file
//
#include "SVEnumerateCombo.h"
#include "SVValueObjectImpl.h"

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVFillBlobDlg dialog

class SVFillBlobDlg : public CDialog, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVFillBlobDlg(CWnd* pParent = NULL);   // standard constructor

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVFillBlobDlg)
	enum { IDD = IDD_DLG_FILL_BLOBS };
	SVEnumerateComboClass	m_ctlBlobType;
	SVEnumerateComboClass	m_ctlBlobColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVFillBlobDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(SVFillBlobDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboBlobColor();
	afx_msg void OnSelchangeComboBlobType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVEnumerateValueObjectClass *m_pvoBlobType;
	SVEnumerateValueObjectClass *m_pvoBlobFillColor;

private:
	void FillCombos();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVFILLBLOBDLG_H__96BD7895_131C_4186_8793_AFD7A234D0B0__INCLUDED_)

