//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageEditor
//* .File Name       : $Workfile:   SVDlgImageEditor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 12:48:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDlgImageEditor.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVImageLibrary/SVExtentFigureStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVDlgImageEditor, SvOg::SVDlgImageClass)
	//{{AFX_MSG_MAP(SVDlgImageEditor)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVDlgImageEditor::SVDlgImageEditor()
{
	m_pFigureEditor = nullptr;
	m_hDefaultPen = ::CreatePen( PS_DOT, 1, RGB(0,255,0) );
}

SVDlgImageEditor::~SVDlgImageEditor()
{
	if( m_hDefaultPen )
		DeleteObject( m_hDefaultPen );
}


HRESULT SVDlgImageEditor::SetFigureEditor( SVFigureEditor* pFigureEditor )
{
	m_pFigureEditor = pFigureEditor;
	refresh();
	return S_OK;
}

LRESULT SVDlgImageEditor::OnNcHitTest( CPoint point )
{
	return HTCLIENT;	// must do this to get mouse input
}

CPoint SVDlgImageEditor::TranslateClientCoordinatesToImageCoordinates(CPoint point)
{
	CRect rectClient = m_ClientRect;
	CSize sizeFigure;
	if ( m_pFigureEditor )
		sizeFigure =  m_pFigureEditor->GetSize();
	else
	{
		CRect rect = GetOutputRectFromImage();
		sizeFigure.cx = rect.Width();
		sizeFigure.cy = rect.Height();
	}

	double dScaleX = m_dZoomX * (double) rectClient.Width() / (double) sizeFigure.cx;
	double dScaleY = m_dZoomY * (double) rectClient.Height() / (double) sizeFigure.cy;

	CPoint ptImage( static_cast<int>(point.x / dScaleX), static_cast<int>(point.y / dScaleY) );
	ptImage += m_ViewportRect.TopLeft();

	return ptImage;
}

void SVDlgImageEditor::OnMouseMove(UINT nFlags, CPoint point) 
{
	//nFlags can have the following values:
	//MK_CONTROL   Set if the CTRL key is down. 
	//MK_LBUTTON   Set if the left mouse button is down. 
	//MK_MBUTTON   Set if the middle mouse button is down. 
	//MK_RBUTTON   Set if the right mouse button is down. 
	//MK_SHIFT   Set if the SHIFT key is down. 

	if ( (nullptr != m_hWnd ) && (nullptr != m_pFigureEditor ) )
	{
		bool bResetMouse = false;

		CPoint ptMouse = point;
		CRect l_ClientRect;
		GetClientRect(&l_ClientRect);

		if ( m_ClientRect.PtInRect( point ) )
		{
			ClientToScreen(&ptMouse);

			CPoint ptImage = TranslateClientCoordinatesToImageCoordinates(point);
			m_ptLastMousePosition = ptImage;

			if ( !m_pFigureEditor->IsCaptured() )
			{
				m_uiHitTest = m_pFigureEditor->OnHitTest( ptImage );
				OnSetCursor(this, m_uiHitTest, WM_MOUSEMOVE );
			}

			m_pFigureEditor->OnMouseMove(nFlags, ptImage);
		}
		else
		{
			if ( point.y > l_ClientRect.bottom - m_iScrollBarSize )
				m_uiHitTest = HTHSCROLL;
			else
			if ( point.x > l_ClientRect.right - m_iScrollBarSize )
				m_uiHitTest = HTVSCROLL;
			else
				m_uiHitTest = HTNOWHERE;

			OnSetCursor(this, m_uiHitTest, WM_MOUSEMOVE );
		}
	}// end if (m_hWnd)


	SvOg::SVDlgImageClass::OnMouseMove(nFlags, point);
	
	if ( m_ClientRect.PtInRect( point ) )
	{
		if ( GetOwner() )
			GetOwner()->PostMessage(WM_MOUSEMOVE, nFlags, 0 );	// REFLECT TO parent/owner
	}
}

CPoint SVDlgImageEditor::GetMouseCoordinates()	// in image coordinates, not screen
{
	return m_ptLastMousePosition;
}


