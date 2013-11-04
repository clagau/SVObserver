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

#if !defined(AFX_SVPERSPECTIVEWARPDLG_H__49C41C4E_B381_440B_AAC7_EA6BEDE2A517__INCLUDED_)
#define AFX_SVPERSPECTIVEWARPDLG_H__49C41C4E_B381_440B_AAC7_EA6BEDE2A517__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVPerspectiveWarpDlg.h : header file
//
#include "SVEnumerateCombo.h"
#include "SVValueObjectImpl.h"	// Added by ClassView

#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;

/////////////////////////////////////////////////////////////////////////////
// SVPerspectiveWarpDlg dialog

class SVPerspectiveWarpDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	long m_lLastWarpType;
	SVPerspectiveWarpDlg( SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );

	virtual HRESULT SetInspectionData();

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

#endif // !defined(AFX_SVPERSPECTIVEWARPDLG_H__49C41C4E_B381_440B_AAC7_EA6BEDE2A517__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPerspectiveWarpDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:22:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 07:59:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2005 15:47:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Interpolation mode to PerspectiveWarpTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Mar 2005 13:46:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in new files for the new perspective tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/