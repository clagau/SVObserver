//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVAdjustToolSizePositionDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   03 Oct 2013 13:31:02  $
//******************************************************************************

#include "stdafx.h"
#include <limits>
#include "SVAdjustToolSizePositionDlg.h"

#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"

#include "svobserver.h"
#include "SVTaskObject.h"
#include "SVInspectionProcess.h"
#include "SVGuiExtentUpdater.h"
#include "SVImageListClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool Equal( double d1, double d2 )	// this should move to a library
{
	return abs( d1 - d2 ) < std::numeric_limits<float>::epsilon();	// give a little breathing room (use float instead of double)
}

const int SVAdjustToolSizePositionPropertiesPage::m_iPropertyFilter = SVExtentPropertyPositionsInput | SVExtentPropertyDimensionsInput /* | SVExtentPropertyDimentionsOutput */;

IMPLEMENT_DYNAMIC(SVAdjustToolSizePositionDlg, CPropertySheet)

BEGIN_MESSAGE_MAP(SVAdjustToolSizePositionDlg, CPropertySheet)
	//{{AFX_MSG_MAP(SVAdjustToolSizePositionDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()


SVAdjustToolSizePositionDlg::SVAdjustToolSizePositionDlg( LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, SVTaskObjectClass* pToolTask )
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
{
	m_pToolTask = pToolTask;
	ASSERT( m_pToolTask );
	
	if ( m_pToolTask )
	{
		if ( m_pToolTask->DoesObjectHaveExtents() == S_OK )
			m_pToolTask->GetImageExtent(m_svExtents);

	}
	m_svOriginalExtents = m_svExtents;
	CreatePages();
}

SVAdjustToolSizePositionDlg::~SVAdjustToolSizePositionDlg()
{
	DestroyPages();
}

HRESULT SVAdjustToolSizePositionDlg::CreatePages()
{
	ASSERT( m_pToolTask );

	SVAdjustToolSizePositionMainPage* pMainDlg = new SVAdjustToolSizePositionMainPage(CString(_T("Adjust")), this);
	AddPage(pMainDlg);

	SVAdjustToolSizePositionPropertiesPage* pPropertiesDlg = new SVAdjustToolSizePositionPropertiesPage( CString(_T("Properties")), this );
	AddPage(pPropertiesDlg);

	return S_OK;
}

void SVAdjustToolSizePositionDlg::DestroyPages()
{
	CPropertyPage* pPage;
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		pPage = GetPage( i );
		RemovePage( i );
		if( pPage )
		{
			delete pPage;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVAdjustToolSizePositionDlg message handlers

BOOL SVAdjustToolSizePositionDlg::OnInitDialog() 
{
	m_bModeless = TRUE;	// trick base class into removing the standard buttons
	BOOL bResult = CPropertySheet::OnInitDialog();
	m_bModeless = FALSE;
	
	bool bCancelable = CanCancel();	// from ISVCancel base //SVPropertySheetCancelImpl base
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if ( pWnd )
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );

	//ModifyStyleEx(0, WS_EX_TOOLWINDOW, SWP_DRAWFRAME | SWP_FRAMECHANGED);

	CTabCtrl* pTab = GetTabControl();
	pTab->ModifyStyle(TCS_FOCUSONBUTTONDOWN, TCS_FOCUSNEVER);
	pTab->SetWindowPos(NULL, 1,1, 0,0, SWP_NOSIZE | SWP_NOZORDER );

	pTab->SetItemSize(CSize(1,15));
	pTab->SetPadding(CSize(4,2));

	SetActivePage(1);
	SetActivePage(0);

	int iX = AfxGetApp()->GetProfileInt(_T("State"), _T("ToolAdjustSizePos-X"), 0);
	int iY = AfxGetApp()->GetProfileInt(_T("State"), _T("ToolAdjustSizePos-Y"), 0);

	if ( iX != 0 && iY != 0 )
	{
		SetWindowPos(NULL, iX, iY, 0,0, SWP_NOSIZE | SWP_NOZORDER );
	}

	return bResult;
}

void SVAdjustToolSizePositionDlg::OnDestroy() 
{
	CRect rect;
	GetWindowRect(&rect);
	AfxGetApp()->WriteProfileInt( _T("State"), _T("ToolAdjustSizePos-X"), rect.left );
	AfxGetApp()->WriteProfileInt( _T("State"), _T("ToolAdjustSizePos-Y"), rect.top );
	
	CPropertySheet::OnDestroy();
	
}

void SVAdjustToolSizePositionDlg::OnOK() 
{

	int cnt = GetPageCount();

	for( int i = 0;i < cnt; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			//pPage->OnOK();
		}
	}
	EndDialog(IDOK);
}

