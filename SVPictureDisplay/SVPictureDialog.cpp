//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDialog
//* .File Name       : $Workfile:   SVPictureDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   14 Aug 2014 17:35:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <cmath>
#include "SVPictureDisplay.h"
#include "SVPictureDialog.h"
#include "SVPictureTabs.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


IMPLEMENT_DYNAMIC(SVPictureDialog, CDialog)

SVPictureDialog::SVPictureDialog( long p_lTab, AppearanceTypes p_lAppearance )
: CDialog( SVPictureDialog::IDD )
, m_ButtonSize(40)
, m_lButtonSpacing(10)
, m_lButtonJustify(0)
, m_lScrollBarWidth(20)
, m_bSaveButton(false)
, m_PictDisplay(p_lTab)
, m_bZoomSlider(true)
, m_eAppearance( p_lAppearance )
, m_StaticZoomMaxTextLen(5)
{
	LoadToolbarIcons();
}

SVPictureDialog::~SVPictureDialog()
{
	// release Icons
	for (IconMap::iterator it = m_Icons.begin(); it != m_Icons.end(); ++it)
	{
		::DestroyIcon(it->second);
	}
}

void SVPictureDialog::LoadToolbarIcons()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	HICON l_TmpIcon = static_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE(IDI_ZOOM_IN_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED ) );
	m_Icons.insert(std::make_pair(IDI_ZOOM_IN_ICON, l_TmpIcon));

	l_TmpIcon = static_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE(IDI_ZOOM_OUT_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED ) );
	m_Icons.insert(std::make_pair(IDI_ZOOM_OUT_ICON, l_TmpIcon));

	l_TmpIcon = static_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE(IDI_ZOOM_FIT_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED ) );
	m_Icons.insert(std::make_pair(IDI_ZOOM_FIT_ICON, l_TmpIcon));

	l_TmpIcon = static_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE(IDI_ZOOM_ONE_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED ) );
	m_Icons.insert(std::make_pair(IDI_ZOOM_ONE_ICON, l_TmpIcon));

	l_TmpIcon = static_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE(IDI_SAVE_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED ) );
	m_Icons.insert(std::make_pair(IDI_SAVE_ICON, l_TmpIcon));
}

void SVPictureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICT_DISPLAY, m_PictDisplay);
	DDX_Control(pDX, IDC_HORIZ_SCROLL, m_HorizScroll);
	DDX_Control(pDX, IDC_VERTICAL_SCROLL, m_VertScroll);
	DDX_Control(pDX, IDC_ZOOM_PLUS, m_ButtonZoomPlus);
	DDX_Control(pDX, IDC_ZOOM_MINUS, m_ButtonZoomMinus);
	DDX_Control(pDX, IDC_ZOOM_FIT, m_ButtonZoomFit);
	DDX_Control(pDX, IDC_ZOOM_ONE, m_ButtonZoomOne);
	DDX_Control(pDX, IDC_SAVE_BUTTON, m_ButtonSave);
	DDX_Control(pDX, IDC_STATIC_ZOOM, m_StaticZoom);
	DDX_Control(pDX, IDC_SLIDER_ZOOM, m_SliderCtrlZoom);
}

BEGIN_MESSAGE_MAP(SVPictureDialog, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_ZOOM_PLUS, &SVPictureDialog::OnBnClickedZoomPlus)
	ON_BN_CLICKED(IDC_ZOOM_MINUS, &SVPictureDialog::OnBnClickedZoomMinus)
	ON_BN_CLICKED(IDC_ZOOM_FIT, &SVPictureDialog::OnBnClickedZoomFit)
	ON_BN_CLICKED(IDC_ZOOM_ONE, &SVPictureDialog::OnBnClickedZoomOne)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &SVPictureDialog::OnBnClickedSaveButton)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// SVPictureDialog message handlers

BOOL SVPictureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(0, WS_EX_TRANSPARENT);

	// Initialize to the first bitmap.
	m_PictDisplay.SetZoomFactor(1.0f);
	m_PictDisplay.SetZoom(ZoomOneToOne);
	PutIconsOnButtons();
	SetScrollBars();
	PositionVerticalSlider( 0, m_ButtonSize, 0, 0, IDC_VERTICAL_SCROLL );
	PositionHorizontalSlider( 0, m_ButtonSize, 0, 0, IDC_HORIZ_SCROLL );
	PositionPictureDisplay();
	PositionButtons();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVPictureDialog::IsScrollbarHidden()
{
	BOOL res = FALSE;

	SVPictureTabs* pPictureTab = GetPictureTab();
	if(pPictureTab)
	{
		res = (pPictureTab->IsScrollbarHidden());
	}
	return res;
}

void SVPictureDialog::AdjustSizeToImage()
{
	m_PictDisplay.AdjustSizeToImage();
}

