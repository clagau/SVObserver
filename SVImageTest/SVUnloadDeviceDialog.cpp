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
#pragma region Includes
#include "stdafx.h"
#include "svimagetest.h"
#include "SVUnloadDeviceDialog.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVUnloadDeviceDialog dialog

SVUnloadDeviceDialog::SVUnloadDeviceDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(SVUnloadDeviceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVUnloadDeviceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVUnloadDeviceDialog::~SVUnloadDeviceDialog()
{
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

	m_hWnd = nullptr;
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
}

BOOL SVUnloadDeviceDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	return true;
}

