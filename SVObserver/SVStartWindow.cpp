//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStartWindow
//* .File Name       : $Workfile:   SVStartWindow.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:09:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVStartWindow.h"
#include "SVSystemLibrary/SVVersionInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVStartWindowClass 


SVStartWindowClass::SVStartWindowClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVStartWindowClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVStartWindowClass)
	//}}AFX_DATA_INIT
	startImage.Init( IDB_START );

	CString strVersion = "Version ";

	strVersion += SeidenaderVision::SVVersionInfo::GetVersion().c_str();

	startImage.TextOut( 0.42, 0.70, RGB( 0, 0, 0 ), strVersion );
}

SVStartWindowClass::~SVStartWindowClass()
{
}

void SVStartWindowClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVStartWindowClass)
	DDX_Control(pDX, IDC_START_BITMAP, startImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVStartWindowClass, CDialog)
	//{{AFX_MSG_MAP(SVStartWindowClass)
	ON_WM_CTLCOLOR()
	ON_WM_PALETTEISCHANGING()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVStartWindowClass 

HBRUSH SVStartWindowClass::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Attribute des Gerätekontexts hier ändern
	
	// TODO: Anderen Pinsel zurückgeben, falls Standard nicht verwendet werden soll
	return hbr;
}

void SVStartWindowClass::OnPaletteIsChanging(CWnd* pRealizeWnd) 
{
	CDialog::OnPaletteIsChanging(pRealizeWnd);
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	
}

BOOL SVStartWindowClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( startImage.OEMMode || true) // always; EB 2002 08 23
                                    // the bitmap looks terrible when resized
	{
		// In OEM Mode - Resize parent dialog to fit bitmap
		CRect rect;
		GetWindowRect( &rect );

		// Get Bitmap Size
		CSize size = startImage.GetDimensions();
		CSize sizeBitmap = size;

		// EB 2005 02 28
		// get window border size and adjust final window size
		CRect rectFrame, rectClient;
		GetWindowRect(&rectFrame);
		GetClientRect(&rectClient);
		CSize sizeBorder( rectFrame.Width() - rectClient.Width(), rectFrame.Height() - rectClient.Height() );
		size += sizeBorder;
	
		// Center the dialog
		int x = ( rect.left + ( rect.Width() / 2 ) ) - ( size.cx / 2 );
		int y = ( rect.top + ( rect.Height() / 2 ) ) - ( size.cy / 2 );
		SetWindowPos( &wndTop, x, y, size.cx, size.cy, SWP_NOZORDER ); 

		// remove the border from the picture holder
		//startImage.ModifyStyle( WS_BORDER, 0 );
		//startImage.ModifyStyleEx(WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, 0);
		// only these are really necessary...
		startImage.ModifyStyleEx(WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME, 0);
		// Resize the SVDlgBitmapClass
		startImage.SetWindowPos( &wndTop, 0, 0, sizeBitmap.cx, sizeBitmap.cy, SWP_NOZORDER | SWP_NOMOVE ); 
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVStartWindowClass::OnPaint() 
{
	CPaintDC dc(this); // device context for painting


	// Kein Aufruf von CDialog::OnPaint() für Zeichnungsnachrichten
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStartWindow.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:09:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   18 Sep 2012 18:35:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  781
   SCR Title:  Update SVObserver Version Number for Esok Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the creation and management for the version information.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   27 Jan 2011 11:58:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   28 Feb 2005 12:18:52   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  452
   SCR Title:  Upgrade SVObserver to version 4.50
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fix bitmap sizing problem
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   28 Feb 2005 12:11:46   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  452
   SCR Title:  Upgrade SVObserver to version 4.50
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed splash screen bitmap display
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 15:34:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
 */
