// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVUnloadDeviceDialog.cpp
// * .File Name       : $Workfile:   SVUnloadDeviceDialog.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:38  $
// ******************************************************************************

#include "stdafx.h"
#include "svimagetest.h"
#include "SVUnloadDeviceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVUnloadDeviceDialog dialog


SVUnloadDeviceDialog::SVUnloadDeviceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SVUnloadDeviceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVUnloadDeviceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SVUnloadDeviceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVUnloadDeviceDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void SVUnloadDeviceDialog::Show(CWnd* pParent)
{
	CDialog::Create(IDD, pParent);
}

void SVUnloadDeviceDialog::Destroy()
{
	DestroyWindow();

	m_hWnd = NULL;
}

BEGIN_MESSAGE_MAP(SVUnloadDeviceDialog, CDialog)
	//{{AFX_MSG_MAP(SVUnloadDeviceDialog)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVUnloadDeviceDialog message handlers

void SVUnloadDeviceDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	CDialog::OnClose();
}

BOOL SVUnloadDeviceDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
//	return CDialog::OnCommand(wParam, lParam);
	return true;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVUnloadDeviceDialog.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:15:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Mar 2008 09:43:32   Joe
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