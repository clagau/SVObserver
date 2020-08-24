// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMaskEditorDialog
// * .File Name       : $Workfile:   SVMaskEditorDialog.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:07:50  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#include "SVMaskEditorWnd.h"
#include "SVLibrary\SVGraphix.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#pragma endregion Includes

class SVGraphixDrawObjectClass;

class SVMaskEditorDialog : public CDialog
{
// Construction
public:
	SVMaskEditorDialog( CWnd* pParent = nullptr );   // standard constructor

    void PanX( int OffsetX );
    void PanY( int OffsetY );
	void ScalePoint( CPoint& RPoint, BOOL BDown );
	
	void CanvasLButtonDown( UINT nFlags, CPoint point );
	void CanvasLButtonUp( UINT nFlags, CPoint point );
	void CanvasLButtonDblClk( UINT nFlags, CPoint point );
	void CanvasMouseMove( UINT nFlags, CPoint point );

// Dialog Data
	//{{AFX_DATA(SVMaskEditorDialog)
	enum { IDD = IDD_EDITOR_DIALOG };
	CComboBox	ArithmeticComboCtl;
	CString	StrZoom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVMaskEditorDialog)
	public:
	virtual void OnFinalRelease() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVMaskEditorDialog)
	virtual BOOL OnInitDialog() override;
	virtual void OnCancel() override;
	virtual void OnOK() override;
	afx_msg void OnUndoButton();
	afx_msg void OnRedoButton();
	afx_msg void OnZoomInButton();
	afx_msg void OnZoomOutButton();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPenRadio();
	afx_msg void OnPolygonRadio();
	afx_msg void OnRectRadio();
	afx_msg void OnEllipseRadio();
	afx_msg void OnFilledEllipseRadio();
	afx_msg void OnFilledRectRadio();
	afx_msg void OnPolylineRadio();
	afx_msg void OnFillRadio();
	afx_msg void OnSelchangeArithmeticCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(SVMaskEditorDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

    void freeMilResources();
    void prepareMaskBuffer();
    void evaluateMask();
    void zoom( int ZoomOperand );
	SVGraphixDrawObjectClass* getDrawObject();
	void enableButtons();

public:

    HBITMAP         HImageBitmap;
    SVGraphixClass  GraphixObject;
    long          m_lMilArithmeticOperation;
	SVMaskEditorWnd	DisplayWndCtl;

protected:

	__int64     milDisplay = 0LL;
	SVMatroxBuffer      milDisplayBuffer;
    SVMatroxBuffer      milMaskBuffer;
    SVMatroxBuffer      milImageBuffer;

    long currentZoomX;
    long currentZoomY;
    long currentPanX;
    long currentPanY;

	SVGraphixDrawObjectEnum		currentDrawObjectSelection;
	BOOL						bNewDrawObject;
	BOOL						bDrawActive;
	SVGraphixDrawObjectClass*	pCurrentDrawObject;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


