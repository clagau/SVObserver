//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageGraphROI
//* .File Name       : $Workfile:   SVDlgImageGraphROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:17:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgImageGraphROI.h"
#include "SVValueObject.h"
#include "SVImageLibrary/SVExtentFigureStruct.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int RESIZE_REGION_SIZE = 3;

BEGIN_MESSAGE_MAP(SVDlgImageGraphROI, SVDlgImageGraphClass)
	//{{AFX_MSG_MAP(SVDlgImageGraphROI)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



LRESULT SVDlgImageGraphROI::OnNcHitTest( CPoint point )
{
	return HTCLIENT;	// must do this to get mouse input
}

HRESULT SVDlgImageGraphROI::SetROI(const CRect &p_roRect)
{
	HRESULT hr = S_OK;

	SVExtentFigureStruct l_svFigure = p_roRect;

	m_roi.ClearObject();
	m_roi.AddExtentFigureData( l_svFigure );
	
	AddPoints( m_roi, SVGraphScale );

	m_oRect = p_roRect;

	ScaleRealToDisplay( m_oRect );
	
	return hr;
}

HRESULT SVDlgImageGraphROI::GetROI(CRect &p_roRect)
{
	HRESULT hr = S_OK;
	
	p_roRect.left   = m_roi.GetPointAt( 0 ).x;
	p_roRect.top    = m_roi.GetPointAt( 0 ).y;
	p_roRect.right  = m_roi.GetPointAt( 2 ).x;
	p_roRect.bottom = m_roi.GetPointAt( 2 ).y;
	
	return hr;
}

void SVDlgImageGraphROI::UpdateToolFigure()
{
	// Update Tool Figure
	CRect l_oRect = m_oRect;

	ScaleDisplayToReal( l_oRect );

	SVExtentFigureStruct l_svFigure = l_oRect;

	m_roi.ClearObject();
	m_roi.AddExtentFigureData( l_svFigure );
	ClearPoints();
	AddPoints( m_roi, SVGraphScale );
}

void SVDlgImageGraphROI::DrawFigure()
{
	//RefreshPoints();
	refresh();
}


void SVDlgImageGraphROI::UpdateDisplay()
{
	// notify owner
	if ( GetOwner() )
	{
		GetOwner()->SendMessage(WM_MOUSEMOVE, 0, 0);
	}
}

void SVDlgImageGraphROI::ScaleDisplayToReal(CRect &p_roRect )
{
	long l_lTmpWidth = static_cast<long>(p_roRect.Width() / m_dxScale);
	long l_lTmpHeight = static_cast<long>(p_roRect.Height() / m_dyScale);
	p_roRect.top    = (long) (ceil((double)p_roRect.top    / m_dyScale));
	p_roRect.left   = (long) (ceil((double)p_roRect.left   / m_dxScale));
	p_roRect.right  = p_roRect.left + l_lTmpWidth ;
	p_roRect.bottom = p_roRect.top + l_lTmpHeight ;
}

void SVDlgImageGraphROI::ScaleRealToDisplay(CRect &p_roRect)
{
	long l_lTmpWidth = static_cast<long>(ceil( (double)p_roRect.Width() * m_dxScale));
	long l_lTmpHeight = static_cast<long>(ceil( (double)p_roRect.Height() * m_dyScale));
	p_roRect.top    = (long) ((double)p_roRect.top    * m_dyScale);
	p_roRect.left   = (long) ((double)p_roRect.left   * m_dxScale);
	p_roRect.right  = p_roRect.left + l_lTmpWidth ;
	p_roRect.bottom = p_roRect.top + l_lTmpHeight ;
}

