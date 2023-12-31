//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutDlg
//* .File Name       : $Workfile:   SVLutDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:19:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVMFCControls/SVDlgGraph.h"
#include "InspectionEngine/SVVirtualCamera.h"
#pragma endregion Includes

class SVLutDlg : public CPropertySheet
{
public:
	typedef std::map< uint32_t, SVLut > SVLutMap;

	explicit SVLutDlg(LPCTSTR Caption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~SVLutDlg();

	bool Create(SvIe::SVVirtualCameraPtrVector& rCameraVector, SVLutMap& raLut );

// Operator(s):
protected:
	void DestroyAllPages();

// Class Wizard Generated Message Map Entries
protected:
	//{{AFX_MSG(SVLutDlg)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Class Wizard Generated Virtual Function(s):
	virtual BOOL OnInitDialog() override;
	//{{AFX_VIRTUAL(SVLutDlg)
	//}}AFX_VIRTUAL
};

class SVLutDlgPage : public CPropertyPage
{
// Construction
public:
	SVLutDlgPage( SVLutDlg* Parent, LPCTSTR Caption);
	virtual ~SVLutDlgPage();

// Operator(s):
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVLutDlgPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
	
public:
	SVLut m_Lut;
	SvIe::SVVirtualCamera* m_pCamera;   // for band information
	SvCl::SVObjectLongArrayClass m_alBandData;

	//{{AFX_DATA(SVLutDlgPage)
	enum { IDD = IDD_LUT_DIALOG };
	CSliderCtrl	m_UpperSlider;
	CSliderCtrl	m_LowerSlider;
	SvMc::SVDlgGraph m_LUTGraph;
	SvMc::SVEnumerateCombo m_LutModeCombo;
	CString	m_UpperClipValue;
	CString	m_LowerClipValue;
	BOOL m_bUseLUT;
	BOOL m_bContinuousRecalcLUT;
	int	m_iBand;
	int	m_iX1;
	int	m_iX2;
	int	m_iY1;
	int	m_iY2;
	CString	m_RangeXMax;
	CString	m_RangeYMax;
	CString	m_RangeXYMax;
	//}}AFX_DATA

	static BOOL CALLBACK GraphMousePointCallback( POINT Point, LPVOID pThis_ );
	
protected:
	SVLutDlg* m_pParentDialog;
	SVLutTransformOperationEnum m_eLutMode;

	int m_iCurrentBand;	// for rgb color systems
    int m_iNumBands;
    int m_iBandWidth;
    int m_iCamBand;

	COLORREF m_rgbLineColor;

	CPoint m_ptLastMousePos;
	static const CPoint m_ptNoLastMousePos;
	static SVLutTransformOperationMap m_mapOperations;

	long m_lMaxLutValue;

protected:
	void Refresh();
	void SetLutParameters(const SVLutTransformParameters& param);
	void ShowControls(bool bShow, SVLutTransformOperationEnum e = LutTransformTypeUnknown );
	void EnableControls(bool bEnable);
	void ShowBoundaryControls(bool bShow);
	void ShowRangeControls(bool bShow);

	// Generated message map functions
	//{{AFX_MSG(SVLutDlgPage)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnActivateCheck();
	afx_msg void OnSelchangeLutModeCombo();
	afx_msg void OnContinuousRecalcCheck();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnColorBandAll();
	afx_msg void OnColorBandBlue();
	afx_msg void OnColorBandGreen();
	afx_msg void OnColorBandRed();
	afx_msg void OnColorBandSync();
	afx_msg void OnChangeX1();
	afx_msg void OnChangeX2();
	afx_msg void OnChangeY1();
	afx_msg void OnChangeY2();
	//}}AFX_MSG
	void OnChangeKnee();
	afx_msg LRESULT OnGraphRefresh(WPARAM mp1, LPARAM mp2);
	DECLARE_MESSAGE_MAP()
};

