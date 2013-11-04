//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROIFigureEditor
//* .File Name       : $Workfile:   SVROIFigureEditor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:01:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVROIFigureEditor.h"
#include "SVMaskShape.h"	// reuse existing shape (rectangle) drawing functions
#include "SVImageLibrary/SVGeometryClass.h"

#pragma warning ( disable : 4065 )

// MUCH OF THIS CODE WAS COPIED FROM SVMASKSHAPE.CPP
// There was not enough time to properly factor out common functionality
// EB 2006 08 11, for Dione release (4.77)
// "I would not have made this so long except that I do not have the
//  time to make it shorter" - Blaise Pascal, 1656

// from SVMaskShapeRectangle::RenderOutline
HRESULT SVROIShape::DrawOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgbOuter, COLORREF rgbInner /* = -1 */ )
{
	HRESULT hr = S_OK;

	CRect rect( GetRect() );

	TranslateToDisplay(rectViewport, rectDisplay, rect);

	if ( hr == S_OK )
	{
		CBrush brush;
		brush.CreateSolidBrush( rgbOuter );
		CBrush* pOldBrush = dc.SelectObject( &brush );

		dc.FrameRect(&rect, &brush);

		if ( rgbInner != static_cast<COLORREF> (-1) )
		{
			rect.DeflateRect(1,1);
			CBrush brush1;
			brush1.CreateSolidBrush( rgbInner );
			dc.SelectObject( &brush1 );
			dc.FrameRect( &rect, &brush1 );
		}

		dc.SelectObject( pOldBrush );
	}

	return hr;
}

CRect SVROIShape::GetRect()
{
	return m_Rect;
}

void  SVROIShape::SetRect( const CRect& rect )
{
	m_Rect = rect;
}


/*
HRESULT SVROIShape::Refresh()
{
	// take the properties and draw them on the internal bitmap
	HRESULT hr = S_OK;

	if ( m_bImageInfoChanged && m_dib.hbm != NULL )
	{
		::DeleteObject( m_dib.hbm );
		m_dib.FreeBitmapInfo();
		m_dib.Clear();
		m_RenderDC.DeleteDC();
	}

	if ( m_dib.hbm == NULL )
	{
		m_RenderDC.CreateCompatibleDC(NULL);
		hr = SVIHBitmapUtilities::SVImageInfoToNewDIB( m_svImageInfo, m_dib );
		m_bImageInfoChanged = false;
	}

	if ( hr == S_OK )
	{
		HBITMAP hbmOld = (HBITMAP) ::SelectObject(m_RenderDC.GetSafeHdc(), m_dib.hbm);

		// Render Shape
		hr = RenderOutline( m_RenderDC, , , rgb );

		::SelectObject(m_RenderDC.GetSafeHdc(), hbmOld);
	}

	return hr;
}// SVMaskShape::Refresh
*/

CRect SVROIShape::GetParentImageRect()
{
	CRect rect;
	m_svImageInfo.GetExtents().GetOutputRectangle(rect);
	return rect;
}

HRESULT SVROIShape::SetImageInfo( const SVImageInfoClass& svImageInfo )
{
	m_bImageInfoChanged = true;
	m_svImageInfo = svImageInfo;
	return S_OK;
}

HRESULT SVROIShape::GetImageInfo( SVImageInfoClass& svImageInfo ) const
{
	svImageInfo = m_svImageInfo;
	return S_OK;
}


HRESULT SVROIShape::TranslateToDisplay( CRect rectViewport, CRect rectDisplay, CRect& rectShape )
{
	HRESULT hr = S_OK;

	SVExtentFigureStruct figure(rectShape);

	CRect rectImage = GetParentImageRect();
	
	SVExtentFigureStruct figureSource(rectImage);  // mask image is the input
	SVExtentFigureStruct figureDest(rectViewport); // viewport is the output

	// translate mask image to viewport
	figure -= (figureDest.m_svTopLeft - figureSource.m_svTopLeft); // simple offset, no scaling

	if ( hr == S_OK )
	{
		figureSource = figureDest;	// viewport is now the input
		figureDest = rectDisplay;   // display is the output
		// translate viewport to display
		hr = TranslateCoordinates(figureSource, figureDest, figure);
		if ( hr == S_OK )
		{
			rectShape = figure.Rect();
		}
	}

	return hr;
}

