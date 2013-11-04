// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVUnloadDeviceDialog.h
// * .File Name       : $Workfile:   SVUnloadDeviceDialog.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:14  $
// ******************************************************************************

#if !defined(AFX_SVUNLOADDEVICEDIALOG_H__24BB1C13_E87E_41A2_991D_E8BCDFCAC3E1__INCLUDED_)
#define AFX_SVUNLOADDEVICEDIALOG_H__24BB1C13_E87E_41A2_991D_E8BCDFCAC3E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVUnloadDeviceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SVUnloadDeviceDialog dialog

class SVUnloadDeviceDialog : public CDialog
{
// Construction
public:
	SVUnloadDeviceDialog(CWnd* pParent = NULL);   // standard constructor
	void Show(CWnd* pParent);
	void Destroy();

// Dialog Data
	//{{AFX_DATA(SVUnloadDeviceDialog)
	enum { IDD = IDD_UNLOADDEVICE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVUnloadDeviceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVUnloadDeviceDialog)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVUNLOADDEVICEDIALOG_H__24BB1C13_E87E_41A2_991D_E8BCDFCAC3E1__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVUnloadDeviceDialog.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:15:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Mar 2008 09:43:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Nov 2006 11:07:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Revision header block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/