//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_OGD
//* .File Name       : $Workfile:   SV_OGD.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:17:02  $
//******************************************************************************

#if !defined(AFX_SV_OGD_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_)
#define AFX_SV_OGD_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVNumericEdit.h"


/////////////////////////////////////////////////////////////////////////////
// SVOCRGeometryDlg dialog

class SVOCRGeometryDlg : public CPropertyPage
{
// Construction
public:
	SVOCRGeometryDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVOCRGeometryDlg)
	enum { IDD = IDD_OCR_GEOM_DIALOG };
	CEdit	m_editMaxBlobPixels;
	CEdit	m_editMinBlobPixels;
	CEdit	m_sensitivityEdit;
	CComboBox	m_rotationModeCombo;
	CEdit	m_minWidthEdit;
	CSVNumericEdit	m_minScaleEdit;
	CEdit	m_minHeightEdit;
	CEdit	m_minAngleEdit;
	CEdit	m_maxWidthEdit;
	CEdit	m_maxScaleEdit;
	CEdit	m_maxHeightEdit;
	CEdit	m_maxAngleEdit;
	BOOL	m_scaleInvarientButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGeometryDlg)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
	public:
	BOOL	UpdateOCRParameters ();

//  Pointer to working OCRParameters structure of property sheet.
	CString				m_rotaionModeComboContents [3];

	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:

	// Generated message map functions
	//{{AFX_MSG(SVOCRGeometryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	float m_fMinScaleValue;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OGD_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV_OGD.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:17:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   25 Jun 2003 15:01:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  349
 * SCR Title:  Fix Bug I0001101 Enter text in minimum scale field
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the min scale edit to use SVNumericEdit which will allow only a float value
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 Apr 2003 16:14:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   22 Nov 1999 17:13:28   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 20 1999 07:21:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes for character x and y axis sort work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 31 1999 19:35:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   SVObserver 3.0 architecture changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/