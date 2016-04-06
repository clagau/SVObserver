//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnvironmentSettingsDialog
//* .File Name       : $Workfile:   SVEnvironmentSettingsDialog.CPP  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   31 Jan 2014 17:16:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEnvironmentSettingsDialog.h"
#pragma region Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVEnvironmentSettingsDialogClass 


SVEnvironmentSettingsDialogClass::SVEnvironmentSettingsDialogClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVEnvironmentSettingsDialogClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVEnvironmentSettingsDialogClass)
	StartLastAutomatically = FALSE;
	//}}AFX_DATA_INIT
}

SVEnvironmentSettingsDialogClass::~SVEnvironmentSettingsDialogClass()
{
}

void SVEnvironmentSettingsDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVEnvironmentSettingsDialogClass)
	DDX_Check(pDX, IDC_STARTLASTAUTOMATICALLY_CHECK, StartLastAutomatically);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVEnvironmentSettingsDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVEnvironmentSettingsDialogClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVEnvironmentSettingsDialogClass 

BOOL SVEnvironmentSettingsDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVEnvironmentSettingsDialogClass::OnOK() 
{
	UpdateData( TRUE );
	CDialog::OnOK();
}

