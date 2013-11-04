//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDeviceApp
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceApp.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:46  $
//******************************************************************************

#if !defined(SVFILEACQUISITIONDEVICEAPP_H)
#define SVFILEACQUISITIONDEVICEAPP_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SVFileAcquisitionDevice.h"

/////////////////////////////////////////////////////////////////////////////
// SVFileAcquisitionDeviceApp
// See SVFileAcquisitionDeviceApp.cpp for the implementation of this class
//

class SVFileAcquisitionDeviceApp : public CWinApp
{
public:
	SVFileAcquisitionDeviceApp();
	SVFileAcquisitionDevice m_fileAcqDevice;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVFileAcquisitionDeviceApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVFileAcquisitionDeviceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern SVFileAcquisitionDeviceApp g_svTheApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVFileAcquisitionDevice\SVFileAcquisitionDeviceApp.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:28:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:58:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
