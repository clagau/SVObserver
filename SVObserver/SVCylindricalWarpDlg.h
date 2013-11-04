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

#ifndef SVCYLINDRICALWARPDLG_H_
#define SVCYLINDRICALWARPDLG_H_
#pragma once

#include "SVEnumerateCombo.h"
#include "SVValueObjectImpl.h"

#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;

/////////////////////////////////////////////////////////////////////////////
// SVCylindricalWarpDlg dialog

class SVCylindricalWarpDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	long m_lLastWarpType;
	SVCylindricalWarpDlg( SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );

	virtual HRESULT SetInspectionData();

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeOrientationCombo();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeInterpolationModeCombo();
	afx_msg void OnKillfocusWarpAngleEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #ifndef SVCYLINDRICALWARPDLG_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCylindricalWarpDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:05:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:37:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jul 2005 08:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Cylindrical Dialog Changes
 * Added Warp Angle and Interpolation Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Mar 2005 12:17:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/