void SVAdjustToolSizePositionDlg::OnCancel()
{
	// restore original data
	HRESULT hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, m_svOriginalExtents);

	EndDialog(IDCANCEL);
}

bool SVAdjustToolSizePositionDlg::CanCancel()
{
	ASSERT(m_pToolTask);
	return false;//return m_pTool->CanCancel();
}

HRESULT SVAdjustToolSizePositionDlg::GetCancelData(SVCancelData*& rpData)
{
	ASSERT(m_pToolTask);
	return S_FALSE; //return m_pTool->GetCancelData(rpData);
}

HRESULT SVAdjustToolSizePositionDlg::SetCancelData(SVCancelData* pData)
{
	ASSERT(m_pToolTask);
	return S_FALSE; //return m_pTool->SetCancelData(pData);
}








/////////////////////////////////////////////////////////////////////////////
// SVAdjustToolSizePositionMainPage dialog

BEGIN_MESSAGE_MAP(SVAdjustToolSizePositionMainPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVAdjustToolSizePositionMainPage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOP_LEFT_RADIO, OnModeRadio)
	ON_MESSAGE( WM_SV_NOTIFY_LBUTTONDOWN, OnNotifyLButtonDown )
	ON_MESSAGE( WM_SV_NOTIFY_LBUTTONUP, OnNotifyLButtonUp )
	ON_BN_CLICKED(IDC_MOVETOOL_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_ROTATION_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_BOTTOM_RIGHT_RADIO, OnModeRadio)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnIdOK)
	ON_BN_CLICKED(IDCANCEL, OnIdCancel)
END_MESSAGE_MAP()


SVAdjustToolSizePositionMainPage::SVAdjustToolSizePositionMainPage(const CString& sTitle, SVAdjustToolSizePositionDlg* pParent)
	: CPropertyPage(SVAdjustToolSizePositionMainPage::IDD)
{
	m_sTitle = sTitle;
    m_psp.pszTitle = m_sTitle;
    m_psp.dwFlags |= PSP_USETITLE;
	m_pParentDialog = pParent;
	m_pToolTask = m_pParentDialog->m_pToolTask;
	m_pExtents   = &(m_pParentDialog->m_svExtents);

	//{{AFX_DATA_INIT(SVAdjustToolSizePositionMainPage)
	m_iMode = -1;
	m_sWidthHeight = _T("");
	m_sTopLeft = _T("");
	//}}AFX_DATA_INIT
}

SVAdjustToolSizePositionMainPage::~SVAdjustToolSizePositionMainPage()
{
}

void SVAdjustToolSizePositionMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAdjustToolSizePositionMainPage)
	DDX_Control(pDX, IDC_UP_BUTTON, m_btnUp);
	DDX_Control(pDX, IDC_RIGHT_BUTTON, m_btnRight);
	DDX_Control(pDX, IDC_LEFT_BUTTON, m_btnLeft);
	DDX_Control(pDX, IDC_DOWN_BUTTON, m_btnDown);
	DDX_Radio(pDX, IDC_TOP_LEFT_RADIO, m_iMode);
	DDX_Text(pDX, IDC_WIDTH_HEIGHT, m_sWidthHeight);
	DDX_Text(pDX, IDC_STATIC_TOP_LEFT, m_sTopLeft);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVAdjustToolSizePositionMainPage message handlers

