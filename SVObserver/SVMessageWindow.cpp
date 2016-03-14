//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMessageWindowClass
//* .File Name       : $Workfile:   SVMessageWindow.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVMessageWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVMessageWindowClass 


SVMessageWindowClass::SVMessageWindowClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVMessageWindowClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVMessageWindowClass)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void SVMessageWindowClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVMessageWindowClass)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVMessageWindowClass, CDialog)
	//{{AFX_MSG_MAP(SVMessageWindowClass)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

