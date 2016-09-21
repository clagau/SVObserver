//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveWarpDlg
//* .File Name       : $Workfile:   SVPerspectiveWarpDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEnumerateCombo.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;

class SVPerspectiveWarpDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	long m_lLastWarpType;
	SVPerspectiveWarpDlg( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );
	virtual ~SVPerspectiveWarpDlg();

	virtual HRESULT SetInspectionData() override;

// Dialog Data
	//{{AFX_DATA(SVPerspectiveWarpDlg)
	enum { IDD = IDD_TA_PERSPECTIVE_TOOL_DIALOG };
	SVEnumerateComboClass	m_FunctionCombo;
	SVEnumerateComboClass	m_cbInterpolation;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPerspectiveWarpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SVToolClass* m_pTool;
	SVEnumerateValueObjectClass* m_pWarpType;
	SVEnumerateValueObjectClass* m_pInterpolationMode;

	SVToolAdjustmentDialogSheetClass*	pParentDialog;
	// Generated message map functions
	//{{AFX_MSG(SVPerspectiveWarpDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePerspectiveUnwarpTypeCombo();
	afx_msg void OnSelchangeInterpolationModeCombo(); 
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