BOOL SVAdjustToolSizePositionMainPage::OnInitDialog() 
{
	ASSERT( m_pToolTask != NULL );

	m_iMode = MODE_MOVE;	// Default action: Move

	//m_pToolTask->GetImageExtent(*m_pExtents);

	CPropertyPage::OnInitDialog();

	m_icoArrowDown = ::LoadImage(AfxGetApp()->m_hInstance,
	                                     MAKEINTRESOURCE(IDI_ARROW_DOWN),
	                                     IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);
	//SVWinHandle<HICON> h( m_icoArrowDown );
	//h = m_icoArrowDown;
	m_icoArrowUp = ::LoadImage(AfxGetApp()->m_hInstance,
	                                   MAKEINTRESOURCE(IDI_ARROW_UP),
	                                   IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);

	m_icoArrowLeft = ::LoadImage(AfxGetApp()->m_hInstance,
	                                     MAKEINTRESOURCE(IDI_ARROW_LEFT),
	                                     IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);

	m_icoArrowRight = ::LoadImage(AfxGetApp()->m_hInstance,
	                                      MAKEINTRESOURCE(IDI_ARROW_RIGHT),
	                                      IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);

	m_icoArrowClockwise = ::LoadImage(AfxGetApp()->m_hInstance,
	                                          MAKEINTRESOURCE(IDI_ARROW_CLOCKWISE),
	                                          IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);

	m_icoArrowCounterclockwise = ::LoadImage(AfxGetApp()->m_hInstance,
	                                                 MAKEINTRESOURCE(IDI_ARROW_COUNTERCLOCKWISE),
	                                                 IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);

	SVExtentPropertyMapType map;
	m_pExtents->GetExtentPropertyList(SVExtentPropertyPositionsInput, map);
	if ( map.find( SVExtentPropertyRotationAngle ) == map.end() )	// if doesn't support rotation angle
	{
		GetDlgItem( IDC_ROTATION_RADIO )->ShowWindow( SW_HIDE );	// hide rotation option
	}

	OnModeRadio();

	UpdateExtentsDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT SVAdjustToolSizePositionMainPage::OnNotifyLButtonDown( WPARAM wParam, LPARAM lParam )
{
	SVUpDownButton* pButton = reinterpret_cast <SVUpDownButton*> (lParam);
	m_pButton = pButton;

	ButtonAction( m_pButton, 0 );

	SetTimer( TIMER_PAUSE, 600, NULL );
	return 0;
}

LRESULT SVAdjustToolSizePositionMainPage::OnNotifyLButtonUp( WPARAM wParam, LPARAM lParam )
{
	SVUpDownButton* pButton = reinterpret_cast <SVUpDownButton*> (lParam);
	KillTimer( TIMER_PAUSE  );
	KillTimer( TIMER_REPEAT );
	m_pButton = NULL;
	return 0;
}


void SVAdjustToolSizePositionMainPage::OnTimer( UINT_CUSTOM nIDEvent )
{
	switch ( nIDEvent )
	{
		case TIMER_PAUSE:

			SetTimer( TIMER_REPEAT, 100, NULL );
			break;

		case TIMER_REPEAT:
			ButtonAction( m_pButton, 0 );
			break;

		default:
			CPropertyPage::OnTimer(nIDEvent);
	}
	
}

HRESULT SVAdjustToolSizePositionMainPage::ButtonAction(SVUpDownButton* pButton, UINT nFlags)
{
	static SVExtentLocationPropertyEnum aActions[2][4] =	// rows are buttons, colums are modes
	{// TOP_LEFT, BOTTOM_RIGHT, MOVE, ROTATE
		{ SVExtentLocationPropertyTop, SVExtentLocationPropertyBottom, SVExtentLocationPropertyCenter, SVExtentLocationPropertyUnknown },	// btnUp & btnDown
		{ SVExtentLocationPropertyLeft, SVExtentLocationPropertyRight, SVExtentLocationPropertyCenter, SVExtentLocationPropertyRotate },	// btnLeft & btnRight
	};
	enum { BTN_UP_DOWN, BTN_LEFT_RIGHT };

	HRESULT hr = S_OK;

	UpdateData();

	int dx = 0;
	int dy = 0;

	SVExtentLocationPropertyEnum eAction = SVExtentLocationPropertyUnknown;

	if ( pButton == &m_btnUp )
	{
		eAction = aActions[ BTN_UP_DOWN ][ m_iMode ];
		dy = -1;
	}
	else if ( pButton == &m_btnDown )
	{
		eAction = aActions[ BTN_UP_DOWN ][ m_iMode ];
		dy = 1;
	}
	else if ( pButton == &m_btnLeft )
	{
		eAction = aActions[ BTN_LEFT_RIGHT ][ m_iMode ];
		dx = -1;
	}
	else if ( pButton == &m_btnRight )
	{
		eAction = aActions[ BTN_LEFT_RIGHT ][ m_iMode ];
		dx = 1;
	}

	// disable properties that use formulas
	bool bDisable = false;
	/*
	SVExtentPropertyInfoStruct info;
	SVTaskObjectClass* pTask = m_pImageView->GetSelectedObject();
	ASSERT( pTask );
	if ( pTask->GetPropertyInfo( eAction, info ) == S_OK )
	{
		bDisable = info.bFormula;
	}
	*/

	if ( !bDisable )
	{
		if ( eAction == SVExtentLocationPropertyRotate )
		{
			hr = AdjustToolAngle( dx );
		}
		else
		{
			hr = AdjustTool( eAction, dx, dy );
		}

		m_pToolTask->GetImageExtent(*m_pExtents);

		UpdateExtentsDisplay();
	}

	return hr;
}

HRESULT SVAdjustToolSizePositionMainPage::AdjustTool( SVExtentLocationPropertyEnum eAction, int dx, int dy )
{
	HRESULT hr = S_OK;

	SVImageExtentClass l_pExtents;

	hr = m_pToolTask->GetImageExtent(l_pExtents);

	if ( hr == S_OK )
	{
		hr = l_pExtents.UpdateFromOutputSpace( eAction, static_cast< long >( dx ),static_cast< long >( dy ) );
	}

	if ( hr == S_OK )
	{
		hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, l_pExtents);
	}	
	return hr;
}


