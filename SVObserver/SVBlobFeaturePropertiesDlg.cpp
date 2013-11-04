//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobFeaturePropertiesDlg
//* .File Name       : $Workfile:   SVBlobFeaturePropertiesDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:08  $
//******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVBlobFeaturePropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBlobFeaturePropertiesDlg dialog


SVBlobFeaturePropertiesDlg::SVBlobFeaturePropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVBlobFeaturePropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVBlobFeaturePropertiesDlg)
	m_dDefaultValue = 0.0;
	//}}AFX_DATA_INIT
}


void SVBlobFeaturePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBlobFeaturePropertiesDlg)
	DDX_Text(pDX, IDC_DEFAULT_VALUE, m_dDefaultValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVBlobFeaturePropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(SVBlobFeaturePropertiesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVBlobFeaturePropertiesDlg message handlers

void SVBlobFeaturePropertiesDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL SVBlobFeaturePropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strTitle = _T("Properties for ");
	strTitle += m_strFeatureName;
	SetWindowText(strTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBlobFeaturePropertiesDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:45:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jul 2005 13:10:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/