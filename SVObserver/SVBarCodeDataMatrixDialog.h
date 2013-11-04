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

#ifndef SVBARCODEDATAMATRIXDIALOG_H
#define SVBARCODEDATAMATRIXDIALOG_H

#include "SVValueObjectImpl.h"

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
	SVBarCodeDataMatrixDialog(CWnd* pParent = NULL);   // standard constructor

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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBARCODEDATAMATRIXDIALOG_H__AF5FDD6B_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBarCodeDataMatrixDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:41:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   04 Oct 2012 11:06:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   08 Aug 2005 15:26:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Value Object header info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 Apr 2003 16:54:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
