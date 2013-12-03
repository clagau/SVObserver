//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorCtl
//* .File Name       : $Workfile:   SVMaskEditorCtl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:22:34  $
//******************************************************************************

#if !defined(AFX_SVMASKEDITORCTL_H__C87C9B82_E6DC_11D2_A77B_00106F000C7D__INCLUDED_)
#define AFX_SVMASKEDITORCTL_H__C87C9B82_E6DC_11D2_A77B_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SVMaskEditorDialog.h"


// SVMaskEditorCtl.h : Declaration of the SVMaskEditorCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl : See SVMaskEditorCtl.cpp for implementation.

class SVMaskEditorCtrl : public COleControl
{
	DECLARE_DYNCREATE(SVMaskEditorCtrl)

// Constructor
public:
	SVMaskEditorCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVMaskEditorCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	virtual void OnClick(USHORT iButton);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~SVMaskEditorCtrl();

	DECLARE_OLECREATE_EX(SVMaskEditorCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(SVMaskEditorCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(SVMaskEditorCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(SVMaskEditorCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(SVMaskEditorCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(SVMaskEditorCtrl)
	afx_msg OLE_HANDLE GetImageDIBHandle();
	afx_msg void SetImageDIBHandle(OLE_HANDLE nNewValue);
	afx_msg long GetMaskOperator();
	afx_msg void SetMaskOperator(long nNewValue);
	afx_msg OLE_HANDLE GetMaskData();
	afx_msg void SetMaskData(OLE_HANDLE nNewValue);
	afx_msg INT_PTR DoEditMaskModal();
	afx_msg void Refresh();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(SVMaskEditorCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(SVMaskEditorCtrl)
	dispidImageDIBHandle = 1L,
	dispidMaskOperator = 2L,
	dispidMaskData = 3L,
	dispidDoEditMaskModal = 4L,
	//}}AFX_DISP_ID
	};

protected:

	CBitmap		bitmap;
	SVMaskEditorDialogClass maskEditorDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMASKEDITORCTL_H__C87C9B82_E6DC_11D2_A77B_00106F000C7D__INCLUDED)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaskEditor\SVMaskEditorCtl.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 10:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2013 14:07:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
