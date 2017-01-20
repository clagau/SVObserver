//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAboutDialogClass
//* .File Name       : $Workfile:   SVAboutDialogClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVAboutDialogClass.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes


//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVAboutDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVAboutDialogClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ***** Public Operators *****

SVAboutDialogClass::SVAboutDialogClass() : CDialog(SVAboutDialogClass::IDD)
{
	logoBmp = nullptr;

	//{{AFX_DATA_INIT(SVAboutDialogClass)
	//}}AFX_DATA_INIT
}

SVAboutDialogClass::~SVAboutDialogClass()
{
	if( logoBmp )
		::DeleteObject( logoBmp );
}

void SVAboutDialogClass::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAboutDialogClass)
	DDX_Control(pDX, IDC_VERSION_LIST, m_LCVersion);
	//}}AFX_DATA_MAP
}

BOOL SVAboutDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set current version in about box....
	CWnd* pWnd = GetDlgItem( IDC_CURRENT_VERSION_STATIC );
	if( pWnd )
	{
		CString appTitle;
		appTitle.LoadString(AFX_IDS_APP_TITLE);

		CString Version = appTitle;
		
		Version += _T( " " );
		Version += SvSyl::SVVersionInfo::GetVersion().c_str();

		pWnd->SetWindowText( Version );
	}
	
	LoadOEMInfo();
	LoadVersionList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////
// Load OEM Logo and OEM Text
////////////////////////////////////
void SVAboutDialogClass::LoadOEMInfo()
{
	// Load Bitmap for OEM Logo
	logoBmp = nullptr;
	

	SVString logoFileName = SvStl::GlobalPath::Inst().GetBinPath(_T("OEM\\logo.bmp")); 
	if( 0 == ::_access( logoFileName.c_str(), 0 ) )
	{
		logoBmp = (HBITMAP) ::LoadImage( nullptr, 
								logoFileName.c_str(),
								IMAGE_BITMAP,
								0,
								0,
								LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE );
	}// end if

	SVString TextFileName = SvStl::GlobalPath::Inst().GetBinPath(_T("OEM\\oem.txt")); 
	
	FILE* oemFile;
	oemFile = _tfopen( TextFileName.c_str(),"r" );

	if( logoBmp && oemFile )
	{
		// Set The bitmap
		CStatic* pStaticWnd = (CStatic *)GetDlgItem( IDC_LOGO );
		if( pStaticWnd )
			pStaticWnd->SetBitmap( logoBmp );

		// Hide SVResearch info
		HideSVRText();
		
		// Load OEM text
		TCHAR tmp[128];

		// Read the Company name
		while( _fgetts( tmp, sizeof( tmp ), oemFile ) )
		{
			SetOEMText( tmp );
		}

	}// end if

	// Close the file if it was open
	if( oemFile )
		fclose( oemFile );
}

void SVAboutDialogClass::HideSVRText()
{
	CWnd* pWnd;

	// Hide Company
	pWnd = GetDlgItem( IDC_COMPANY );
	if( pWnd )
		pWnd->ShowWindow( SW_HIDE );

	// Hide Country
	pWnd = GetDlgItem( IDC_COUNTRY );
	if( pWnd )
		pWnd->ShowWindow( SW_HIDE );

	// Hide Address 2
	pWnd = GetDlgItem( IDC_ADDRESS );
	if( pWnd )
		pWnd->ShowWindow( SW_HIDE );

	// Hide Country 2
	pWnd = GetDlgItem( IDC_COUNTRY2 );
	if( pWnd )
		pWnd->ShowWindow( SW_HIDE );

	// Hide Phone 2
	pWnd = GetDlgItem( IDC_PHONE );
	if( pWnd )
		pWnd->ShowWindow( SW_HIDE );

	// Hide Internet 2
	pWnd = GetDlgItem( IDC_EMAIL );
	if( pWnd )
		pWnd->ShowWindow( SW_HIDE );

}

void SVAboutDialogClass::SetOEMText( LPCTSTR text )
{
	CWnd* pWnd;
	CString oemText;
	int pos;
	CString keyWords[5] = { _T( "Company=" ),
							_T( "Address=" ),
							_T( "Country=" ),
							_T( "Phone=" ),
							_T( "Email=" ) };

	oemText = text;

	if( ( pos = oemText.Find( keyWords[0] ) ) != -1 )
	{
		pWnd = GetDlgItem( IDC_COMPANY );
		if( pWnd )
		{
			pWnd->SetWindowText( oemText.Mid( pos + keyWords[0].GetLength() ) );
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
	}

	else if( (pos = oemText.Find( keyWords[1] ) ) != -1)
	{	
		pWnd = GetDlgItem( IDC_ADDRESS );
		if( pWnd )
		{
			pWnd->SetWindowText( oemText.Mid( pos + keyWords[1].GetLength() ) );
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
	}

	else if( ( pos = oemText.Find( keyWords[2] ) ) != -1 )
	{	
		pWnd = GetDlgItem( IDC_COUNTRY2 );
		if( pWnd )
		{
			pWnd->SetWindowText( oemText.Mid( pos + keyWords[2].GetLength() ) );
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
	}

	else if( ( pos = oemText.Find( keyWords[3] ) ) != -1 )
	{	
		pWnd = GetDlgItem( IDC_PHONE );
		if( pWnd )
		{
			pWnd->SetWindowText( oemText.Mid( pos + keyWords[3].GetLength() ) );
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
	}
	
	else if( ( pos = oemText.Find( keyWords[4] ) ) != -1 )
	{	
		pWnd = GetDlgItem( IDC_EMAIL );
		if( pWnd )
		{
			pWnd->SetWindowText( oemText.Mid( pos + keyWords[4].GetLength() ) );
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
	}
}

void SVAboutDialogClass::LoadVersionList()
{
	BSTR l_bstName = nullptr;

	// IO Board Version.....

	// SVBoardVersion enum is used here to make the code more clear.
	// however at some time in the future the Dll parameters may be implemented
	// as an array and therefore this enum may not apply.
	HRESULT l_hr = SVIOConfigurationInterfaceClass::Instance().GetParameterName(SVBoardVersion, &l_bstName );

	if( S_OK == l_hr )
	{
		CString l_strName( l_bstName, ::SysStringLen( l_bstName ) );
		CString l_strValue;
		CString l_strTmp;
		_variant_t l_vVersion;

		::SysFreeString( l_bstName );

		l_hr = SVIOConfigurationInterfaceClass::Instance().GetParameterValue( SVBoardVersion, &l_vVersion );

		// Display Information about the Board.
		l_strValue = l_vVersion.bstrVal;
		::VariantClear( &l_vVersion );

		l_strTmp.Format("I/O %s - %s",
			l_strName, 
			l_strValue );

		CDC* dc = GetDC();
		// Get the length of the strings to insert
		CSize size = dc->GetTextExtent( l_strTmp );	

		m_LCVersion.InsertColumn(0, "Board Information",  LVCFMT_LEFT, size.cx  );

		m_LCVersion.InsertItem(0, l_strTmp );
		m_LCVersion.SetExtendedStyle(m_LCVersion.GetExtendedStyle()|LVS_EX_GRIDLINES);
	}
	// Other Versions here....
}

