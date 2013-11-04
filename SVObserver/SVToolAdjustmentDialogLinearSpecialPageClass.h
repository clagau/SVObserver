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

#if !defined(AFX_SVTOOLADJUSTMENTDIALOGLINEARSPECIALPAGECLASS_H__357B0B09_DB58_4022_A197_7B121038A8DE__INCLUDED_)
#define AFX_SVTOOLADJUSTMENTDIALOGLINEARSPECIALPAGECLASS_H__357B0B09_DB58_4022_A197_7B121038A8DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVToolAdjustmentDialogLinearSpecialPageClass.h : header file
//

#include "SVEnumerateCombo.h"
#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;


/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogLinearSpecialPageClass dialog

class SVToolAdjustmentDialogLinearSpecialPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVToolAdjustmentDialogLinearSpecialPageClass)

// Construction
public:
	SVToolAdjustmentDialogLinearSpecialPageClass( SVToolAdjustmentDialogSheetClass* PParent = NULL);

	~SVToolAdjustmentDialogLinearSpecialPageClass();

	virtual HRESULT SetInspectionData();

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	SVToolAdjustmentDialogSheetClass*	m_pParentDialog;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogLinearSpecialPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboProfileOrientation();
	afx_msg void OnCheckRotation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTOOLADJUSTMENTDIALOGLINEARSPECIALPAGECLASS_H__357B0B09_DB58_4022_A197_7B121038A8DE__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolAdjustmentDialogLinearSpecialPageClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:36:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:45:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/