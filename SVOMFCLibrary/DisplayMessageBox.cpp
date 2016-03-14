//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// .Module Name     : Display message dialog
/// .File Name       : $Workfile:   DisplayMessageBox.cpp  $
/// .Description	 : This class is used to display Information, Warning and Error
///					   messages and is based on the Windows Message box
/// ----------------------------------------------------------------------------
/// .Current Version : $Revision:   1.0  $
/// .Check In Date   : $Date:   18 Feb 2015 02:56:04  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVOResource\resource.h"
#include "DisplayMessageBox.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVOMFCLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int Button1 = 0;
static const int Button2 = 1;
static const int Button3 = 2;

static const COLORREF WhiteColor = RGB( 255, 255, 255);

static const TCHAR ButtonOk[] = _T("OK");
static const TCHAR ButtonCancel[] = _T("Cancel");
static const TCHAR ButtonYes[] = _T("Yes");
static const TCHAR ButtonNo[] = _T("No");
static const TCHAR ButtonAbort[] = _T("Abort");
static const TCHAR ButtonRetry[] = _T("Retry");
static const TCHAR ButtonIgnore[] = _T("Ignore");
static const TCHAR ButtonTry[] = _T("Try");
static const TCHAR ButtonContinue[] = _T("Continue");
#pragma endregion Declarations

IMPLEMENT_DYNAMIC(DisplayMessageBox, CDialog)

BEGIN_MESSAGE_MAP(DisplayMessageBox, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, ID_BUTTON1, ID_BUTTON3, &DisplayMessageBox::OnBnClickedButton)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SHOW_HIDE_DETAILS, &DisplayMessageBox::OnBnClickedShowHideDetails)
	ON_WM_PAINT()
END_MESSAGE_MAP()

#pragma region Constructor
DisplayMessageBox::DisplayMessageBox( CWnd* pParent /*nullptr*/ )
	: CDialog(DisplayMessageBox::IDD, pParent)
	, m_Icon( nullptr )
	, m_Message( _T("") )
	, m_MessageDetails( _T("") )
	, m_ShowDetails( true )
	, m_Type( MB_OK|MB_ICONASTERISK )
{

}

DisplayMessageBox::DisplayMessageBox( CWnd* pParent,  LPCTSTR Message, LPCTSTR MessageDetails, UINT Type )
	: CDialog(DisplayMessageBox::IDD, pParent)
	, m_Icon( nullptr )
	, m_Message( Message )
	, m_MessageDetails( MessageDetails )
	, m_ShowDetails( true )
	, m_Type( Type )
{
	Initialize();
}

DisplayMessageBox::~DisplayMessageBox()
{
}
#pragma endregion Constructor

#pragma region Public Methods
INT_PTR DisplayMessageBox::showDialog( HWND hParent, LPCTSTR Message, LPCTSTR MessageDetails, UINT Type )
{
	INT_PTR Result( IDCANCEL );

	//The setting for hParent can influence the behavior of modal dialogs which call this method to display a message (eg. setting hParent to the main window)
	CWnd* pParent( nullptr );
	if( NULL != hParent )
	{
		pParent = CWnd::FromHandle( hParent );
	}
	
	DisplayMessageBox ErrorMsgBox( pParent, Message, MessageDetails, Type );
	Result = ErrorMsgBox.DoModal();

	return Result;
}
#pragma endregion Public Methods

#pragma region Private Methods
void DisplayMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_BOX_ICON, m_MessageBoxIcon);
	DDX_Control(pDX, IDC_MESSAGE, m_MessageCtrl);
	DDX_Control(pDX, ID_BUTTON1, m_Buttons[Button1]);
	DDX_Control(pDX, ID_BUTTON2, m_Buttons[Button2]);
	DDX_Control(pDX, ID_BUTTON3, m_Buttons[Button3]);
	DDX_Control(pDX, IDC_SHOW_HIDE_DETAILS, m_ShowHideDetails);
	DDX_Control(pDX, IDC_MESSAGE_DETAILS, m_MessageDetailsCtrl);
	DDX_Control(pDX, IDC_DETAILS_BORDER, m_DetailsBorder);
	DDX_Text(pDX, IDC_MESSAGE_DETAILS, m_MessageDetails);
}

