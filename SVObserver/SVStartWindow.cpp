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


SVStartWindowClass::SVStartWindowClass(CWnd* pParent /*=nullptr*/)
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

