//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVValueObject.h"
#pragma endregion Includes

class SVBarCodeDataMatrixDialog : public CPropertyPage
{
// Construction
public:
  int SetCellX (SVDoubleValueObjectClass &svdCellX);
  int SetCellY (SVDoubleValueObjectClass &svdCellY);
  int SetMinCellSize (SVDoubleValueObjectClass &svdMinCellSize);
  int SetMaxCellSize (SVDoubleValueObjectClass &svdMaxCellSize);
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
	CString	m_szCellX;
	int 	m_iCellX;
	BOOL	m_bCellX;
	int 	m_iCellY;
	BOOL	m_bCellY;
	CString	m_szCellY;
	int 	m_iMaxCellSize;
	BOOL	m_bMaxCellSize;
	CString	m_szMaxCellSize;
	int 	m_iMinCellSize;
	BOOL	m_bMinCellSize;
	CString	m_szMinCellSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBarCodeDataMatrixDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeDataMatrixDialog)
	virtual BOOL OnInitDialog();
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

