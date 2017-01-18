//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLuminanceSetup
//* .File Name       : $Workfile:   SVAnalyzerLuminanceSetup.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:28  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVLibrary/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVLuminanceAnalyzerClass;

/////////////////////////////////////////////////////////////////////////////
// SVLuminanceAnalyzerSetupClass dialog

class SVLuminanceAnalyzerSetupClass : public CDialog, public SVTaskObjectValueInterface
{
public:
	SVLuminanceAnalyzerClass* m_pAnalyzer;

	HRESULT SetInspectionData();

protected:
	// Construction
public:
	SVLuminanceAnalyzerSetupClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVLuminanceAnalyzerSetupClass();

// Dialog Data
	//{{AFX_DATA(SVLuminanceAnalyzerSetupClass)
	enum { IDD = IDD_LUMINANCE_ANALYZER_SETUP };
	BOOL	m_calcStdDev;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVLuminanceAnalyzerSetupClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVLuminanceAnalyzerSetupClass)
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	virtual void OnCancel() override;
	afx_msg void OnRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
