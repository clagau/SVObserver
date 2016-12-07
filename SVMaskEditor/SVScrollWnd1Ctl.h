//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVScrollWnd1Ctl
//* .File Name       : $Workfile:   SVScrollWnd1Ctl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:09:24  $
//******************************************************************************

#pragma once

class SVScrollWnd1Ctrl : public COleControl
{
	DECLARE_DYNCREATE(SVScrollWnd1Ctrl)

// Constructor
public:
	SVScrollWnd1Ctrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVScrollWnd1Ctrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) override;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void DoPropExchange(CPropExchange* pPX) override;
	virtual void OnResetState() override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	~SVScrollWnd1Ctrl();

	DECLARE_OLECREATE_EX(SVScrollWnd1Ctrl)    // Class factory and guid
	DECLARE_OLETYPELIB(SVScrollWnd1Ctrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(SVScrollWnd1Ctrl)     // Property page IDs
	DECLARE_OLECTLTYPE(SVScrollWnd1Ctrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	//{{AFX_MSG(SVScrollWnd1Ctrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(SVScrollWnd1Ctrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(SVScrollWnd1Ctrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(SVScrollWnd1Ctrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