void SVPictureDialog::PositionVerticalSlider( int LeftSpacing, int TopSpacing, int RightSpacing, int BottomSpacing, unsigned int ID )
{
	UINT flag = IsScrollbarHidden() ? SWP_HIDEWINDOW : SWP_SHOWWINDOW;
	CRect rect;
	CDialog::GetClientRect(&rect);

	rect.right -= LeftSpacing + RightSpacing;
	rect.bottom -= TopSpacing + BottomSpacing;

	int calcWidth = m_lScrollBarWidth;
	int calcHeight = rect.Height() - calcWidth;
	CWnd* pWin = GetDlgItem( ID );
	if( pWin )
	{
		pWin->SetWindowPos( &CWnd::wndTop,
			LeftSpacing + ( rect.right - calcWidth ),
			TopSpacing,
			calcWidth,
			calcHeight,
			flag );
	}
}

// Positions a horizontal slider at the bottom of the screen
void SVPictureDialog::PositionHorizontalSlider( int LeftSpacing, int TopSpacing, int RightSpacing, int BottomSpacing, unsigned int ID )
{
	UINT flag = IsScrollbarHidden() ? SWP_HIDEWINDOW : SWP_SHOWWINDOW;
	CRect rect;
	CDialog::GetClientRect(&rect);

	rect.right -= LeftSpacing + RightSpacing;
	rect.bottom -= TopSpacing + BottomSpacing;

	int calcHeight = m_lScrollBarWidth;
	int calcWidth = rect.Width() - calcHeight;
	CWnd* pWin = GetDlgItem( ID );
	if( pWin )
	{
		pWin->SetWindowPos( &CWnd::wndTop,
			LeftSpacing,
			TopSpacing + (rect.bottom - calcHeight ),
			calcWidth,
			calcHeight,
			flag );
	}
}

void SVPictureDialog::PositionPictureDisplay()
{
	if( !::IsWindow( m_PictDisplay.m_hWnd ) )
	{
		return;
	}

	CRect rect;
	CDialog::GetClientRect(&rect);
	long ScrollBarWidth = IsScrollbarHidden() ? 0 : m_lScrollBarWidth;
	long gap = 0;

	switch( m_eAppearance )
	{
	case TopButtons:
		{
			m_PictDisplay.SetWindowPos(&CWnd::wndTop, 
				0,														// x
				m_ButtonSize,											// y
				rect.Width() - ScrollBarWidth - gap,					// width
				rect.Height() - m_ButtonSize - ScrollBarWidth - gap,	// Height
				SWP_SHOWWINDOW );
			break;
		}
	case RightButtons:
		{
			m_PictDisplay.SetWindowPos(&CWnd::wndTop, 
				0,													// x
				0,													// y
				rect.Width() - m_ButtonSize - ScrollBarWidth - gap,	// width
				rect.Height() - ScrollBarWidth - gap,				// Height
				SWP_SHOWWINDOW );
			break;
		}
	case LeftButtons:
		{
			m_PictDisplay.SetWindowPos(&CWnd::wndTop, 
				m_ButtonSize,										// x
				0,													// y
				rect.Width()- m_ButtonSize - ScrollBarWidth - gap,	// width
				rect.Height() - ScrollBarWidth - gap,				// Height
				SWP_SHOWWINDOW );
			break;
		}
	case BottomButtons:
		{
			m_PictDisplay.SetWindowPos(&CWnd::wndTop, 
				0,														// x
				0,														// y
				rect.Width() - ScrollBarWidth - gap,					// width
				rect.Height() - m_ButtonSize - ScrollBarWidth - gap,	// Height
				SWP_SHOWWINDOW );
			break;
		}

	case HideButtons:
		{
			m_PictDisplay.SetWindowPos(&CWnd::wndTop, 
				0,										// x
				0,										// y
				rect.Width() - ScrollBarWidth - gap,	// width
				rect.Height() - ScrollBarWidth - gap,	// Height
				SWP_SHOWWINDOW );
			break;
		}
	default:
		{
			break; // Do nothing.
		}
	}
	
	m_PictDisplay.SaveMaxRect();
}

