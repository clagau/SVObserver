//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeStringMatchDialog
//* .File Name       : $Workfile:   svbarcodestringmatchdialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Sep 2014 13:38:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource/Resource.h"
#include "SVBarCodeStringMatchDialog.h"
#include "SVFileNameClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeStringMatchDialog dialog


SVBarCodeStringMatchDialog::SVBarCodeStringMatchDialog(CWnd* pParent /*=nullptr*/)
	: CPropertyPage(SVBarCodeStringMatchDialog::IDD)
{
	m_SaveInFile = FALSE;
	m_UseMultiMatch = FALSE;
	m_UseSingleMatch = TRUE;
}

SVBarCodeStringMatchDialog::~SVBarCodeStringMatchDialog()
{
}

BOOL SVBarCodeStringMatchDialog::EnableFileSelect(BOOL bEnable)
{
	if ( IsDlgButtonChecked( IDC_BC_USE_SINGLE ) != 0 )
	{
		CButton *pFileSelect;

		pFileSelect = (CButton *) GetDlgItem (IDC_BARCODE_SELECTFILE);
		pFileSelect->EnableWindow (bEnable);

		return bEnable;
	}

	return FALSE;
}

void SVBarCodeStringMatchDialog::EnableDisableControls(BOOL bSingle)
{
	CButton *pUseSingle = (CButton *) GetDlgItem (IDC_BC_USE_SINGLE);
	CButton *pUseMulti = (CButton *) GetDlgItem (IDC_BC_USE_MULTIPLE);
	CEdit *pMatchString = (CEdit *) GetDlgItem (IDC_BARCODE_REGEXPRESSION);
	CButton *pSaveInFile = (CButton *) GetDlgItem (IDC_BARCODE_SAVEINFILE);
	CButton *pSingleSelect = (CButton *) GetDlgItem (IDC_BARCODE_SELECTFILE);
	CButton *pMultiSelect = (CButton *) GetDlgItem (IDC_BC_MULTI_SELECTFILE);

	if ( bSingle )
	{
		pUseSingle->SetCheck( 1 );
		pUseMulti->SetCheck( 0 );

		pSaveInFile->EnableWindow ( TRUE );
		pMatchString->EnableWindow( TRUE );
		EnableFileSelect( pSaveInFile->GetCheck() == 1 );

		pMultiSelect->EnableWindow( FALSE );
	}
	else
	{
		pUseSingle->SetCheck( 0 );
		pUseMulti->SetCheck( 1 );

		pSaveInFile->EnableWindow ( FALSE );
		pMatchString->EnableWindow( FALSE );
		pSingleSelect->EnableWindow( FALSE );

		pMultiSelect->EnableWindow( TRUE );
	}
}

BOOL SVBarCodeStringMatchDialog::SetUseSingle( BOOL bValue )
{
	m_UseSingleMatch = bValue;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetRegExpression( LPCTSTR szRegExpression )
{
	m_RegExpression = szRegExpression;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetSaveInFile( BOOL bValue )
{
	m_SaveInFile = bValue;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetSingleFileName( LPCTSTR szFileName )
{
	m_StringFileName = szFileName;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetUseMultiple( BOOL bValue )
{
	m_UseMultiMatch = bValue;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetMultiFileName( LPCTSTR szFileName )
{
	m_MultiMatchFileName = szFileName;
	return TRUE;
}

void SVBarCodeStringMatchDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBarCodeStringMatchDialog)
	DDX_Text(pDX, IDC_BARCODE_REGEXPRESSION, m_RegExpression);
	DDX_Check(pDX, IDC_BARCODE_SAVEINFILE, m_SaveInFile);
	DDX_Text(pDX, IDC_BARCODE_STRINGFILE, m_StringFileName);
	DDX_Text(pDX, IDC_BC_MULTI_STRINGFILE, m_MultiMatchFileName);
	DDX_Check(pDX, IDC_BC_USE_MULTIPLE, m_UseMultiMatch);
	DDX_Check(pDX, IDC_BC_USE_SINGLE, m_UseSingleMatch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVBarCodeStringMatchDialog, CPropertyPage)
	//{{AFX_MSG_MAP(SVBarCodeStringMatchDialog)
	ON_BN_CLICKED(IDC_BARCODE_SAVEINFILE, OnBarcodeSaveInFile)
	ON_BN_CLICKED(IDC_BARCODE_SELECTFILE, OnBarCodeSelectFile)
	ON_BN_CLICKED(IDC_BC_USE_SINGLE, OnBcUseSingle)
	ON_BN_CLICKED(IDC_BC_USE_MULTIPLE, OnBcUseMultiple)
	ON_BN_CLICKED(IDC_BC_MULTI_SELECTFILE, OnBcMultiSelectfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeStringMatchDialog message handlers

void SVBarCodeStringMatchDialog::OnBarcodeSaveInFile() 
{
	UpdateData (TRUE);
	EnableFileSelect (m_SaveInFile);
}

void SVBarCodeStringMatchDialog::OnBarCodeSelectFile() 
{
	// begin SES 18-Jan-2001
	SVFileNameClass   svfnFileName;

	svfnFileName.SetFileType ( SV_BAR_CODE_STORE_VALUE_FILE_TYPE );

	if ( svfnFileName.SelectFile () == IDOK )
	{
		m_StringFileName = svfnFileName.GetFullFileName().c_str();

		UpdateData ( FALSE );
	}
	// end SES 18-Jan-2001
}

BOOL SVBarCodeStringMatchDialog::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	EnableDisableControls( m_UseSingleMatch );
	return TRUE;
}

void SVBarCodeStringMatchDialog::OnBcUseSingle() 
{
	EnableDisableControls(TRUE);

	UpdateData (TRUE);
}

void SVBarCodeStringMatchDialog::OnBcUseMultiple() 
{
	EnableDisableControls(FALSE);

	UpdateData (TRUE);
}

void SVBarCodeStringMatchDialog::OnBcMultiSelectfile() 
{
	SVFileNameClass   svfnFileName;

	svfnFileName.SetFileType ( SV_BAR_CODE_MATCH_STRING_FILE_TYPE );

	if ( svfnFileName.SelectFile () == IDOK )
	{
		m_MultiMatchFileName = svfnFileName.GetFullFileName().c_str();

		UpdateData ( FALSE );
	}
}