void SVDlgImageGraphROI::OnMouseMove(UINT nFlags, CPoint point) 
{
	//nFlags can have the following values:
	//MK_CONTROL   Set if the CTRL key is down. 
	//MK_LBUTTON   Set if the left mouse button is down. 
	//MK_MBUTTON   Set if the middle mouse button is down. 
	//MK_RBUTTON   Set if the right mouse button is down. 
	//MK_SHIFT   Set if the SHIFT key is down. 

	if (m_hWnd)
	{
		bool bResetMouse = false;

		CPoint ptMouse = point;
		CRect rectDlgRect;
		CRect rectClientArea;

		GetClientRect(&rectClientArea);

		ClientToScreen(&ptMouse);

		// Use Image Rectangle
		CRect l_oRect = m_oRect;

		rectDlgRect = l_oRect;

		ClientToScreen(&rectDlgRect);

		if (rectDlgRect.PtInRect(ptMouse) || GetCapture() == this )
		{
			CRect rect;
			rect.top = point.y;
			rect.left = point.x;

			//MK_LBUTTON   Set if the left mouse button is down. 

			if ( GetCapture() == this )
			{
				if ( nFlags & MK_LBUTTON )
				{
					long deltaX = ( point.x - m_ptPickPoint.x );
					long deltaY = ( point.y - m_ptPickPoint.y );

					bool bUpdatePickPointX = true;
					bool bUpdatePickPointY = true;

					switch( m_ePickType )
					{
						case PickTypeMove:
						{
							l_oRect.left += deltaX;
							l_oRect.top += deltaY;
							l_oRect.right += deltaX;
							l_oRect.bottom += deltaY;
						}
						break;

						case PickTypeResizeRight:
						{ 
							l_oRect.right += deltaX;
						}
						break;

						case PickTypeResizeBottom:
						{ 
							l_oRect.bottom += deltaY;
						}
						break;

						case PickTypeResizeLeft:
						{ 
							l_oRect.left += deltaX;
						}
						break;

						case PickTypeResizeTop:
						{ 
							l_oRect.top += deltaY;
						}
						break;

						case PickTypeResizeBottomRight:
						{ 
							l_oRect.right += deltaX;
							l_oRect.bottom += deltaY;
						}
						break;

						case PickTypeResizeBottomLeft:
						{ 
							l_oRect.left += deltaX;
							l_oRect.bottom += deltaY;
						}
						break;

						case PickTypeResizeTopRight:
						{ 
							l_oRect.top += deltaY;
							l_oRect.right += deltaX;
						}
						break;

						case PickTypeResizeTopLeft:
						{ 
							l_oRect.top += deltaY;
							l_oRect.left += deltaX;
						}
						break;
					}// end switch( m_ePickType )

					// Prevent box from moving outside client area
					if ( l_oRect.bottom > rectClientArea.bottom )
					{
						l_oRect.top -= ( l_oRect.bottom - rectClientArea.bottom );

						bUpdatePickPointY = false;
					}

					if ( l_oRect.right > rectClientArea.right )
					{
						l_oRect.left -= ( l_oRect.right - rectClientArea.right );

						bUpdatePickPointX = false;
					}

					if ( l_oRect.top < 0 )
					{
						if ( l_oRect.bottom - l_oRect.top <= rectClientArea.bottom )
						{
							l_oRect.bottom -= l_oRect.top;
						}

						l_oRect.top = 0;

						bUpdatePickPointY = false;
					}

					if ( l_oRect.left < 0 )
					{
						if ( l_oRect.right - l_oRect.left <= rectClientArea.right )
						{
							l_oRect.right -= l_oRect.left;
						}

						l_oRect.left = 0;

						bUpdatePickPointX = false;
					}

					if ( l_oRect.bottom > rectClientArea.bottom )
					{
						l_oRect.bottom = rectClientArea.bottom;

						bUpdatePickPointY = false;
					}

					if ( l_oRect.right > rectClientArea.right )
					{
						l_oRect.right = rectClientArea.right;

						bUpdatePickPointX = false;
					}

					m_oRect = l_oRect;

					UpdateToolFigure();
					DrawFigure();
					UpdateDisplay();

					if ( bUpdatePickPointX )
						m_ptPickPoint.x = point.x;
					if ( bUpdatePickPointY )
						m_ptPickPoint.y = point.y;

				}// end if ( nFlags & MK_LBUTTON )
			}// end if ( GetCapture() == this )
			else
			{
				if ( UpdatePickType( point ) )
				{
					m_szCursorId = GetPickCursor();
				}
			}
		}// end if (rectDlgRect.PtInRect(ptMouse))
		else
		{
			if ( UpdatePickType( point ) )
			{
				m_szCursorId = GetPickCursor();
			}

			if ( GetCapture() == this )
			{
				bResetMouse = true;
			}
		}

		if ( bResetMouse )
		{
			point = m_ptPickPoint;

			CPoint cpTemp = point;

			ClientToScreen( &cpTemp );
		}
	}// end if (m_hWnd)

	SVDlgImageGraphClass::OnMouseMove(nFlags, point);
}

BOOL SVDlgImageGraphROI::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	m_szCursorId = GetPickCursor();
	
	HCURSOR hCursor = ::LoadCursor(NULL, m_szCursorId);
	::SetCursor(hCursor);

	//return SVDlgImageGraphClass::OnSetCursor(pWnd, nHitTest, message);

	return TRUE;
}

void SVDlgImageGraphROI::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_ePickType != PickTypeUnknown )
	{
		m_ptPickPoint = point;

		//MapWindowPoints(this, &m_ptPickPoint, 1);

		SetCapture();
	}
	
	SVDlgImageGraphClass::OnLButtonDown(nFlags, point);
}

void SVDlgImageGraphROI::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( GetCapture() == this )
	{
		ReleaseCapture();
	}
	
	SVDlgImageGraphClass::OnLButtonUp(nFlags, point);
}