BOOL SVDlgImageEditor::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	m_szCursorId = GetPickCursor( m_uiHitTest );
	
	HCURSOR hCursor = ::LoadCursor(nullptr, m_szCursorId);
	::SetCursor(hCursor);

	return TRUE;
}


void SVDlgImageEditor::OnKillFocus(CWnd* pNewWnd) 
{
	SvOg::SVDlgImageClass::OnKillFocus(pNewWnd);

	if ( m_pFigureEditor )
		m_pFigureEditor->OnLButtonUp( 0, CPoint(0,0) );
	
}

void SVDlgImageEditor::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_uiHitTest != HTCLIENT && nullptr != m_pFigureEditor )
	{
		SetCapture();
		CPoint ptImage = TranslateClientCoordinatesToImageCoordinates(point);
		//m_ptPickPoint = ptImage;
		m_pFigureEditor->OnLButtonDown( nFlags, ptImage );
	}
	else
	{
		SvOg::SVDlgImageClass::OnLButtonDown(nFlags, point);
	}
}

void SVDlgImageEditor::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( GetCapture() == this )
	{
		ReleaseCapture();
	}

	if ( m_pFigureEditor )
	{
		CPoint ptImage = TranslateClientCoordinatesToImageCoordinates(point);
		m_pFigureEditor->OnLButtonUp( nFlags, ptImage );
	}
	
	SvOg::SVDlgImageClass::OnLButtonUp(nFlags, point);
}

LPCTSTR SVDlgImageEditor::GetPickCursor( UINT nHitTest )
{
	LPCTSTR szNewCursorId = IDC_ARROW;
	
	switch ( nHitTest )
	{
		case HTOBJECT:	// Move
		{
			szNewCursorId = IDC_SIZEALL;
			break;
		}
		case HTBOTTOMRIGHT:
		{
			szNewCursorId = IDC_SIZENWSE;
			break;
		}
		case HTBOTTOMLEFT:
		{
			szNewCursorId = IDC_SIZENESW;
			break;
		}
		case HTTOPRIGHT:
		{
			szNewCursorId = IDC_SIZENESW;
			break;
		}
		case HTTOPLEFT:
		{
			szNewCursorId = IDC_SIZENWSE;
			break;
		}
		case HTBOTTOM:
		{
			szNewCursorId = IDC_SIZENS;
			break;
		}
		case HTRIGHT:
		{
			szNewCursorId = IDC_SIZEWE;
			break;
		}
		case HTLEFT:
		{
			szNewCursorId = IDC_SIZEWE;
			break;
		}
		case HTTOP:
		{
			szNewCursorId = IDC_SIZENS;
			break;
		}
		default:
		{
			szNewCursorId = IDC_ARROW;
			break;
		}
	}

	if ( m_szCursorId != szNewCursorId )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( szNewCursorId ) );
	}
	return szNewCursorId;
}

void SVDlgImageEditor::OnPaintOverlay(CPaintDC& dc)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &m_ClientRect );
	dc.SelectClipRgn( &rgn, RGN_COPY );

	if ( m_pFigureEditor )
	{
		m_pFigureEditor->PaintOverlay(dc, m_ViewportRect, m_ClientRect);
	}

	DrawOverlay(dc);
}




HRESULT SVDlgImageEditor::ClearOverlayPoints()
{
	m_drawList.clear();
	return S_OK;
}


HRESULT SVDlgImageEditor::AddOverlayPoints( SVDrawObjectListClass* pDrawObjectList, SVDlgImageOverlayOptions options )
{
	HRESULT hr = S_OK;

	int size = pDrawObjectList->GetSize();
	
	for( int i = 0; i < size; i++ )
	{
		SVDrawObjectClass& rsvDrawObject = pDrawObjectList->GetAt(i);
		hr = AddOverlayPoints( rsvDrawObject, options );
	}

	return hr;
}

HRESULT SVDlgImageEditor::AddOverlayPoints( SVDrawObjectClass& rsvDrawObject, SVDlgImageOverlayOptions options )
{
	HRESULT hr = S_OK;

	OverlayStruct overlay;
	overlay.options = options;
	overlay.svDrawObject = rsvDrawObject;

	Normalize( overlay );

	m_drawList.push_back( overlay );

	return hr;
}