void SVPictureDialog::PositionButtons()
{
	long ScrollBarWidth = IsScrollbarHidden() ? 0 : m_lScrollBarWidth;

	if(!::IsWindow( m_StaticZoom.m_hWnd) ||!::IsWindow( m_SliderCtrlZoom.m_hWnd) ||!::IsWindow( m_ButtonSave.m_hWnd) )
	{
		return;
	}

	if(!::IsWindow( m_ButtonZoomPlus.m_hWnd) ||!::IsWindow( m_ButtonZoomMinus.m_hWnd) ||!::IsWindow( m_ButtonZoomFit.m_hWnd) )
	{
		return;
	}

	if(m_eAppearance == HideButtons)
	{
		m_ButtonZoomPlus.ShowWindow( SW_HIDE );
		m_ButtonZoomMinus.ShowWindow( SW_HIDE );
		m_ButtonZoomFit.ShowWindow( SW_HIDE );
		m_ButtonZoomOne.ShowWindow( SW_HIDE );
		m_ButtonSave.ShowWindow( SW_HIDE );
		m_StaticZoom.ShowWindow( SW_HIDE );
		m_SliderCtrlZoom.ShowWindow( SW_HIDE );
		return;
	}

	m_StaticZoom.ShowWindow( SW_HIDE );
	m_SliderCtrlZoom.ShowWindow(SW_HIDE);

	m_ButtonSave.ShowWindow( m_bSaveButton ? SW_SHOW : SW_HIDE );

	CRect rect;
	CDialog::GetClientRect( &rect );
	SVPictureTabs* pPictureTab = GetPictureTab();
	if(pPictureTab)
	{
		m_bZoomSlider = (pPictureTab->GetShowZoomSlider() == TRUE);
	}

	int size = 32;
	int ButtonCount = 4;
	if( m_bSaveButton )
	{
		ButtonCount += 1;
	}

	switch( m_eAppearance )
	{
	case TopButtons: // fall through...
	case BottomButtons:
		{
			int JustifyComp = 0;
			int SliderLen = 0;
			///Calculate slider Len
			if(m_bZoomSlider)
			{
				SliderLen = rect.Width() - (ButtonCount * size) - size - (ButtonCount + 3) * m_lButtonSpacing;
				if(SliderLen < 3 * size)
				{
 					SliderLen = 0;
					m_bZoomSlider = 0;
				}
				else
				{
					SliderLen = 3 * size;
				}
			}

			if( m_lButtonJustify )
			{
				JustifyComp = rect.right - ButtonCount * size - (ButtonCount + 1) * m_lButtonSpacing - ScrollBarWidth;
				if(m_bZoomSlider)
				{
					JustifyComp -= ( SliderLen + size );
				}
			}
			long x = JustifyComp;
			long y = 0;
			if(m_eAppearance == BottomButtons)
			{
				y = rect.bottom - size;
			}
			if(m_bZoomSlider)
			{
				m_StaticZoom.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
				x += 2*m_lButtonSpacing + size;
			}
			else
			{
				x += m_lButtonSpacing;
			}

			m_ButtonZoomMinus.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			x += size;
			if(m_bZoomSlider)
			{
				m_SliderCtrlZoom.SetWindowPos( &CWnd::wndTop, x, y, SliderLen, size, SWP_SHOWWINDOW );
				x += SliderLen;
				m_SliderCtrlZoom.ModifyStyle(0, TBS_HORZ, 0);
			}
			else
			{
				x += m_lButtonSpacing;
			}
			m_ButtonZoomPlus.SetWindowPos(&CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW ); 
			x += size + m_lButtonSpacing;
			if(m_bZoomSlider)
			{
				x += m_lButtonSpacing;
			}
			m_ButtonZoomFit.SetWindowPos(&CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW ); 
			x += size + m_lButtonSpacing;
			m_ButtonZoomOne.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW ); 
			x += size + m_lButtonSpacing;
			if( m_bSaveButton )
			{
				m_ButtonSave.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			}

			m_ButtonSize = size;
			break;
		}

	case LeftButtons: // fall through...
	case RightButtons:
		{
			int JustifyComp = 0;

			int SliderHeight = 0;
			///Calculate slider Len
			if(m_bZoomSlider)
			{
				SliderHeight = rect.Height() - (ButtonCount * size) - size - (ButtonCount +1) * m_lButtonSpacing;
				if(SliderHeight < 3 * size)
				{
					SliderHeight = 0;
					m_bZoomSlider = 0;
				}
				else
				{
					SliderHeight = 3 * size;
				}
			}

			if( m_lButtonJustify )
			{
				JustifyComp = rect.Height() - ButtonCount * size - (ButtonCount + 1) * m_lButtonSpacing - ScrollBarWidth;
				if(m_bZoomSlider)
				{
					JustifyComp -= ( SliderHeight + size );
				}
			}

			if( m_lButtonJustify )
			{
				JustifyComp = rect.right - ButtonCount * size - (ButtonCount + 1) * m_lButtonSpacing - ScrollBarWidth;
			}
			long x = rect.right - size;
			long y = JustifyComp + m_lButtonSpacing;

			if(m_eAppearance == LeftButtons)
			{
				x = 0;
			}

			if(m_bZoomSlider)
			{
				m_StaticZoom.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
				y += m_lButtonSpacing + size;
			}
			m_ButtonZoomPlus.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			y += size + m_lButtonSpacing;
			if(m_bZoomSlider)
			{
				m_SliderCtrlZoom.SetWindowPos( &CWnd::wndTop, x, y, size, 3*size, SWP_SHOWWINDOW );
				y += 3*size;
				m_SliderCtrlZoom.ModifyStyle(0, TBS_VERT, 0);
			}
			else
			{
				y += m_lButtonSpacing;
			}
			m_ButtonZoomMinus.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			y += size + m_lButtonSpacing;
			m_ButtonZoomFit.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			y += size + m_lButtonSpacing;
			m_ButtonZoomOne.SetWindowPos( &CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			y += size + m_lButtonSpacing;
			if( m_bSaveButton )
			{
				m_ButtonSave.SetWindowPos(&CWnd::wndTop, x, y, size, size, SWP_SHOWWINDOW );
			}
			m_ButtonSize = size;
			break;
		}

	default:
		{
			break; // Do nothing.
		}
	}
	// The next few lines make the buttons all look selected. So they do not change as you press them.

	m_ButtonZoomMinus.SetButtonStyle( BS_DEFPUSHBUTTON );

	m_ButtonZoomPlus.SetButtonStyle( BS_DEFPUSHBUTTON );

	m_ButtonZoomFit.SetButtonStyle( BS_DEFPUSHBUTTON );

	m_ButtonZoomOne.SetButtonStyle( BS_DEFPUSHBUTTON );

	if( m_bSaveButton )
	{
		m_ButtonSave.SetButtonStyle( BS_DEFPUSHBUTTON );
	}

	if(m_bZoomSlider)
	{
		double zoomMin, zoomMax;
		m_PictDisplay.GetZoomRanges(zoomMin, zoomMax);
		int max  = int(SliderProp * zoomMax);
		int min = int(SliderProp * zoomMin);
		if( m_eAppearance == LeftButtons || m_eAppearance == RightButtons )
		{
			m_SliderCtrlZoom.SetRangeMin(-max);
			m_SliderCtrlZoom.SetRangeMax(-min);
		}
		else
		{
			m_SliderCtrlZoom.SetRangeMin(max);
			m_SliderCtrlZoom.SetRangeMax(min);
		}
	}
}