HRESULT SVROIShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rRect)
{
	SVExtentPointStruct ptTopLeft = rRect.m_svTopLeft;
	SVExtentPointStruct ptBottomRight = rRect.m_svBottomRight;
	TranslateCoordinates(rectSource, rectDest, ptTopLeft);
	TranslateCoordinates(rectSource, rectDest, ptBottomRight);
	rRect.SetRect( ptTopLeft.m_dPositionY, ptTopLeft.m_dPositionX, ptBottomRight.m_dPositionY, ptBottomRight.m_dPositionX);
	return S_OK;
}

HRESULT SVROIShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentPointStruct& rPoint)
{
	double dScaleX = rectDest.Size().m_dCX / rectSource.Size().m_dCX;
	double dScaleY = rectDest.Size().m_dCY / rectSource.Size().m_dCY;

	rPoint -= rectDest.m_svTopLeft;
	
	rPoint.m_dPositionX *= dScaleX;
	rPoint.m_dPositionY *= dScaleY;
	return S_OK;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







SVROIFigureEditor::SVROIFigureEditor()
{
	m_pShape = new SVROIShape;
}

SVROIFigureEditor::~SVROIFigureEditor()
{
	delete m_pShape;
	m_pShape = NULL;
}

HRESULT SVROIFigureEditor::PaintOverlay( CDC& dc, CRect rectViewport, CRect rectDisplay )
{
	CBrush brush;
	brush.CreateSolidBrush(m_rgbOuterOutlineColor);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_rgbOuterOutlineColor);
	CPen* pOldPen = dc.SelectObject(&pen);

	HRESULT hr = S_OK;
	if ( m_bUseInnerOutline )
		hr = m_pShape->DrawOutline(dc, rectViewport, rectDisplay, m_rgbOuterOutlineColor, m_rgbInnerOutlineColor );
	else
		hr = m_pShape->DrawOutline(dc, rectViewport, rectDisplay, m_rgbOuterOutlineColor );

	dc.SelectObject( pOldPen );
	dc.SelectObject( pOldBrush );

	return hr;
}

void SVROIFigureEditor::SetRect( const CRect& rect )
{
	m_pShape->SetRect( rect );
}

CRect SVROIFigureEditor::GetRect()
{
	return m_pShape->GetRect();
}

/*
HRESULT SVROIFigureEditor::Refresh()
{
	return m_pShape->Refresh();
}
*/


HRESULT SVROIFigureEditor::SetImageInfo( const SVImageInfoClass& svImageInfo )
{
	return m_pShape->SetImageInfo( svImageInfo );
}

HRESULT SVROIFigureEditor::GetImageInfo( SVImageInfoClass& svImageInfo ) const
{
	return m_pShape->GetImageInfo( svImageInfo );
}

CRect SVROIFigureEditor::GetParentImageRect()
{
	return m_pShape->GetParentImageRect();
}


CSize SVROIFigureEditor::GetSizeImpl()
{
	long lWidth=0;
	long lHeight=0;
	CRect rect = m_pShape->GetParentImageRect();
	CSize size(rect.Width(), rect.Height());
	return size;

	// not m_pShape->GetSize() ... this function is size of the image, not size of the shape.
}

CPoint SVROIFigureEditor::GetReferencePoint()
{
	return m_pShape->GetRect().TopLeft();	// we need top left for ROI purposes
	//return m_pShape->GetCenter();	// this is how MaskShapeFigureEditor works
}



HRESULT SVROIFigureEditor::MouseMove(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;

	CRect rect = m_pShape->GetRect();

	switch ( m_HitType.nHitTest )
	{
		case HTTOPLEFT:
		case HTBOTTOMLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMRIGHT:
		case HTTOP:
		case HTBOTTOM:
		case HTLEFT:
		case HTRIGHT:
		{
			// figure out mouse position differences

			// must take in to consideration what point is used for GetReferencePoint, which is...
			// TopLeft

			// deltaPosition is in relation to the reference point.


			CSize deltaPosition = CalculateDeltaPos( point );

			switch ( m_HitType.nHitTest )
			{
				case HTTOP:
					rect.top    = m_ptReference.y + deltaPosition.cy; break;
				case HTBOTTOM:
					rect.bottom = m_ptReference.y + deltaPosition.cy; break;
				case HTLEFT:
					rect.left   = m_ptReference.x + deltaPosition.cx; break;
				case HTRIGHT:
					rect.right  = m_ptReference.x + deltaPosition.cx; break;

				case HTTOPLEFT:
					rect.top    = m_ptReference.y + deltaPosition.cy;
					rect.left   = m_ptReference.x + deltaPosition.cx; break;
				case HTBOTTOMLEFT:
					rect.bottom = m_ptReference.y + deltaPosition.cy;
					rect.left   = m_ptReference.x + deltaPosition.cx; break;
				case HTTOPRIGHT:
					rect.top    = m_ptReference.y + deltaPosition.cy;
					rect.right  = m_ptReference.x + deltaPosition.cx; break;
				case HTBOTTOMRIGHT:
					rect.bottom = m_ptReference.y + deltaPosition.cy;
					rect.right  = m_ptReference.x + deltaPosition.cx; break;

			}// end switch ( m_HitType.nHitTest )

			rect.NormalizeRect();
			
			m_pShape->SetRect( rect );

			break;
		}// end case multiple

		case HTOBJECT:
			int iWidth = rect.Width();
			int iHeight = rect.Height();
			rect.top = point.y - m_sizeMouseDelta.cy;
			rect.bottom = rect.top + iHeight;
			rect.left = point.x - m_sizeMouseDelta.cx;
			rect.right = rect.left + iWidth;
			m_pShape->SetRect( rect );
			break;
	}// end switch ( m_HitType.nHitTest )


	return hr;
}// SVROIFigureEditor::MouseMove

