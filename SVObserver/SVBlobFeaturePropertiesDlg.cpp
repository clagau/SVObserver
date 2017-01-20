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

#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVBlobFeaturePropertiesDlg.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVBlobFeaturePropertiesDlg::SVBlobFeaturePropertiesDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVBlobFeaturePropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVBlobFeaturePropertiesDlg)
	m_dDefaultValue = 0.0;
	//}}AFX_DATA_INIT
}

SVBlobFeaturePropertiesDlg::~SVBlobFeaturePropertiesDlg()
{
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
	
	SVString Title = _T("Properties for ");
	Title += m_FeatureName;
	SetWindowText( Title.c_str() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

