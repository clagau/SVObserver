//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMessageWindow
//* .File Name       : $Workfile:   SVMessageWindow.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:26  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMessageWindow.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVMessageWindow 


SVMessageWindow::SVMessageWindow(CWnd* pParent /*=nullptr*/)
	: CDialog(SVMessageWindow::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVMessageWindow)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

SVMessageWindow::~SVMessageWindow()
{
}

void SVMessageWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVMessageWindow)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVMessageWindow, CDialog)
	//{{AFX_MSG_MAP(SVMessageWindow)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