BOOL SVDlgImageGraphROI::UpdatePickType( CPoint point )
{
	BOOL bOk = TRUE;
	// RB
	if ( IsBottomRightCornerHit( point ) )
	{
		m_ePickType = PickTypeResizeBottomRight;
	}
	// LB
	else if ( IsBottomLeftCornerHit( point ) )
	{
		m_ePickType = PickTypeResizeBottomLeft;
	}
	// RT
	else if ( IsTopRightCornerHit( point ) )
	{
		m_ePickType = PickTypeResizeTopRight;
	}
	// LT
	else if ( IsTopLeftCornerHit( point ) )
	{
		m_ePickType = PickTypeResizeTopLeft;
	}
	// B
	else if ( IsBottomEdgeHit( point ) )
	{
		m_ePickType = PickTypeResizeBottom;
	}
	// R
	else if ( IsRightEdgeHit( point ) )
	{
		m_ePickType = PickTypeResizeRight;
	}
	// L
	else if ( IsLeftEdgeHit( point ) )
	{
		m_ePickType = PickTypeResizeLeft;
	}
	// T
	else if ( IsTopEdgeHit( point ) )
	{
		m_ePickType = PickTypeResizeTop;
	}
	// Move
	else if( IsCenterHit( point ) )
	{
		m_ePickType = PickTypeMove;
	}
	else 	
	{
		m_ePickType = PickTypeUnknown;
		bOk = FALSE;
	}

	return bOk;
}

LPCTSTR SVDlgImageGraphROI::GetPickCursor()
{
	LPCTSTR szNewCursorId = IDC_ARROW;

	switch ( m_ePickType )
	{
		case PickTypeMove:
		{
			szNewCursorId = IDC_SIZEALL;
			break;
		}
		case PickTypeResizeBottomRight:
		{
			szNewCursorId = IDC_SIZENWSE;
			break;
		}
		case PickTypeResizeBottomLeft:
		{
			szNewCursorId = IDC_SIZENESW;
			break;
		}
		case PickTypeResizeTopRight:
		{
			szNewCursorId = IDC_SIZENESW;
			break;
		}
		case PickTypeResizeTopLeft:
		{
			szNewCursorId = IDC_SIZENWSE;
			break;
		}
		case PickTypeResizeBottom:
		{
			szNewCursorId = IDC_SIZENS;
			break;
		}
		case PickTypeResizeRight:
		{
			szNewCursorId = IDC_SIZEWE;
			break;
		}
		case PickTypeResizeLeft:
		{
			szNewCursorId = IDC_SIZEWE;
			break;
		}
		case PickTypeResizeTop:
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


bool SVDlgImageGraphROI::IsCenterHit( CPoint point ) 
{
	CRect rect = m_oRect;

	rect.DeflateRect( RESIZE_REGION_SIZE, RESIZE_REGION_SIZE );

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsLeftEdgeHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.left -= RESIZE_REGION_SIZE;
	rect.right = rect.left + RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsRightEdgeHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.right += RESIZE_REGION_SIZE;
	rect.left = rect.right - RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsTopEdgeHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.top -= RESIZE_REGION_SIZE;
	rect.bottom = rect.top + RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsBottomEdgeHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.bottom += RESIZE_REGION_SIZE;
	rect.top = rect.bottom - RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsTopLeftCornerHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.left -= RESIZE_REGION_SIZE;
	rect.right = rect.left + RESIZE_REGION_SIZE * 2;
	rect.top -= RESIZE_REGION_SIZE;
	rect.bottom = rect.top + RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsTopRightCornerHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.right += RESIZE_REGION_SIZE;
	rect.left = rect.right - RESIZE_REGION_SIZE * 2;
	rect.top -= RESIZE_REGION_SIZE;
	rect.bottom = rect.top + RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsBottomLeftCornerHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.left -= RESIZE_REGION_SIZE;
	rect.right = rect.left + RESIZE_REGION_SIZE * 2;
	rect.bottom += RESIZE_REGION_SIZE;
	rect.top = rect.bottom - RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}

bool SVDlgImageGraphROI::IsBottomRightCornerHit( CPoint point ) 
{
	CRect rect = m_oRect;
	
	rect.right += RESIZE_REGION_SIZE;
	rect.left = rect.right - RESIZE_REGION_SIZE * 2;
	rect.bottom += RESIZE_REGION_SIZE;
	rect.top = rect.bottom - RESIZE_REGION_SIZE * 2;

	return rect.PtInRect( point ) != FALSE;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImageGraphROI.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:17:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 09:01:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Jun 2007 10:38:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 12:27:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Oct 2005 08:35:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified  Scale RealToDisplay and ScaleDisplayToReal so width will not change as a result of ROI Position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 13:49:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Feb 2004 17:30:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed compiler warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Aug 2003 13:45:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   prevent box from resizing when attempt to move out of bounds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Aug 2003 08:49:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