void SVPictureDialog::PutIconsOnButtons()
{
	PutIconOnButton( IDI_ZOOM_IN_ICON, IDC_ZOOM_PLUS );
	PutIconOnButton( IDI_ZOOM_OUT_ICON, IDC_ZOOM_MINUS );
	PutIconOnButton( IDI_ZOOM_FIT_ICON, IDC_ZOOM_FIT );
	PutIconOnButton( IDI_ZOOM_ONE_ICON, IDC_ZOOM_ONE );
	PutIconOnButton( IDI_SAVE_ICON, IDC_SAVE_BUTTON );
}

bool SVPictureDialog::PutIconOnButton( unsigned int p_iIconResource, unsigned int p_iButton )
{
	bool l_bOk = false;

	CButton* pBtn = (CButton*)GetDlgItem( p_iButton );
	if( pBtn != NULL )
	{
		IconMap::iterator it = m_Icons.find(p_iIconResource);
		if (it != m_Icons.end())
		{
			HICON l_TmpIcon = it->second;

			if( l_TmpIcon != NULL )
			{
				pBtn->SetIcon( l_TmpIcon );
				l_bOk = true;
			}
		}
	}
	return l_bOk;
}

HRESULT SVPictureDialog::SetScrollBars()
{
	HRESULT hr = S_FALSE;
	// Sets the scroll bars based on the picture that is displayed.
	const SIZE& size = m_PictDisplay.GetImageSize();
	double zoom = m_PictDisplay.GetZoomFactor();

	CRect MaxRect;
	m_PictDisplay.GetViewMaxRect(MaxRect);

	SCROLLINFO info;
	memset( &info, 0, sizeof( SCROLLINFO ) );
	info.cbSize = sizeof( SCROLLINFO );
	m_VertScroll.GetScrollInfo(&info, SIF_POS);
	info.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL | SIF_POS;

	info.nMin = 0;
	info.nPage = static_cast<int>(MaxRect.Height() / zoom);

	if( info.nPage >= static_cast<unsigned int>( size.cy ) )
	{
		info.nMax = 0;
	}
	else
	{
		info.nMax = size.cy;
	}

	int maxpos = info.nMax - max(info.nPage, 0);
	
	info.nPos = min(info.nPos, maxpos);
	info.nPos = max(info.nPos, info.nMin);
	m_VertScroll.SetScrollInfo( &info );
	m_PictDisplay.SetYOffset(info.nPos);

	m_HorizScroll.GetScrollInfo(&info, SIF_POS);
	info.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL | SIF_POS;
	info.nMin = 0;
	info.nPage = static_cast<int>((MaxRect.Width() / zoom));

	if( info.nPage >= static_cast<unsigned int>( size.cx ) )
	{
		info.nMax = 0;
	}
	else
	{
		info.nMax = size.cx;
	}

	maxpos = info.nMax - max(info.nPage, 0);

	info.nPos = min( info.nPos, maxpos );
	info.nPos = max( info.nPos, info.nMin );
	m_HorizScroll.SetScrollInfo( &info );
	m_PictDisplay.SetXOffset(info.nPos);

	hr = S_OK;
	return hr;
}

