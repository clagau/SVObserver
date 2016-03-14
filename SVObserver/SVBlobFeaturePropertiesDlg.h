//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobFeaturePropertiesDlg
//* .File Name       : $Workfile:   SVBlobFeaturePropertiesDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:16  $
//******************************************************************************

#if !defined(AFX_SVBLOBFEATUREPROPERTIESDLG_H__84685960_AB6C_4332_B5BD_0E6A1587F0AB__INCLUDED_)
#define AFX_SVBLOBFEATUREPROPERTIESDLG_H__84685960_AB6C_4332_B5BD_0E6A1587F0AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVBlobFeaturePropertiesDlg : public CDialog
{
// Construction
public:
	SVBlobFeaturePropertiesDlg(CWnd* pParent = NULL);   // standard constructor

	CString m_strFeatureName;

// Dialog Data
	//{{AFX_DATA(SVBlobFeaturePropertiesDlg)
	enum { IDD = IDD_BLOB_FEATURE_PROPERTIES };
	double	m_dDefaultValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBlobFeaturePropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBlobFeaturePropertiesDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBLOBFEATUREPROPERTIESDLG_H__84685960_AB6C_4332_B5BD_0E6A1587F0AB__INCLUDED_)