BOOL DisplayMessageBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_WhiteBrush.CreateSolidBrush( WhiteColor );

	SetDisplayMessageBoxFont();
	ShowHideMessageDetails();

	if( nullptr != m_Icon )
	{
		m_MessageBoxIcon.SetIcon( m_Icon );
	}

	for( int i=0; i < MaxButtons; i++ )
	{
		m_Buttons[i].SetWindowText( m_ButtonInfos[i].m_Label);
		m_Buttons[i].ShowWindow( m_ButtonInfos[i].m_Show );
	}

	if( m_MessageDetails.IsEmpty() )
	{
		m_ShowHideDetails.ShowWindow( SW_HIDE );
	}

	UpdateData(FALSE);
	return TRUE;
}

void DisplayMessageBox::OnPaint()
{
	CPaintDC dc(this);
	CRect WndRect;
	CRect Rect;

	GetClientRect( WndRect );
	m_DetailsBorder.GetWindowRect( Rect );
	ScreenToClient( Rect );
	WndRect.bottom = Rect.top;
	dc.FillSolidRect( WndRect, WhiteColor);

	if( !m_Message.IsEmpty() )
	{
		CFont* pOriginalFont = dc.SelectObject( &m_DlgFont );
		int nOriginalMode = dc.SetBkMode( TRANSPARENT );

		m_MessageCtrl.GetWindowRect( WndRect );
		ScreenToClient(WndRect);
		Rect.right = WndRect.Width();
		dc.DrawText( m_Message, &Rect, DT_LEFT | DT_WORDBREAK | DT_CALCRECT );

		Rect.OffsetRect( WndRect.left, WndRect.CenterPoint().y - Rect.CenterPoint().y );
		dc.DrawText( m_Message, &Rect, DT_LEFT | DT_WORDBREAK );
		dc.SetBkMode( nOriginalMode );
		dc.SelectObject( pOriginalFont );
	}
}

HBRUSH DisplayMessageBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT CtrlID = pWnd->GetDlgCtrlID();
	if( CTLCOLOR_STATIC == nCtlColor  && IDC_MSG_BOX_ICON == CtrlID )
	{
		pDC->SetBkColor( WhiteColor );
		hbr = static_cast<HBRUSH> (m_WhiteBrush.GetSafeHandle());
	}

	return hbr;
}

void DisplayMessageBox::OnBnClickedButton( UINT nID )
{
	int ButtonIndex( nID-ID_BUTTON1 );
	if( 0 <= ButtonIndex && MaxButtons > ButtonIndex )
	{
		CDialog::EndDialog( m_ButtonInfos[ButtonIndex].m_Result );
	}
}

void DisplayMessageBox::OnBnClickedShowHideDetails()
{
	ShowHideMessageDetails();
}

void DisplayMessageBox::Initialize()
{
	initializeButtons();
	initializeIcon();
}

