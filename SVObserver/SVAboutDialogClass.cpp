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

#include "stdafx.h"
#include "SVAboutDialogClass.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

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
	logoBmp = NULL;

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

		CString strVersion = appTitle;
		
		strVersion += _T( " " );
		strVersion += SeidenaderVision::SVVersionInfo::GetVersion().c_str();

		pWnd->SetWindowText( strVersion );
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
	logoBmp = NULL;

	if( 0 == ::_access( _T("C:\\SVObserver\\Bin\\OEM\\logo.bmp"), 0 ) )
	{
		logoBmp = (HBITMAP) ::LoadImage( NULL, 
								_T("C:\\SVObserver\\Bin\\OEM\\logo.bmp"),
								IMAGE_BITMAP,
								0,
								0,
								LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE );
	}// end if

	FILE* oemFile;
	oemFile = _tfopen( _T("C:\\SVObserver\\Bin\\OEM\\oem.txt"),"r" );

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
	BSTR l_bstName = NULL;

	// IO Board Version.....

	// SVBoardVersion enum is used here to make the code more clear.
	// however at some time in the future the Dll parameters may be implemented
	// as an array and therefore this enum may not apply.
	HRESULT l_hr = SVIOConfigurationInterfaceClass::Instance().GetParameterName(SVBoardVersion, &l_bstName );

	if( l_hr == S_OK )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAboutDialogClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:23:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Sep 2012 18:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the creation and management for the version information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Sep 2012 13:57:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed hard coded name in On Init Dialog method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Aug 2012 14:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Oct 2011 09:07:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed scroll in Information box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jan 2011 10:47:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Nov 2010 10:29:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Dec 2009 14:55:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 11:51:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Mar 2006 14:55:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added comments about emuns.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Mar 2006 14:44:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a List box to display additional version information on the SVObserver about box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Apr 2003 16:14:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Nov 2002 09:48:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the drive letter for LoadImage and tfopen statements
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Apr 2001 16:05:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change was required due to changes in SVObserver.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