void SVDlgImageEditor::CopyOverlayPoints( const SVCPointArray& rsvaPoints, std::vector<CPoint>& rvecPoints )
{
	rvecPoints.resize( rsvaPoints.GetSize() );
	for ( size_t i=0; i < rvecPoints.size(); ++i )
	{
		rvecPoints[i] = rsvaPoints.GetAt(static_cast<int>(i));
	}

}

void SVDlgImageEditor::Normalize( OverlayStruct& rOverlay )
{
	const SVCPointArray& rPointArray = rOverlay.svDrawObject.GetPointArray();
	CopyOverlayPoints( rPointArray, rOverlay.vecPoints );

	CSize sizeFigure = rOverlay.options.sizeROI;

	int iFigureArea = sizeFigure.cx * sizeFigure.cy;
	
	// figure out bounds of data
	CRect rectBounds(99999999, 99999999, -99999999, -99999999);	// initialize to inverted rectangle
	size_t i;
	for ( i=0; i < rOverlay.vecPoints.size(); ++i )
	{
		CPoint& pt = rOverlay.vecPoints[i];
		rectBounds.top    = std::min(rectBounds.top,    pt.y );
		rectBounds.bottom = std::max(rectBounds.bottom, pt.y );
		rectBounds.left   = std::min(rectBounds.left,   pt.x );
		rectBounds.right  = std::max(rectBounds.right,  pt.x );
	}

	if ( rOverlay.options.bScaleX )
	{
		rectBounds.left  = rOverlay.options.rectMaxXY.left;
		rectBounds.right = rOverlay.options.rectMaxXY.right;
	}
	if ( rOverlay.options.bScaleY )
	{
		rectBounds.top    = rOverlay.options.rectMaxXY.top;
		rectBounds.bottom = rOverlay.options.rectMaxXY.bottom;
	}

	CSize sizeBounds = rectBounds.Size();

	CSize offset = rectBounds.TopLeft() - m_ClientRect.TopLeft();	// client rect origin should be (0,0) but just in case
	double dScaleX = (double) (m_ClientRect.Width()-4) / (double) rectBounds.Width();
	double dScaleY = (double) (m_ClientRect.Height()-4) / (double) rectBounds.Height();

	if ( rOverlay.options.bNormalizeY_ROI )
	{
		dScaleY = (double) m_ClientRect.Height() / pow((double) iFigureArea, 0.4);
	}

	for ( i=0; i < rOverlay.vecPoints.size(); ++i )
	{
		CPoint& rpt = rOverlay.vecPoints[i];
		rpt -= offset;
		rpt.x = (int) ((double) rpt.x * dScaleX) + 1;
		if ( rOverlay.options.bNormalizeY_ROI )
		{
			rpt.y = (int) (pow((double) rpt.y, 0.4) * dScaleY) + 1;
		}
		else
		{
			rpt.y = (int) ((double) rpt.y * dScaleY) + 1;
		}

		if ( rOverlay.options.bFlipVertical )
		{
			rpt.y = m_ClientRect.Height() - rpt.y;
		}
	}
}
									 
void SVDlgImageEditor::DrawOverlay(CPaintDC& dc)
{
	for( size_t i = 0; i < m_drawList.size(); i++)
	{
		OverlayStruct& rOverlay = m_drawList[i];
		std::vector<CPoint>& vecPoints = rOverlay.vecPoints;
		
		size_t size = vecPoints.size();
		
		if( size > 0 )
		{
			CPen* pPen = nullptr;
			
			HPEN hPen = (HPEN) ( rOverlay.svDrawObject.GetDrawPen() );
			
			if( !hPen )
			{
				// use default Pen
				pPen = CPen::FromHandle( m_hDefaultPen );
			}
			else
			{
				pPen = CPen::FromHandle( hPen );
			}
			
			CPen* pOldPen = dc.SelectObject( pPen );
			
			dc.Polyline( &(vecPoints[0]), static_cast<int>(size) );
			
			dc.SelectObject( pOldPen );
		}
	}
}

void SVDlgImageEditor::OnSetZoom()
{
	for( size_t i = 0; i < m_drawList.size(); i++)
	{
		OverlayStruct& rOverlay = m_drawList[i];
		Normalize( rOverlay );
	}
}