HRESULT SVAdjustToolSizePositionMainPage::AdjustToolAngle(double dDAngle)
{
	HRESULT hr = S_OK;

	SVImageExtentClass l_Extents;

	hr = m_pToolTask->GetImageExtent(l_Extents);

	double dCurrentAngle=0.0;
	hr = l_Extents.GetExtentProperty( SVExtentPropertyRotationAngle, dCurrentAngle );
	if ( l_Extents.GetTranslation() == SVExtentTranslationPolarUnwrap )
	{
		dCurrentAngle += dDAngle;
	}
	else
	{
		dCurrentAngle -= dDAngle;
	}

	if ( hr == S_OK )
	{
		hr = l_Extents.SetExtentProperty( SVExtentPropertyRotationAngle, dCurrentAngle );

		if ( hr == S_OK )
		{
			hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, l_Extents);
			*m_pExtents = l_Extents;
		}
	}
	return hr;
}

void SVAdjustToolSizePositionMainPage::OnModeRadio() 
{
	static CString aCaptions[4][2] = 
	{
		{_T("Top"), _T("Left")},
		{_T("Bottom"), _T("Right")},
		{_T("X Position"), _T("Y Position")},
		{_T("End Angle"), _T("")}
	};

	UpdateData();
	if ( m_iMode == MODE_ROTATE )
	{
		m_btnDown.SetIcon( NULL );
		m_btnUp.SetIcon( NULL );
		m_btnLeft.SetIcon( m_icoArrowClockwise );
		m_btnRight.SetIcon( m_icoArrowCounterclockwise );
	}
	else
	{
		m_btnDown.SetIcon( m_icoArrowDown );
		m_btnUp.SetIcon( m_icoArrowUp );
		m_btnLeft.SetIcon( m_icoArrowLeft );
		m_btnRight.SetIcon( m_icoArrowRight );
	}

	UpdateData(FALSE);
}

BOOL SVAdjustToolSizePositionMainPage::OnSetActive()
{
	BOOL l_bRet = CPropertyPage::OnSetActive();
	UpdateExtentsDisplay();
	return l_bRet;
}

void SVAdjustToolSizePositionMainPage::UpdateExtentsDisplay()
{
	long lWidth = 0;
	long lHeight = 0;

	long lTop = 0;
	long lLeft = 0;

	HRESULT hr1 = m_pExtents->GetExtentProperty( SVExtentPropertyOutputWidth, lWidth );
	HRESULT hr2 = m_pExtents->GetExtentProperty( SVExtentPropertyOutputHeight, lHeight );
	HRESULT hr3 = m_pExtents->GetExtentProperty( SVExtentPropertyPositionPointX, lLeft );
	HRESULT hr4 = m_pExtents->GetExtentProperty( SVExtentPropertyPositionPointY, lTop );
	m_sWidthHeight.Format(_T("%d x %d"), lWidth, lHeight);
	m_sTopLeft.Format(_T("%d, %d"), lLeft, lTop);

	UpdateData(FALSE);
}

