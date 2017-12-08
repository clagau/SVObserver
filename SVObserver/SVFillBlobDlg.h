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

#pragma once

#pragma region Includes
#include "SVEnumerateCombo.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "InspectionEngine/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVToolClass;

class SVFillBlobDlg : public CDialog, public SVTaskObjectValueInterface
{
public:
	SVFillBlobDlg(SVToolClass* pTool, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVFillBlobDlg();
	HRESULT SetInspectionData();

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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVFillBlobDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeComboBlobColor();
	afx_msg void OnSelchangeComboBlobType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVToolClass* m_pTool;
	SVEnumerateValueObjectClass *m_pvoBlobType;
	SVEnumerateValueObjectClass *m_pvoBlobFillColor;

private:
	void FillCombos();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
