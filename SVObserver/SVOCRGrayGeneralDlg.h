//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayGeneralDlg
//* .File Name       : $Workfile:   SVOCRGrayGeneralDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:54:16  $
//******************************************************************************

#if !defined(AFX_SV_OCR_GRAY_MISC_DLG_H__6E7FD401_ECEA_11D2_846B_00403355EC6D__INCLUDED_)
#define AFX_SV_OCR_GRAY_MISC_DLG_H__6E7FD401_ECEA_11D2_846B_00403355EC6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayGeneralDlg dialog
class SVOCRGrayGeneralDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVOCRGrayGeneralDlg)

// Construction
public:
	SVOCRGrayGeneralDlg();
	~SVOCRGrayGeneralDlg();

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVOCRGrayGeneralDlg)
	enum { IDD = IDD_OCR_GRAY_GENERAL_DIALOG };
	int		m_yVicinity;
	int		m_xVicinity;
	int		m_maxMatches;
	float	m_minContrast;
	CString	m_fontFilename;
	BOOL	m_output;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGrayGeneralDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void	UpdateOCRParameters ();

//  samplesInt	0-5.
	int	m_samplesInt;
	
//  featuresInt 0-100.
	int m_featuresInt;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCRGrayGeneralDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontBrowseCmd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OCR_GRAY_MISC_DLG_H__6E7FD401_ECEA_11D2_846B_00403355EC6D__INCLUDED_)