// Debug Function
void SVPictureDialog::ToEditCtrl( unsigned int p_iResource, long p_lValue )
{
	CEdit* l_pEdit = dynamic_cast< CEdit* >( GetDlgItem( p_iResource ) );
	if( l_pEdit != nullptr )
	{
		CString l_strTmp;
		l_strTmp.Format( _T("%d"), p_lValue );
		l_pEdit->SetWindowText( l_strTmp );
	}
}

void SVPictureDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWnd* pWnd = dynamic_cast< CWnd* >( pScrollBar );
	int CtrlId = pWnd->GetDlgCtrlID();
	if(CtrlId == IDC_SLIDER_ZOOM)
	{
		ZoomToSliderPos();
		SetScrollBars();
		CloneScrollPosition(SB_BOTH);

		return;
	}

	// Get the minimum and maximum scroll-bar positions.
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	pScrollBar->GetScrollInfo(&info, SIF_ALL);
	int minpos = info.nMin;
	int maxpos = info.nMax - max( info.nPage, 0 );
	int curpos = info.nPos;

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if ( curpos > minpos ) { curpos--; }
		break;

	case SB_LINERIGHT:   // Scroll right.
		if ( curpos < maxpos ) { curpos++; }
		break;

	case SB_PAGELEFT:    // Scroll one page left.
		{
			if (curpos > minpos)
			{
				curpos = max( minpos, curpos -  static_cast< int >( info.nPage ) );
			}
		}
		break;

	case SB_PAGERIGHT:      // Scroll one page right.
		{
			if (curpos < maxpos)
			{
				curpos = min( maxpos, curpos + static_cast< int >( info.nPage ) );
			}
		}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;

	default:
		break; // Do nothing.
	}

	//check position
	curpos = min( curpos, maxpos );
	curpos = max( curpos, minpos );

	pScrollBar->SetScrollPos(curpos);
	m_PictDisplay.SetXOffset(curpos);
	m_PictDisplay.Refresh();
	if(GetPictureTab()->GetZoomSimultaneous())
	{
		GetPictureTab()->CloneScrollPosition(SB_HORZ, m_PictDisplay.GetTabHandler());
	}

	// Set the new position of the thumb (scroll box).
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVPictureDialog::CloneScrollPosition(int orientation)
{
	if(GetPictureTab()->GetZoomSimultaneous() == FALSE)
	{
		return;
	}

	switch (orientation)
	{
	case SB_HORZ:
		GetPictureTab()->CloneScrollPosition(SB_HORZ, m_PictDisplay.GetTabHandler());
		break;

	case SB_VERT:
		GetPictureTab()->CloneScrollPosition(SB_VERT, m_PictDisplay.GetTabHandler());
		break;

	case SB_BOTH:
		GetPictureTab()->CloneScrollPosition(SB_HORZ, m_PictDisplay.GetTabHandler());
		GetPictureTab()->CloneScrollPosition(SB_VERT, m_PictDisplay.GetTabHandler());
		break;

	default:
		break;
	}
}

