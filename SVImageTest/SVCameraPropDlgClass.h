// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPropDlgClass.h
// * .File Name       : $Workfile:   SVCameraPropDlgClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:08:28  $
// ******************************************************************************

#ifndef SVCAMERAPROPDLGCLASS_H
#define SVCAMERAPROPDLGCLASS_H

#include "SVMFCControls\SVListCtrl.h"

class SVDigitizerLoadLibraryClass;

class SVCameraPropDlgClass : public CDialog
{
public:
	SVCameraPropDlgClass(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(SVCameraPropDlgClass)
	enum { IDD = IDD_CAMERA_PROP };
	SvMc::SVListCtrl	m_svListCtrl;
	//}}AFX_DATA

	SVDigitizerLoadLibraryClass *m_psvDigitizers;
	unsigned long m_ulHandle;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCameraPropDlgClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVCameraPropDlgClass)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVCameraPropDlgClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:08:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 11:42:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Mar 2008 09:39:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Nov 2006 12:23:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Revision header block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