CSize SVROIFigureEditor::CalculateDeltaPos(CPoint point)
{
	CSize deltaPosition = (point - m_ptReference);
	return deltaPosition;
}// SVROIFigureEditor::CalculateDeltaPos



/* this should be SVFigureEditor::SVHitTestStruct but for some reason it won't compile (VC6 bug?) */
SVROIFigureEditor::SVHitTestStruct SVROIFigureEditor::HitTest( CPoint point )
{
	SVHitTestStruct HitTest = HTNOWHERE;

	CRect rect = m_pShape->GetRect();

	CPoint ptTopLeft = rect.TopLeft();
	CPoint ptTopRight = CPoint( rect.right, rect.top );
	CPoint ptBottomLeft = CPoint( rect.left, rect.bottom );
	CPoint ptBottomRight = rect.BottomRight();

	CRect rectHotSpot = GetHotSpot( point );

	std::auto_ptr<CRgn> prgnTop = GetHotSpot( ptTopLeft, ptTopRight );
	std::auto_ptr<CRgn> prgnLeft = GetHotSpot( ptTopLeft, ptBottomLeft );
	std::auto_ptr<CRgn> prgnBottom = GetHotSpot( ptBottomLeft, ptBottomRight );
	std::auto_ptr<CRgn> prgnRight = GetHotSpot( ptTopRight, ptBottomRight );

	if ( rectHotSpot.PtInRect( ptTopLeft ) )
		HitTest = HTTOPLEFT;
	else if ( rectHotSpot.PtInRect( ptTopRight ) )
		HitTest = HTTOPRIGHT;
	else if ( rectHotSpot.PtInRect( ptBottomLeft ) )
		HitTest = HTBOTTOMLEFT;
	else if ( rectHotSpot.PtInRect( ptBottomRight ) )
		HitTest = HTBOTTOMRIGHT;

	else if ( prgnTop->PtInRegion( point ) )
		HitTest = HTTOP;
	else if ( prgnLeft->PtInRegion( point ) )
		HitTest = HTLEFT;
	else if ( prgnBottom->PtInRegion( point ) )
		HitTest = HTBOTTOM;
	else if ( prgnRight->PtInRegion( point ) )
		HitTest = HTRIGHT;

	else if ( rect.PtInRect( point ) )
		HitTest = HTOBJECT;

	return HitTest;
}// SVROIFigureEditor::HitTest

CSize SVROIFigureEditor::GetHotSpotSize()
{
	CRect rect = m_pShape->GetRect();
	if ( rect.Height() > 32 && rect.Width() > 32 )
		return SVFigureEditor::GetHotSpotSize();
	else
	{
		CSize size;

		if ( rect.Height() > 16 )
			size.cy = 4;
		else if ( rect.Height() > 8 )
			size.cy = 2;
		else if ( rect.Height() > 4 )
			size.cy = 1;
		else
			size.cy = 1; // can't be 0

		if ( rect.Width() > 16 )
			size.cx = 4;
		else if ( rect.Width() > 8 )
			size.cx = 2;
		else if ( rect.Width() > 4 )
			size.cx = 1;
		else
			size.cx = 1; // can't be 0

		return size;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVROIFigureEditor.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:01:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2009 12:34:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Jul 2007 07:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
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
 *    Rev 1.1   18 Aug 2006 16:03:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Aug 2006 11:25:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