int SVPictureDialog::GetScrollPosition(int orientation) const
{
	int res = 0;
	switch(orientation)
	{
	case SB_HORZ:
		res = m_HorizScroll.GetScrollPos();
		break;

	case SB_VERT:
		res = m_VertScroll.GetScrollPos();
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return res;
}

void SVPictureDialog::SetScrollPosition(int orientation, int pos)
{
	switch(orientation)
	{
	case SB_HORZ:
		m_HorizScroll.SetScrollPos(pos);
		m_PictDisplay.SetXOffset(pos);
		break;
	case SB_VERT:
		m_VertScroll.SetScrollPos(pos);
		m_PictDisplay.SetYOffset(pos);
		break;
	case SB_BOTH:
		m_HorizScroll.SetScrollPos(pos);
		m_PictDisplay.SetXOffset(pos);
		m_VertScroll.SetScrollPos(pos);
		m_PictDisplay.SetYOffset(pos);
		break;
	default:
		break;
	}
}

void SVPictureDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWnd* pWnd = dynamic_cast< CWnd* >( pScrollBar );
	int CtrlId = pWnd->GetDlgCtrlID();
	if(CtrlId == IDC_SLIDER_ZOOM)
	{
		if ( m_bZoomSlider )
		{
			ZoomToSliderPos();
			SetScrollBars();
			CloneScrollPosition(SB_BOTH);
		}
		return;
	}

	// Get the minimum and maximum scroll-bar positions.
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	pScrollBar->GetScrollInfo(&info, SIF_ALL);
	int minpos = info.nMin;
	int maxpos = info.nMax - max(info.nPage, 0);
	int curpos = info.nPos;

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos) { curpos--; }
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos) { curpos++; }
		break;

	case SB_PAGELEFT:    // Scroll one page left.
		{
			if (curpos > minpos)
			{
				curpos = max( minpos, curpos - static_cast< int >( info.nPage ) );
			}
		}
		break;

	case SB_PAGERIGHT:      // Scroll one page right.
		{
			if (curpos < maxpos)
			{
				curpos = min(maxpos, curpos + static_cast< int >( info.nPage ) );
			}
		}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;     // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;    // position that the scroll box has been dragged to.
		break;

	default:
		break;
	}

	// Check Position
	curpos = min( curpos, maxpos );
	curpos = max( curpos, minpos );

	// Set the new position of the thumb (scroll box).
	pScrollBar->SetScrollPos(curpos);
	m_PictDisplay.SetYOffset(curpos);
	m_PictDisplay.Refresh();

	if(GetPictureTab()->GetZoomSimultaneous())
	{
		GetPictureTab()->CloneScrollPosition(SB_VERT, m_PictDisplay.GetTabHandler());
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SVPictureDialog::OnBnClickedZoomPlus()
{
	m_PictDisplay.SetZoom(ZoomPlus);
	SetZoomSlider(TRUE);
	SetScrollBars();
	CloneScrollPosition(SB_BOTH);
	Invalidate();
}

void SVPictureDialog::SetZoomSlider(BOOL bRange)
{
	if(m_bZoomSlider)
	{
		if(bRange)
		{
			double zoomMin, zoomMax;
			m_PictDisplay.GetZoomRanges(zoomMin, zoomMax);

			int max = int(SliderProp * zoomMax);
			int min = int(SliderProp * zoomMin);
			m_SliderCtrlZoom.SetRangeMin(min);
			m_SliderCtrlZoom.SetRangeMax(max);

			if( m_eAppearance == LeftButtons || m_eAppearance == RightButtons )
			{
				//max should be at top
				//multiply by -1 to invert orientation of slidercontrol
				m_SliderCtrlZoom.SetRangeMax(-min);
				m_SliderCtrlZoom.SetRangeMin(-max);
			}
			else
			{
				m_SliderCtrlZoom.SetRangeMin(min);
				m_SliderCtrlZoom.SetRangeMax(max);
			}

			//Calculate the change in zoomfactor when clicking on zoomslider dependent on the Zoomfactor
			int pagesize = static_cast<int>(SliderProp * GetZoomHelper().GetZoomStep());
			m_SliderCtrlZoom.SetPageSize(pagesize);
		}

		int nZoom( static_cast< int >( GetZoomFactor() * float( SliderProp ) ) );
		CString csText;
		csText.Format(_T(" %i"), nZoom);
		if(csText.GetLength() < m_StaticZoomMaxTextLen)
		{
			csText.Append(_T("%"));
		}

		m_StaticZoom.SetWindowText(csText);
		if( m_eAppearance == LeftButtons || m_eAppearance == RightButtons )
		{
			//multiply by -1 to invert orientation of slidercontrol
			nZoom *= -1;
		}

		m_SliderCtrlZoom.SetPos(nZoom);
	}
}

void SVPictureDialog::ZoomToSliderPos()
{
	if(m_bZoomSlider)
	{
		int nZoom = m_SliderCtrlZoom.GetPos();
		if( m_eAppearance == LeftButtons || m_eAppearance == RightButtons )
		{
			//multiply by -1 to invert orientation of slidercontrol
			nZoom *= -1;
		}

		CString csText;
		csText.Format(_T(" %i"), nZoom);

		if(csText.GetLength() < m_StaticZoomMaxTextLen)
		{
			csText.Append(_T("%"));
		}

		m_StaticZoom.SetWindowText(csText);
		float scaleFactor = ( nZoom )/float(SliderProp);
		m_PictDisplay.SetZoomFactor(scaleFactor);
	}
}

// @TODO:  Change to avoid using raw pointers.
SVPictureTabs* SVPictureDialog::GetPictureTab()
{
	SVPictureTabs* ret = nullptr;
	CWnd* pParent1 = GetParent(); //Tabcontrol
	CWnd* pParent2 = pParent1->GetParent();
	ret = dynamic_cast<SVPictureTabs*>(pParent2);
	return ret;
}

const ZoomHelper& SVPictureDialog::GetZoomHelper()
{
	return m_PictDisplay.GetZoomHelper();
}

void SVPictureDialog::CloneZoomHelper(const ZoomHelper& zh)
{
	return m_PictDisplay.CloneZoomHelper(zh);
}

void SVPictureDialog::OnBnClickedZoomMinus()
{
	if( m_PictDisplay.HasBitmap() )
	{
		m_PictDisplay.SetZoom(ZoomMinus);
		SetScrollBars();
		SetZoomSlider(TRUE);
		CloneScrollPosition(SB_BOTH);
		Invalidate();
	}
}

void SVPictureDialog::OnBnClickedZoomFit()
{
	if( m_PictDisplay.HasBitmap() )
	{
		m_PictDisplay.SetZoom(ZoomToFit);
		SetScrollBars();
		SetZoomSlider(TRUE);
		CloneScrollPosition(SB_BOTH);
		Invalidate();
	}
}

void SVPictureDialog::OnBnClickedZoomOne()
{
	if( m_PictDisplay.HasBitmap() )
	{
		m_PictDisplay.SetZoom(ZoomOneToOne);
		SetScrollBars();
		SetZoomSlider(TRUE);
		CloneScrollPosition(SB_BOTH);
		Invalidate();
	}
}

HRESULT SVPictureDialog::SetPicture( IPictureDisp* p_Picture, COLORREF BackgroundColor, bool adjustZoom )
{
	HRESULT hr = m_PictDisplay.SetPicture( p_Picture, BackgroundColor, adjustZoom );
	AdjustSizeToImage();
	SetScrollBars();
	SetZoomSlider(TRUE);
	CloneScrollPosition(SB_BOTH);
	return hr;
}

HRESULT SVPictureDialog::SetPictureWithROI( IPictureDisp* p_Picture, COLORREF BackgroundColor, ISVROIList* p_RoiList, bool adjustZoom )
{
	HRESULT hr = m_PictDisplay.SetPictureWithROI( p_Picture, BackgroundColor, p_RoiList, adjustZoom );
	AdjustSizeToImage();
	SetScrollBars();
	SetZoomSlider(TRUE);
	CloneScrollPosition(SB_BOTH);
	return hr;
}

void SVPictureDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	SetupWindow();
}

