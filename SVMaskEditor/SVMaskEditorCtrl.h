//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorCtrl
//* .File Name       : $Workfile:   SVMaskEditorCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   03 Jun 2014 13:20:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMaskEditorDialog.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl : See SVMaskEditorCtrl.cpp for implementation.

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
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) override;
	virtual void DoPropExchange(CPropExchange* pPX) override;
	virtual void OnResetState() override;
	virtual DWORD GetControlFlags() override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SVMaskEditorCtrl();

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
	afx_msg IPictureDisp* GetImageDIBHandle();
	afx_msg void SetImageDIBHandle(IPictureDisp* pPic);
	afx_msg long GetMaskOperator();
	afx_msg void SetMaskOperator(long nNewValue);
	afx_msg IStream* GetMaskData();
	afx_msg void SetMaskData(IStream* nNewValue);
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
	SVMaskEditorDialog maskEditorDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
