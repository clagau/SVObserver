//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageEditor
//* .File Name       : $Workfile:   SVDlgImageEditor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:15:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgImageEditor.h"
#include "SVValueObject.h"
#include "SVImageLibrary/SVExtentFigureStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVDlgImageEditor, SVDlgImageClass)
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
	m_pFigureEditor = NULL;
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
	//GetClientRect(&rectClient);
	CSize sizeFigure;
	if ( m_pFigureEditor )
		sizeFigure =  m_pFigureEditor->GetSize();
	else
	{
		CRect rect;
		mSourceImageInfo.GetOutputRectangle(rect);
		sizeFigure.cx = rect.Width();
		sizeFigure.cy = rect.Height();
	}

	//CPoint ptViewOffset(0,0);

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

	if ( (m_hWnd != NULL) && (m_pFigureEditor != NULL) )
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


	SVDlgImageClass::OnMouseMove(nFlags, point);

	//CRect rectClient;
	//GetClientRect(&rectClient);

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
	//TRACE(_T("HitTest: %d\n"), nHitTest);
	//m_szCursorId = GetPickCursor( nHitTest );
	m_szCursorId = GetPickCursor( m_uiHitTest );
	
	HCURSOR hCursor = ::LoadCursor(NULL, m_szCursorId);
	::SetCursor(hCursor);

	return TRUE;
}


void SVDlgImageEditor::OnKillFocus(CWnd* pNewWnd) 
{
	SVDlgImageClass::OnKillFocus(pNewWnd);

	if ( m_pFigureEditor )
		m_pFigureEditor->OnLButtonUp( 0, CPoint(0,0) );
	
}

void SVDlgImageEditor::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_uiHitTest != HTCLIENT && m_pFigureEditor != NULL )
	{
		SetCapture();
		CPoint ptImage = TranslateClientCoordinatesToImageCoordinates(point);
		//m_ptPickPoint = ptImage;
		m_pFigureEditor->OnLButtonDown( nFlags, ptImage );
	}
	else
	{
		SVDlgImageClass::OnLButtonDown(nFlags, point);
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
	
	SVDlgImageClass::OnLButtonUp(nFlags, point);
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
		rvecPoints[i] = rsvaPoints.GetAt(i);
	}

}

void SVDlgImageEditor::Normalize( OverlayStruct& rOverlay )
{
	const SVCPointArray& rPointArray = rOverlay.svDrawObject.GetPointArray();
	CopyOverlayPoints( rPointArray, rOverlay.vecPoints );

	CSize sizeFigure;
	/*
	if ( m_pFigureEditor )
		sizeFigure = m_pFigureEditor->GetSize();
	else
	*/
		sizeFigure = rOverlay.options.sizeROI;

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
			rpt.y = (int) (pow((double) rpt.y, 0.4) * dScaleY) + 1;
		else
			rpt.y = (int) ((double) rpt.y * dScaleY) + 1;

		if ( rOverlay.options.bFlipVertical )
			rpt.y = m_ClientRect.Height() - rpt.y;
	}
}
									 
void SVDlgImageEditor::DrawOverlay(CPaintDC& dc)
{
	for( size_t i = 0; i < m_drawList.size(); i++)
	{
		OverlayStruct& rOverlay = m_drawList[i];
		std::vector<CPoint>& vecPoints = rOverlay.vecPoints;
		
		long size = vecPoints.size();
		
		if( size > 0 )
		{
			CPen* pPen = NULL;
			
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
			
			dc.Polyline( &(vecPoints[0]), size );
			
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImageEditor.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:15:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Mar 2011 15:08:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Dec 2009 09:01:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Jun 2007 10:38:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2007 12:27:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Aug 2006 15:48:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Zoom / overlay bug
 * changed histogram scaling
 * added ROI feedback on threshold adjust pages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2006 16:03:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:56:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
