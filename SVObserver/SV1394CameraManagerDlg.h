//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManagerDlg
//* .File Name       : $Workfile:   SV1394CameraManagerDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:14  $
//******************************************************************************

#ifndef SV1394CAMERAMANAGERDLG_H
#define SV1394CAMERAMANAGERDLG_H

#include "SV1394CameraManager.h"
#include "SVCameraListBox.h"

class SV1394CameraManagerDlg : public CDialog
{
public:
	SV1394CameraStructSet m_CamList;
	SV1394CameraStructSet m_OriginalCamList;
	SV1394CameraManagerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SV1394CameraManagerDlg)
	enum { IDD = IDD_1394_CAMERA_MANAGER_DLG };
	CButton	m_CUpButton;
	CButton	m_CDownButton;
	SVCameraListBox	m_lbCameras;
	CString	m_sSelectedCamea;
	//}}AFX_DATA

	CBitmap	m_CUpArrowBmp;
	CBitmap m_CDownArrowBmp;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SV1394CameraManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SV1394CameraManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	virtual void OnOK();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnCancel();
	//}}AFX_MSG
	HRESULT Refresh();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV1394CameraManagerDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Mar 2010 11:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to use a different functionality to manage the camera updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Nov 2003 09:37:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added capability to reorder cameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
