//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpDlg
//* .File Name       : $Workfile:   SVCylindricalWarpDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:05:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEnumerateCombo.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVLibrary/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;

class SVCylindricalWarpDlg : public CPropertyPage, public SVTaskObjectValueInterface
{
public:
	long m_lLastWarpType;

	SVCylindricalWarpDlg( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );
	virtual ~SVCylindricalWarpDlg();
	HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVCylindricalWarpDlg)
	enum { IDD = IDD_TA_CYLINDRICAL_TOOL_DIALOG };
	SVEnumerateComboClass	m_cbInterpolation;
	SVEnumerateComboClass	m_cbOrientation;
	double	m_dWarpAngle;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCylindricalWarpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	SVToolClass* m_pTool;
	SVEnumerateValueObjectClass* m_pWarpType;
	SVEnumerateValueObjectClass* m_pInterpolationMode;
	SVDoubleValueObjectClass* m_pWarpAngle;

	SVToolAdjustmentDialogSheetClass*	m_pParentDialog;
	// Generated message map functions
	//{{AFX_MSG(SVCylindricalWarpDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeOrientationCombo();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeInterpolationModeCombo();
	afx_msg void OnKillfocusWarpAngleEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