BOOL SVAdjustToolSizePositionMainPage::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	::DestroyIcon( m_icoArrowDown  );
	::DestroyIcon( m_icoArrowUp );
	::DestroyIcon( m_icoArrowLeft );
	::DestroyIcon( m_icoArrowRight );
	::DestroyIcon( m_icoArrowClockwise );
	::DestroyIcon( m_icoArrowCounterclockwise );

	return CPropertyPage::DestroyWindow();
}







////////////////////////////////////////////////////////////////////////////////////////////////////////////////







namespace	// file local
{
	static int ID_BASE = 1000;
}

/////////////////////////////////////////////////////////////////////////////
// SVAdjustToolSizePositionPropertiesPage property page

IMPLEMENT_DYNCREATE(SVAdjustToolSizePositionPropertiesPage, CPropertyPage)

BEGIN_MESSAGE_MAP(SVAdjustToolSizePositionPropertiesPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVAdjustToolSizePositionPropertiesPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnIdOK)
	ON_BN_CLICKED(IDCANCEL, OnIdCancel)
    ON_NOTIFY(PTN_ITEMCHANGED, IDC_RESULT_LIST, OnItemChanged)
	ON_BN_CLICKED(IDC_FULL_ROI_BTN, &SVAdjustToolSizePositionPropertiesPage::OnBnClickedFullROI)
	ON_MESSAGE(MSG_TO_PARENT, &SVAdjustToolSizePositionPropertiesPage::OnUserMessage )
END_MESSAGE_MAP()


// Custom handler for loosing PropTree focus.
// prevents button clicked message from getting lost.
LRESULT SVAdjustToolSizePositionPropertiesPage::OnUserMessage(WPARAM wparam, LPARAM lparam)
{
	return SendMessage( WM_COMMAND, wparam, lparam );
	//return 0; 
}

SVAdjustToolSizePositionPropertiesPage::SVAdjustToolSizePositionPropertiesPage(const CString& sTitle, SVAdjustToolSizePositionDlg* pParent) 
	: CPropertyPage(SVAdjustToolSizePositionPropertiesPage::IDD)
{
	m_pParentDialog = pParent;
	m_pExtents = &(m_pParentDialog->m_svExtents);
	m_pToolTask = m_pParentDialog->m_pToolTask;

	m_sTitle = sTitle;
    m_psp.pszTitle = m_sTitle;
    m_psp.dwFlags |= PSP_USETITLE;

	//{{AFX_DATA_INIT(SVAdjustToolSizePositionPropertiesPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVAdjustToolSizePositionPropertiesPage::~SVAdjustToolSizePositionPropertiesPage()
{
}

void SVAdjustToolSizePositionPropertiesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAdjustToolSizePositionPropertiesPage)
	//}}AFX_DATA_MAP
}


