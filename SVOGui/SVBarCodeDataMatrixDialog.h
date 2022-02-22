//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeDataMatrixDialog
//* .File Name       : $Workfile:   SVBarCodeDataMatrixDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:41:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes


class SVBarCodeDataMatrixDialog : public CPropertyPage
{
// Construction
public:
  void SetCellX (long cellX);
  void SetCellY (long cellY);
  void SetMinCellSize (long minCellSize);
  void SetMaxCellSize (long maxCellSize);
  int GetCellX ();
  int GetCellY ();
  int GetMinCellSize ();
  int GetMaxCellSize ();
  void EnableControls (int iIdSlider, int iIdEdit, BOOL bEnable);
	SVBarCodeDataMatrixDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVBarCodeDataMatrixDialog();

// Dialog Data
	//{{AFX_DATA(SVBarCodeDataMatrixDialog)
	enum { IDD = IDD_BARCODE_DATAMATRIX };
	CString	m_szCellX{ _T("") };
	int 	m_iCellX = 0;
	BOOL	m_bCellX = false;
	int 	m_iCellY = 0;
	BOOL	m_bCellY = false;
	CString	m_szCellY{ _T("") };
	int 	m_iMaxCellSize = 0;
	BOOL	m_bMaxCellSize = false;
	CString	m_szMaxCellSize{ _T("") };
	int 	m_iMinCellSize = 0;
	BOOL	m_bMinCellSize = false;
	CString	m_szMinCellSize{ _T("") };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBarCodeDataMatrixDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeDataMatrixDialog)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeBarCodeMinCellSizeEdit();
	afx_msg void OnChangeBarCodeMaxCellSizeEdit();
	afx_msg void OnChangeBarCodeCellXEdit();
	afx_msg void OnChangeBarCodeCellYEdit();
	afx_msg void OnBarCodeMinCellSizeAuto();
	afx_msg void OnBarCodeMaxCellSizeAuto();
	afx_msg void OnBarCodeCellXAuto();
	afx_msg void OnBarCodeCellYAuto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

