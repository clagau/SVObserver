//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogLinearSpecialPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogLinearSpecialPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:36:50  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEnumerateCombo.h"
#include "SVLibrary/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;

class SVToolAdjustmentDialogLinearSpecialPageClass : public CPropertyPage, public SVTaskObjectValueInterface
{
public:
	SVToolAdjustmentDialogLinearSpecialPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* PParent = nullptr);
	virtual ~SVToolAdjustmentDialogLinearSpecialPageClass();

	HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVToolAdjustmentDialogLinearSpecialPageClass)
	enum { IDD = IDD_TA_LINEAR_TOOL_PROPERTIES_DIALOG };
	CButton	m_ctlUseRotation;
	SVEnumerateComboClass	m_ctlProfileOrientation;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogLinearSpecialPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	SVToolAdjustmentDialogSheetClass*	m_pParentDialog;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogLinearSpecialPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeComboProfileOrientation();
	afx_msg void OnCheckRotation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

