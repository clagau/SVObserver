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

#pragma once

#pragma region Includes

#pragma endregion Includes

class SVBlobFeaturePropertiesDlg : public CDialog
{
// Construction
public:
	SVBlobFeaturePropertiesDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVBlobFeaturePropertiesDlg();

	std::string m_FeatureName;

// Dialog Data
	//{{AFX_DATA(SVBlobFeaturePropertiesDlg)
	enum { IDD = IDD_BLOB_FEATURE_PROPERTIES };
	double	m_dDefaultValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBlobFeaturePropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBlobFeaturePropertiesDlg)
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