void SVPictureDialog::Resize(unsigned int nType, long x, long y, long Width, long Height)
{
	if (::IsWindow(m_hWnd))
	{
		//It is important to use MoveWindow and not SetWindowPos here because SetWindowPos changes the window order.
		MoveWindow( x, y, Width, Height);
		CRect rect;
		CDialog::GetClientRect( &rect );
		OnSize(nType, rect.Width(), rect.Height() );
		m_PictDisplay.CalculateZoomToFit();
		SetScrollBars();
		CloneScrollPosition(SB_BOTH);
	}
}

void SVPictureDialog::SetupWindow()
{
	if( IsWindow( m_hWnd ) )
	{
		switch( m_eAppearance )
		{
		case TopButtons:
			{
				PositionHorizontalSlider( 0, m_ButtonSize, 0, 0, IDC_HORIZ_SCROLL );
				PositionVerticalSlider( 0, m_ButtonSize, 0, 0, IDC_VERTICAL_SCROLL );
				break;
			}
		case RightButtons:
			{
				PositionHorizontalSlider( 0, 0, m_ButtonSize, 0, IDC_HORIZ_SCROLL );
				PositionVerticalSlider( 0, 0, m_ButtonSize, 0, IDC_VERTICAL_SCROLL );
				break;
			}
		case LeftButtons:
			{
				PositionHorizontalSlider( m_ButtonSize, 0, 0, 0, IDC_HORIZ_SCROLL );
				PositionVerticalSlider( m_ButtonSize, 0, 0, 0, IDC_VERTICAL_SCROLL );
				break;
			}
		case BottomButtons:
			{
				PositionHorizontalSlider( 0, 0, 0, m_ButtonSize, IDC_HORIZ_SCROLL );
				PositionVerticalSlider( 0, 0, 0, m_ButtonSize, IDC_VERTICAL_SCROLL );
				break;
			}
		case HideButtons:
			{
				PositionHorizontalSlider( 0, 0, 0, 0, IDC_HORIZ_SCROLL );
				PositionVerticalSlider( 0, 0, 0, 0, IDC_VERTICAL_SCROLL );
				break;
			}
		default:
			{
				break; // Do nothing.
			}
		}
		PositionPictureDisplay();
		PositionButtons();
	}
}

void SVPictureDialog::SetAppearance( long l_lType )
{
	m_eAppearance = static_cast<AppearanceTypes>(l_lType);
	SetupWindow();
}

long SVPictureDialog::GetAppearance() const
{
	return m_eAppearance;
}

void SVPictureDialog::SetButtonSpacing( long p_lSpacing )
{
	m_lButtonSpacing = p_lSpacing;
	SetupWindow();
}

long SVPictureDialog::GetButtonSpacing() const
{
	return m_lButtonSpacing;
}

void SVPictureDialog::SetButtonJustify( long p_lJustify )
{
	m_lButtonJustify = p_lJustify;
	SetupWindow();
}

long SVPictureDialog::GetButtonJustify() const
{
	return m_lButtonJustify;
}

void SVPictureDialog::SetIsClearOverlayByNewImage( BOOL isValue )
{
	m_PictDisplay.SetIsClearOverlayByNewImage( isValue );
}