BOOL SVAdjustToolSizePositionPropertiesPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	DWORD dwStyle;
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

	// Init the control's size to cover the entire client area
    GetDlgItem(IDC_RESULT_LIST)->GetWindowRect(rc);
    ScreenToClient(rc);
	GetDlgItem(IDC_RESULT_LIST)->DestroyWindow();
	//GetClientRect(&rc);
	//rc.OffsetRect(0, 2);
	//rc.DeflateRect(CSize(2,2));
	// Create SVRPropTree control
	m_Tree.Create(dwStyle, rc, this, IDC_RESULT_LIST);
	m_Tree.SetColumn( rc.Width() / 2 );
	m_Tree.ShowInfoText( false );

	//SVRPropertyItem* pRoot = m_Tree.GetRootItem();
	///*
    SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT( pRoot );
	pRoot->SetCanShrink(false);
    pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2);
	//*/

	SVTaskObjectClass* pTask = m_pToolTask;
	ASSERT( pTask );

	SVExtentPropertyListType list;
	HRESULT hr = pTask->GetFilteredImageExtentPropertyList( list );

	SVExtentPropertyMapType map;
	m_pExtents->GetExtentPropertyList((SVExtentPropertyEnum) m_iPropertyFilter, map);
	SVExtentPropertyMapType::const_iterator iter;
	
	for( iter = map.begin(); iter != map.end(); ++iter )
	{
		if ( std::find( list.begin(), list.end(), iter->first ) == list.end() )	// if item is not in filtered list, skip
			continue;

		SVRPropertyItemEdit* pEdit = dynamic_cast<SVRPropertyItemEdit*>(m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		ASSERT( pEdit );

		CString sName = iter->second;  
		double dValue = 0.0;
		m_pExtents->GetExtentProperty( iter->first, dValue);  

		//pEdit->SetCtrlID( iID++ );	// needs to be a postincrement here
		pEdit->SetCtrlID( ID_BASE + (int) iter->first );

		CString sLabel;
		sLabel = sName;
		pEdit->SetLabelText( sLabel );

		pEdit->SetBold( false );
		pEdit->SetHeight(16);

		CString sValue;
		sValue = AsString(dValue);
		pEdit->SetItemValue( sValue );
		pEdit->OnRefresh();
	}//end for( iter = map.begin(); iter != map.end(); iter++ )

	///*
	SVRPropertyItem* pChild = pRoot->GetChild();
	while ( pChild )
	{
		pChild->Expand( TRUE );
		pChild = pChild->GetSibling();
	}
	pRoot->Expand( true );	// needed for redrawing
	//*/

	UpdateData(FALSE);

	bool l_bShow = dynamic_cast<AllowResizeToParent *>(m_pToolTask) != NULL;
	GetDlgItem(IDC_FULL_ROI_BTN)->ShowWindow( l_bShow ? SW_SHOW : SW_HIDE );
	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow( !IsFullSize() );

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVAdjustToolSizePositionPropertiesPage::FillTreeFromExtents()
{

    SVRPropertyItem* pRoot = m_Tree.GetRootItem();
	ASSERT( pRoot );

	SVTaskObjectClass* pTask = m_pToolTask;
	ASSERT( pTask );

	SVExtentPropertyListType list;
	HRESULT hr = pTask->GetFilteredImageExtentPropertyList( list );

	SVExtentPropertyMapType map;
	m_pExtents->GetExtentPropertyList((SVExtentPropertyEnum) m_iPropertyFilter, map);
	SVExtentPropertyMapType::const_iterator iter;
	
	for( iter = map.begin(); iter != map.end(); ++iter )
	{
		if ( std::find( list.begin(), list.end(), iter->first ) == list.end() )	// if item is not in filtered list, skip
			continue;

		SVRPropertyItemEdit* pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.FindItem(ID_BASE + (int) iter->first));
		ASSERT( pEdit );
		if ( pEdit )
		{
			CString sName = iter->second;  
			double dValue = 0.0;
			m_pExtents->GetExtentProperty( iter->first, dValue);  

			CString sValue;
			sValue = AsString(dValue);
			pEdit->SetItemValue( sValue );
			pEdit->OnRefresh();
		}
	}//end for( iter = map.begin(); iter != map.end(); iter++ )

	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow( !IsFullSize() );

}

void SVAdjustToolSizePositionPropertiesPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
	*plResult = S_OK;
	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		// do validation
		SVExtentPropertyEnum eProperty = (SVExtentPropertyEnum) (pItem->GetCtrlID() - ID_BASE);
		CString sValue;
		pItem->GetItemValue( sValue );
		double dValue = atof( sValue );

		bool bValidated = true;

		if ( !bValidated )
		{
			*plResult = S_FALSE;
		}
		else
		{
			m_pToolTask->GetImageExtent( *m_pExtents );

			double dOldValue;
			m_pExtents->GetExtentProperty( eProperty, dOldValue );
			if ( !Equal( dOldValue, dValue ) )
			{
				m_pExtents->SetExtentProperty( eProperty, dValue );

				m_pExtents->UpdateData();

				m_pExtents->GetExtentProperty( eProperty, dValue );
				sValue = AsString(dValue);
				pItem->SetItemValue( sValue );
				pItem->OnRefresh();

				//
				HRESULT hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, *m_pExtents);
				m_pToolTask->GetImageExtent( *m_pExtents );
				FillTreeFromExtents();
			}
		}
	}
}