void DisplayMessageBox::initializeButtons()
{
	switch( MB_TYPEMASK & m_Type )
	{
	case MB_OK:
		{
			m_ButtonInfos[Button1].m_Show = SW_HIDE;
			m_ButtonInfos[Button2].m_Show = SW_HIDE;
			m_ButtonInfos[Button3].m_Label = ButtonOk;
			m_ButtonInfos[Button3].m_Result = IDOK;
		}
		break;
	case MB_OKCANCEL:
		{
			m_ButtonInfos[Button1].m_Show = SW_HIDE;
			m_ButtonInfos[Button2].m_Label = ButtonOk;
			m_ButtonInfos[Button2].m_Result = IDOK;
			m_ButtonInfos[Button3].m_Label = ButtonCancel;
			m_ButtonInfos[Button3].m_Result = IDCANCEL;
		}
		break;
	case MB_ABORTRETRYIGNORE:
		{
			m_ButtonInfos[Button1].m_Label = ButtonAbort;
			m_ButtonInfos[Button1].m_Result = IDABORT;
			m_ButtonInfos[Button2].m_Label = ButtonRetry;
			m_ButtonInfos[Button2].m_Result = IDRETRY;
			m_ButtonInfos[Button3].m_Label = ButtonIgnore;
			m_ButtonInfos[Button3].m_Result = IDIGNORE;
		}
		break;
	case MB_YESNOCANCEL:
		{

			m_ButtonInfos[Button1].m_Label = ButtonYes;
			m_ButtonInfos[Button1].m_Result = IDYES;
			m_ButtonInfos[Button2].m_Label = ButtonNo;
			m_ButtonInfos[Button2].m_Result = IDNO;
			m_ButtonInfos[Button3].m_Label = ButtonCancel;
			m_ButtonInfos[Button3].m_Result = IDCANCEL;
		}
		break;
	case MB_YESNO:
		{
			m_ButtonInfos[Button1].m_Show = SW_HIDE;
			m_ButtonInfos[Button2].m_Label = ButtonYes;
			m_ButtonInfos[Button2].m_Result = IDYES;
			m_ButtonInfos[Button3].m_Label = ButtonNo;
			m_ButtonInfos[Button3].m_Result = IDNO;
		}
		break;
	case MB_RETRYCANCEL:
		{
			m_ButtonInfos[Button1].m_Show = SW_HIDE;
			m_ButtonInfos[Button2].m_Label = ButtonTry;
			m_ButtonInfos[Button2].m_Result = IDRETRY;
			m_ButtonInfos[Button3].m_Label = ButtonCancel;
			m_ButtonInfos[Button3].m_Result = IDCANCEL;
		}
		break;
	case MB_CANCELTRYCONTINUE:
		{
			m_ButtonInfos[Button1].m_Label = ButtonCancel;
			m_ButtonInfos[Button1].m_Result = IDCANCEL;
			m_ButtonInfos[Button2].m_Label = ButtonTry;
			m_ButtonInfos[Button2].m_Result = IDRETRY;
			m_ButtonInfos[Button3].m_Label = ButtonContinue;
			m_ButtonInfos[Button3].m_Result = IDCONTINUE;
		}
		break;
	default:
		{
		}
		break;
	}
}

void DisplayMessageBox::initializeIcon()
{
	TCHAR* pIcon(nullptr);

	switch( MB_ICONMASK & m_Type )
	{
	case MB_ICONHAND:
		{
			pIcon = IDI_HAND;
		}
		break;
	case MB_ICONQUESTION:
		{
			pIcon = IDI_QUESTION;
		}
		break;
	case MB_ICONEXCLAMATION:
		{
			pIcon = IDI_EXCLAMATION;
		}
		break;
	case MB_ICONASTERISK:
		{
			pIcon = IDI_ASTERISK;
		}
		break;
	}
	if( nullptr != pIcon )
	{
		m_Icon = ::LoadIcon( NULL, pIcon );
	}
}

void DisplayMessageBox::ShowHideMessageDetails()
{
	CRect MsgDetailsRect;
	CRect WindowRect;
	int WindowState(SW_HIDE);
	int Height( 0 );

	//Toggle show state
	m_ShowDetails = !m_ShowDetails;
	m_DetailsBorder.GetClientRect( MsgDetailsRect );
	Height = -MsgDetailsRect.Height();
	if( m_ShowDetails )
	{
		Height = -Height;
		WindowState = SW_SHOW;
	}

	//Display the message box in the center of the screen
	GetWindowRect( WindowRect );
	WindowRect.InflateRect( 0, Height/2 );
	MoveWindow( WindowRect );
	m_DetailsBorder.ShowWindow( WindowState );
	m_MessageDetailsCtrl.ShowWindow( WindowState );
	for( int i=0; i < MaxButtons; i++ )
	{
		m_Buttons[i].GetClientRect( WindowRect );
		WindowRect.OffsetRect(0, Height );
		m_Buttons[i].ClientToScreen( WindowRect );
		ScreenToClient( WindowRect );
		m_Buttons[i].MoveWindow( WindowRect );
	}

	CenterWindow();
}

void DisplayMessageBox::SetDisplayMessageBoxFont()
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	//This will get the same font used by the windows MessageBox
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	LOGFONT lfDlgFont = ncm.lfMessageFont;
	m_DlgFont.CreateFontIndirect(&lfDlgFont);

	SetFont(&m_DlgFont, TRUE);
	SendMessageToDescendants(WM_SETFONT, (WPARAM) m_DlgFont.GetSafeHandle(), MAKELPARAM(FALSE, 0), TRUE);
}
#pragma endregion Private Methods

