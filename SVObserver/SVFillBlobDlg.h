// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFillBlobDlg.h
// * .File Name       : $Workfile:   SVFillBlobDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 10:38:52  $
// ******************************************************************************

#if !defined(AFX_SVFILLBLOBDLG_H__96BD7895_131C_4186_8793_AFD7A234D0B0__INCLUDED_)
#define AFX_SVFILLBLOBDLG_H__96BD7895_131C_4186_8793_AFD7A234D0B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVFillBlobDlg.h : header file
//
#include "SVEnumerateCombo.h"
#include "SVValueObjectImpl.h"

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVFillBlobDlg dialog

class SVFillBlobDlg : public CDialog, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVFillBlobDlg(CWnd* pParent = NULL);   // standard constructor

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVFillBlobDlg)
	enum { IDD = IDD_DLG_FILL_BLOBS };
	SVEnumerateComboClass	m_ctlBlobType;
	SVEnumerateComboClass	m_ctlBlobColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVFillBlobDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(SVFillBlobDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboBlobColor();
	afx_msg void OnSelchangeComboBlobType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVEnumerateValueObjectClass *m_pvoBlobType;
	SVEnumerateValueObjectClass *m_pvoBlobColor;

private:
	void FillCombos();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVFILLBLOBDLG_H__96BD7895_131C_4186_8793_AFD7A234D0B0__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFillBlobDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:38:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:59:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Nov 2003 07:40:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the layout of the dialog and changed logic based on if fill blobs is turned on
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Oct 2003 14:05:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/