BOOL SVAdjustToolSizePositionPropertiesPage::OnSetActive() 
{
	// Update all fields
	SVRPropertyItem* pItem = m_Tree.GetRootItem();
	ASSERT( pItem );
	pItem = pItem->GetChild();
	ASSERT( pItem );
	pItem = pItem->GetChild();
	//ASSERT( pItem );

	while ( pItem )
	{
		SVExtentPropertyEnum eProperty = (SVExtentPropertyEnum) (pItem->GetCtrlID() - ID_BASE);
		double dValue = 0.0;
		m_pExtents->GetExtentProperty( eProperty, dValue);  


		CString sValue;
		sValue = AsString(dValue);
		pItem->SetItemValue( sValue );
		pItem->OnRefresh();

		pItem = pItem->GetSibling();
	}// end while ( pItem )
	
	return CPropertyPage::OnSetActive();
}


void SVAdjustToolSizePositionMainPage::OnIdOK() 
{
	m_pParentDialog->OnOK();
	CPropertyPage::OnOK();
}

void SVAdjustToolSizePositionMainPage::OnIdCancel() 
{
	m_pParentDialog->OnCancel();
	CPropertyPage::OnCancel();
}

void SVAdjustToolSizePositionPropertiesPage::OnIdCancel() 
{
	m_pParentDialog->OnCancel();
	CPropertyPage::OnCancel();
}

void SVAdjustToolSizePositionPropertiesPage::OnIdOK() 
{
	m_pParentDialog->OnOK();
	CPropertyPage::OnOK();
}
void SVAdjustToolSizePositionPropertiesPage::OnBnClickedFullROI()
{
	HRESULT hr = SVGuiExtentUpdater::SetImageExtentToParent(m_pToolTask );

	if( hr == S_OK )
	{
		m_pToolTask->GetImageExtent(*m_pExtents);

		FillTreeFromExtents();
		m_Tree.Invalidate();
	}
}


bool SVAdjustToolSizePositionPropertiesPage::IsFullSize()
{
	bool l_bFull = false;
	long lWidth = 0;
	long lHeight = 0;
	long lTop = 0;
	long lLeft = 0;
	HRESULT hr1 = m_pExtents->GetExtentProperty( SVExtentPropertyOutputWidth, lWidth );
	HRESULT hr2 = m_pExtents->GetExtentProperty( SVExtentPropertyOutputHeight, lHeight );
	HRESULT hr3 = m_pExtents->GetExtentProperty( SVExtentPropertyPositionPointX, lLeft );
	HRESULT hr4 = m_pExtents->GetExtentProperty( SVExtentPropertyPositionPointY, lTop );

	m_pToolTask->GetImageExtent(*m_pExtents);
	SVImageExtentClass l_ParentExtent;
	if( m_pToolTask->GetTool()->GetParentExtent( l_ParentExtent ) == S_OK )
	{
		long l_lParentWidth=0;
		long l_lParentHeight=0;
		long l_lParentTop=0;
		long l_lParentLeft=0;
		l_ParentExtent.GetExtentProperty( SVExtentPropertyOutputWidth, l_lParentWidth );
		l_ParentExtent.GetExtentProperty( SVExtentPropertyOutputHeight, l_lParentHeight );
		l_ParentExtent.GetExtentProperty( SVExtentPropertyPositionPointX, l_lParentTop );
		l_ParentExtent.GetExtentProperty( SVExtentPropertyPositionPointY, l_lParentLeft );
		if( l_lParentWidth == lWidth
			&& l_lParentHeight == lHeight
			&& l_lParentTop == l_lParentTop
			&& l_lParentLeft == l_lParentLeft )
		{
			l_bFull = true;
		}
	}
	return l_bFull;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAdjustToolSizePositionDlg.cpp_v  $
 * 
 *    Rev 1.5   03 Oct 2013 13:31:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Oct 2013 12:05:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Sep 2013 16:13:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  860
 * SCR Title:  Resize Icons Should Change Based on a Tool's Rotation
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated comment regarding m_icoArrowDown.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2013 15:01:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Jun 2013 06:28:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added user message to prevent button clicked message from getting lost when loosing focus from PropTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Apr 2013 08:26:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dialog components to support Full Image button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:27:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Oct 2012 10:36:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2012 16:01:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with updating tool extents when the tool is off the image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Aug 2012 11:43:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Apr 2011 14:16:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues when setting the properties for the Adjust Tool Size/Position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Mar 2011 08:04:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues with Adjust Tool position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Mar 2011 13:19:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Oct 2007 16:35:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DestroyWindow Function to free GDI Resources
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2006 16:58:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnItemChanged method to fix an issue with invalid extent data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:08:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
