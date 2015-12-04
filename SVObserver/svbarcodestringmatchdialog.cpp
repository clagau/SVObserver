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
#include "SVBarCodeStringMatchDialog.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeStringMatchDialog dialog


SVBarCodeStringMatchDialog::SVBarCodeStringMatchDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(SVBarCodeStringMatchDialog::IDD)
{
	//{{AFX_DATA_INIT(SVBarCodeStringMatchDialog)
	m_szRegExpression = _T("");
	m_bSaveInFile = FALSE;
	m_szStringFileName = _T("");
	mcsMultiMatchFileName = _T("");
	mbUseMutiMatch = FALSE;
	mbUseSingleMatch = TRUE;
	//}}AFX_DATA_INIT
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
  mbUseSingleMatch = bValue;
  return TRUE;
}

BOOL SVBarCodeStringMatchDialog::GetUseSingle( BOOL &rbValue )
{
  rbValue = mbUseSingleMatch;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetRegExpression( LPCTSTR szRegExpression )
{
  m_szRegExpression = szRegExpression;
  return TRUE;
}

BOOL SVBarCodeStringMatchDialog::GetRegExpression( CString &rcsRegExpression )
{
  rcsRegExpression = m_szRegExpression;
  return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetSaveInFile( BOOL bValue )
{
  m_bSaveInFile = bValue;
  return TRUE;
}

BOOL SVBarCodeStringMatchDialog::GetSaveInFile( BOOL &rbValue )
{
  rbValue = m_bSaveInFile;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetSingleFileName( LPCTSTR szFileName )
{
   m_szStringFileName = szFileName;
   return TRUE;
}

BOOL SVBarCodeStringMatchDialog::GetSingleFileName(CString& rcsFileName)
{
   rcsFileName = m_szStringFileName;
	 return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetUseMultiple( BOOL bValue )
{
  mbUseMutiMatch = bValue;
  return TRUE;
}

BOOL SVBarCodeStringMatchDialog::GetUseMultiple( BOOL &rbValue )
{
  rbValue = mbUseMutiMatch;
	return TRUE;
}

BOOL SVBarCodeStringMatchDialog::SetMultiFileName( LPCTSTR szFileName )
{
   mcsMultiMatchFileName = szFileName;
   return TRUE;
}

BOOL SVBarCodeStringMatchDialog::GetMultiFileName(CString& rcsFileName)
{
   rcsFileName = mcsMultiMatchFileName;
	 return TRUE;
}

void SVBarCodeStringMatchDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBarCodeStringMatchDialog)
	DDX_Text(pDX, IDC_BARCODE_REGEXPRESSION, m_szRegExpression);
	DDX_Check(pDX, IDC_BARCODE_SAVEINFILE, m_bSaveInFile);
	DDX_Text(pDX, IDC_BARCODE_STRINGFILE, m_szStringFileName);
	DDX_Text(pDX, IDC_BC_MULTI_STRINGFILE, mcsMultiMatchFileName);
	DDX_Check(pDX, IDC_BC_USE_MULTIPLE, mbUseMutiMatch);
	DDX_Check(pDX, IDC_BC_USE_SINGLE, mbUseSingleMatch);
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
  EnableFileSelect (m_bSaveInFile);
}

void SVBarCodeStringMatchDialog::OnBarCodeSelectFile() 
{
   // begin SES 18-Jan-2001
   SVFileNameClass   svfnFileName;

   svfnFileName.SetFileType ( SV_BAR_CODE_STORE_VALUE_FILE_TYPE );

   if ( svfnFileName.SelectFile () == IDOK )
   {
      m_szStringFileName = svfnFileName.GetFullFileName ();

      UpdateData ( FALSE );
   }
   // end SES 18-Jan-2001
}

BOOL SVBarCodeStringMatchDialog::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	EnableDisableControls( mbUseSingleMatch );
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
		mcsMultiMatchFileName = svfnFileName.GetFullFileName ();

		UpdateData ( FALSE );
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\svbarcodestringmatchdialog.cpp_v  $
 * 
 *    Rev 1.1   18 Sep 2014 13:38:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SV_BAR_CODE_STORE_VALUE_FILE_TYPE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:43:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2003 16:54:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/