long SVPictureDialog::AddOverlay(const VariantParamMap& ParameterMap, bool refresh)
{
	return m_PictDisplay.AddOverlay( ParameterMap, refresh );
}

long SVPictureDialog::AddOverlayToGroup( long lGroup, const VariantParamMap& ParameterMap, bool refresh)
{
	return m_PictDisplay.AddOverlayToGroup( lGroup, ParameterMap, refresh );
}

HRESULT SVPictureDialog::EditOverlay(long lHandle, const VariantParamMap& ParameterMap )
{
	return m_PictDisplay.EditOverlay( lHandle, ParameterMap );
}

HRESULT SVPictureDialog::RemoveOverlay( long Handle )
{
	return m_PictDisplay.RemoveOverlay( Handle );
}

void SVPictureDialog::RemoveAllOverlays()
{
	m_PictDisplay.RemoveAllOverlays();
}

long SVPictureDialog::GetScrollBarWidth() const
{
	return m_lScrollBarWidth;
}

void SVPictureDialog::SetScrollBarWidth( long p_lWidth )
{
	m_lScrollBarWidth = p_lWidth;
	SetupWindow();
}

double SVPictureDialog::GetZoomFactor() const
{
	return m_PictDisplay.GetZoomFactor();
}

void SVPictureDialog::SetZoomFactor(double scaleFactor)
{
	m_PictDisplay.SetZoomFactor(scaleFactor);
	SetScrollBars();
	CloneScrollPosition(SB_BOTH);
}

HRESULT SVPictureDialog::SetAllowEdit( long lHandle, long lEnable )
{
	return m_PictDisplay.SetAllowEdit( lHandle, lEnable );
}

HRESULT SVPictureDialog::SelectDrawObject( long lHandle )
{
	return m_PictDisplay.SelectDrawObject( lHandle );
}

HRESULT SVPictureDialog::GetObjectCoords( long lHandle, long& rlX1, long& rlY1, long& rlX2, long& rlY2 )
{
	return m_PictDisplay.GetObjectCoords( lHandle, rlX1, rlY1, rlX2, rlY2 );
}

HRESULT SVPictureDialog::GetObjectParameter( long lHandle, VariantParamMap& ParameterMap )
{
	return m_PictDisplay.GetObjectParameter( lHandle, ParameterMap );
}

long SVPictureDialog::DeleteDrawObject()
{
	return m_PictDisplay.DeleteDrawObject();
}

HRESULT SVPictureDialog::EnableSaveButton( long p_lEnable )
{
	m_bSaveButton = p_lEnable > 0 ? true : false;
	PositionButtons();
	return S_OK;
}

void SVPictureDialog::OnBnClickedSaveButton()
{
	SVPictureTabs* l_pTabs = static_cast< SVPictureTabs* >( GetParent() );
	if( l_pTabs != NULL )
	{
		long lTab = 0;
		l_pTabs->GetParent()->GetParent()->SendMessage( SAVE_BUTTON_PRESSED, static_cast<WPARAM>( lTab ), reinterpret_cast<LPARAM>( m_PictDisplay.GetPicture() ) );
	}
}

HRESULT SVPictureDialog::GetViewRect( long& pX1, long& pY1, long& pX2, long& pY2 ) const
{
	return m_PictDisplay.GetViewRect( pX1, pY1, pX2, pY2 );
}

void SVPictureDialog::GetZoomRanges(double &min, double &max) const
{
	return m_PictDisplay.GetZoomRanges(min, max);
}

void SVPictureDialog::SetZoomFit()
{
	m_PictDisplay.SetZoom(ZoomToFit);
	SetScrollBars();
	CloneScrollPosition(SB_BOTH);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVOBSERVER_SRC\SVPICTUREDISPLAY\SVPictureDialog.cpp_v  $
 * 
 *    Rev 1.1   14 Aug 2014 17:35:52   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   new Parameter adjustZoom in SetPicture
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Jun 2014 16:28:02   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   23 May 2011 10:53:42   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  141
 * SCR Title:  Fix External OCR Adjustment Page Memory Usage When Moving ROI
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use LR_SHARED for loading buton icons
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 May 2011 08:28:40   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  141
 * SCR Title:  Fix External OCR Adjustment Page Memory Usage When Moving ROI
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to only set icons on buttons once.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 May 2011 14:45:28   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  141
 * SCR Title:  Fix External OCR Adjustment Page Memory Usage When Moving ROI
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct GDI resource leak
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Sep 2010 11:19:44   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  33
 * SCR Title:  Font Wizard Improvements
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetViewRect function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 May 2010 11:31:50   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed default display to "Fit Width"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Mar 2010 09:58:16   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetPicture method to return HRESULT
 * Revised SetPictureWithROI method to return HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Mar 2010 10:55:36   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Objected Selected Event
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:20   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/