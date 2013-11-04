//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShapeFigureEditors
//* .File Name       : $Workfile:   SVMaskShapeFigureEditors.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:27:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVMaskShapeFigureEditors.h"
#include "SVImageLibrary/SVGeometryClass.h"

#pragma warning ( disable : 4065 )

SVMaskShapeFigureEditor::SVMaskShapeFigureEditor()
{
}

HRESULT SVMaskShapeFigureEditor::SetShape( SVMaskShape* pShape )
{
	m_pMaskShape = pShape;
	return S_OK;
}

HRESULT SVMaskShapeFigureEditor::PaintOverlay( CDC& dc, CRect rectViewport, CRect rectDisplay )
{
	COLORREF rgb = RGB(0,255,0);

	CBrush brush;
	brush.CreateSolidBrush(rgb);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, rgb);
	CPen* pOldPen = dc.SelectObject(&pen);

	HRESULT hr = m_pMaskShape->DrawOutline(dc, rectViewport, rectDisplay, rgb);

	dc.SelectObject( pOldPen );
	dc.SelectObject( pOldBrush );

	return hr;
}

CSize SVMaskShapeFigureEditor::GetSizeImpl()
{
	long lWidth=0;
	long lHeight=0;
	SVImageInfoClass info;
	m_pMaskShape->GetImageInfo(info);
	info.GetExtents().GetDimensions().GetExtentProperty( SVExtentPropertyWidth, lWidth );
	info.GetExtents().GetDimensions().GetExtentProperty( SVExtentPropertyHeight, lHeight );
	CSize size(lWidth, lHeight);
	return size;

	// not m_pMaskShape->GetSize() ... this function is size of the image, not size of the shape.
}

CPoint SVMaskShapeFigureEditor::GetReferencePoint()
{
	return m_pMaskShape->GetCenter();
}

CRect SVMaskShapeFigureEditor::GetRect()
{
	return m_pMaskShape->GetRect();
}


HRESULT SVMaskShapeFigureEditor::MouseMove(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;

	SVMaskShape::MapType mapProperties;
	m_pMaskShape->GetProperties( mapProperties );

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

			CSize deltaPosition = CalculateDeltaPos( point );

			switch ( m_HitType.nHitTest )
			{
				case HTTOPLEFT:
					mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2;
					mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
				case HTBOTTOMLEFT:
					mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2;
					mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
				case HTTOPRIGHT:
					mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2;
					mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
				case HTBOTTOMRIGHT:
					mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2;
					mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;

				case HTTOP:
					mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2; break;
				case HTBOTTOM:
					mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2; break;
				case HTLEFT:
					mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
				case HTRIGHT:
					mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;

			}// end switch ( m_HitType.nHitTest )

			mapProperties[ SVShapeMaskPropertyHeightGuid ].value =  max< long >( abs( mapProperties[ SVShapeMaskPropertyHeightGuid ].value ), 10 );
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  max< long >( abs( mapProperties[ SVShapeMaskPropertyWidthGuid  ].value ), 10 );

			m_pMaskShape->SetProperties( mapProperties );
			break;
		}// end case multiple

		case HTOBJECT:
			mapProperties[ SVShapeMaskPropertyCenterYGuid ].value = point.y - m_sizeMouseDelta.cy;
			mapProperties[ SVShapeMaskPropertyCenterXGuid ].value = point.x - m_sizeMouseDelta.cx;
			m_pMaskShape->SetProperties( mapProperties );
			break;
	}// end switch ( m_HitType.nHitTest )


	return hr;
}// SVMaskShapeFigureEditor::MouseMove

CSize SVMaskShapeFigureEditor::CalculateDeltaPos(CPoint point)
{
	CSize deltaPosition = (point - m_ptReference);
	CSize sizeOriginalDeltaPosition = m_ptMouseDown - m_ptReference;

	// prevent divide by 0
	if ( sizeOriginalDeltaPosition.cx == 0 )
		sizeOriginalDeltaPosition.cx = 1;
	if ( sizeOriginalDeltaPosition.cy == 0 )
		sizeOriginalDeltaPosition.cy = 1;

	CRect rect = m_rectOriginal;

	CPoint ptCorner;
	switch ( m_HitType.CursorHT() )
	{
		case HTTOPLEFT:
			ptCorner = rect.TopLeft(); break;
		case HTBOTTOMLEFT:
			ptCorner = CPoint(rect.left, rect.bottom); break;
		case HTTOPRIGHT:
			ptCorner = CPoint(rect.right, rect.top); break;
		case HTBOTTOMRIGHT:
			ptCorner = rect.BottomRight(); break;
		case HTTOP:
			ptCorner = CPoint(m_ptReference.x, rect.top); break;
		case HTBOTTOM:
			ptCorner = CPoint(m_ptReference.x, rect.bottom); break;
		case HTLEFT:
			ptCorner = CPoint(rect.left, m_ptReference.y); break;
		case HTRIGHT:
			ptCorner = CPoint(rect.right, m_ptReference.y); break;
		
	}

	// maintain proportional offset
	CSize sizeOffset = ptCorner - m_ptMouseDown;
	sizeOffset.cx = static_cast<long>((double) sizeOffset.cx * (double) deltaPosition.cx / (double) sizeOriginalDeltaPosition.cx);
	sizeOffset.cy = static_cast<long>((double) sizeOffset.cy * (double) deltaPosition.cy / (double) sizeOriginalDeltaPosition.cy);

	deltaPosition += sizeOffset;

	return deltaPosition;
}// SVMaskShapeFigureEditor::CalculateDeltaPos

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT SVMaskShapeRectangleFigureEditor::MouseMove(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;
	switch ( m_HitType.nHitTest )
	{
		default:
			hr = SVMaskShapeFigureEditor::MouseMove(nFlags, point);
	}
	return hr;
}

/* this should be SVFigureEditor::SVHitTestStruct but for some reason it won't compile (VC6 bug?) */
SVMaskShapeRectangleFigureEditor::SVHitTestStruct SVMaskShapeRectangleFigureEditor::HitTest( CPoint point )
{
	SVHitTestStruct HitTest = HTNOWHERE;

	if ( m_pMaskShape->IsAutoResize() )	// the GUI resizing doesn't work in this mode
		return HitTest;

	CRect rect = m_pMaskShape->GetRect();

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
}// SVMaskShapeRectangleFigureEditor::HitTest



HRESULT SVMaskShapeOvalFigureEditor::MouseMove(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;

	switch ( m_HitType.nHitTest )
	{
		default:
			hr = SVMaskShapeFigureEditor::MouseMove(nFlags, point);
	}
	return hr;
}

/* this should be SVFigureEditor::SVHitTestStruct but for some reason it won't compile (VC6 bug?) */
SVMaskShapeOvalFigureEditor::SVHitTestStruct SVMaskShapeOvalFigureEditor::HitTest( CPoint point )	// for cursor display
{
	SVHitTestStruct HitTest = HTNOWHERE;

	if ( m_pMaskShape->IsAutoResize() )	// the GUI resizing doesn't work in this mode
		return HitTest;

	CPoint ptCenter = m_pMaskShape->GetCenter();
	CSize offset = point - ptCenter;
	CSize sizeShape = m_pMaskShape->GetSize();

	CRect rectHotSpot = GetHotSpot( point );

	double dAngleRadians = g_svPi.GetAngleInRadians(offset.cy, offset.cx);
	double dAngleDegrees = g_svPi.RadiansToDegrees( dAngleRadians );

	double dRadiusMouse = g_svPi.GetHypotenuse(offset.cy, offset.cx);

	double A = (sizeShape.cx / 2.0);	// horizontal axis
	double B = (sizeShape.cy / 2.0);	// vertical axis
	double dCos = ::cos( dAngleRadians );
	double dSin = ::sin( dAngleRadians );

	// short circuit the computations if a dimension is 0.
	if ( A == 0 )
	{
		SVMaskShape::MapType mapProperties;
		m_pMaskShape->GetProperties( mapProperties );

		if ( abs(offset.cy) <= abs(mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2.0) )
			return HTRIGHT;
		else
			return HTNOWHERE;
	}
	else if ( B == 0 )
	{
		SVMaskShape::MapType mapProperties;
		m_pMaskShape->GetProperties( mapProperties );

		if ( abs(offset.cx) <= abs(mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2.0) )
			return HTBOTTOM;
		else
			return HTNOWHERE;
	}


	// formula for radius given the angle from the center obtained at:
	// http://mathworld.wolfram.com/Ellipse.html
	// r'² = b²a² / b²cos²+a²sin²
	double dDenominator = (B*B*dCos*dCos + A*A*dSin*dSin);	// check for 0 to prevent divide by error
	double dRadiusShape = dDenominator == 0.0 ? 0.0 : sqrt( (A*A * B*B) / dDenominator );

	// now figure out what angle will be the cutoff for the hot spots (compute [0-90])
	double dAngle3;//     __3__
	double dAngle2;//   2/\   /\ 
	double dAngle1;//  1|`.\ /.'|

	if ( A >= B )   // major axis is horizontal
	{
		dAngle1 = g_svPi.RadiansToDegrees( atan2(abs(B)/2, abs(A)) );
		dAngle2 = g_svPi.RadiansToDegrees( atan2(abs(B), abs(A)) );
		dAngle3 = 180 - (2*(dAngle1 + dAngle2));
	}
	else            // major axis is vertical
	{
		dAngle3 = g_svPi.RadiansToDegrees( atan2(abs(A)/2, abs(B)) ) * 2;
		dAngle2 = g_svPi.RadiansToDegrees( atan2(abs(A), abs(B)) );
		dAngle1 = (180 - (dAngle3 + 2*dAngle2))/2;
	}
	//TRACE("Angle 1=%f, Angle 2=%f, Angle 3=%f\n", dAngle1, dAngle2, dAngle3);

	if ( dRadiusMouse > dRadiusShape + SVFigureEditor::f_iHotSpotSizeX )
		HitTest = HTNOWHERE;
	else if ( dRadiusMouse < dRadiusShape - SVFigureEditor::f_iHotSpotSizeX )
		HitTest = HTOBJECT;
	else
	{
		if ( dAngleDegrees < (-180.0 + dAngle1) || dAngleDegrees >= (180 - dAngle1) )
			HitTest = HTLEFT;
		else if ( dAngleDegrees >= (-180 + dAngle1) && dAngleDegrees < (-180 + dAngle1 + dAngle2 ) )
			HitTest = HTTOPLEFT;
		else if ( dAngleDegrees >= (-180 + dAngle1 + dAngle2 ) && dAngleDegrees < (-180 + dAngle1 + dAngle2 + dAngle3) )
			HitTest = HTTOP;
		else if ( dAngleDegrees >= (-180 + dAngle1 + dAngle2 + dAngle3) && dAngleDegrees < (-180 + dAngle1 + 2*dAngle2 + dAngle3) )
			HitTest = HTTOPRIGHT;
		else if ( dAngleDegrees >= (-180 + dAngle1 + 2*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 2*dAngle2 + dAngle3) )
			HitTest = HTRIGHT;
		else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 2*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 3*dAngle2 + dAngle3) )
			HitTest = HTBOTTOMRIGHT;
		else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 3*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 3*dAngle2 + 2*dAngle3) )
			HitTest = HTBOTTOM;
		else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 3*dAngle2 + 2*dAngle3) && dAngleDegrees < (180-dAngle1) )
			HitTest = HTBOTTOMLEFT;

		//TRACE("Angle = %f, HT=%d, rm=%f, rs=%f\n", dAngleDegrees, HitTest.nHitTest, dRadiusMouse, dRadiusShape);

	}
	return HitTest;
}// SVMaskShapeOvalFigureEditor::HitTest

CRect SVMaskShapeDoughnutFigureEditor::GetRect()
{
	CRect rect(SVMaskShapeFigureEditor::GetRect());

	SVMaskShape::MapType mapProperties;
	m_pMaskShape->GetProperties( mapProperties );

	if ( (m_HitType.nHitTest & 0xFF00) == SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE )
	{
		rect.DeflateRect(CSize( mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value,
		                        mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value) );
	}

	return rect;
}

HRESULT SVMaskShapeDoughnutFigureEditor::MouseMove(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;

	SVMaskShape::MapType mapProperties;
	m_pMaskShape->GetProperties( mapProperties );

	CSize sizeShape = m_pMaskShape->GetSize();

	// maintain proportional offset
	CSize deltaPosition = CalculateDeltaPos( point );

	switch ( m_HitType.nHitTest )
	{
		case HTSVINNERTOP:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value = sizeShape.cy/2 - abs(deltaPosition.cy);
			break;
		case HTSVINNERBOTTOM:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value =  sizeShape.cy/2 - abs(deltaPosition.cy);
			break;
		case HTSVINNERLEFT:
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value = sizeShape.cx/2 - abs(deltaPosition.cx);
			break;
		case HTSVINNERRIGHT:
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value =  sizeShape.cx/2 - abs(deltaPosition.cx);
			break;
		case HTSVINNERTOPLEFT:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value = sizeShape.cy/2 - abs(deltaPosition.cy);
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value = sizeShape.cx/2 - abs(deltaPosition.cx);
			break;
		case HTSVINNERBOTTOMLEFT:
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value = sizeShape.cx/2 - abs(deltaPosition.cx);
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value = sizeShape.cy/2 - abs(deltaPosition.cy);
			break;
		case HTSVINNERTOPRIGHT:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value = sizeShape.cy/2 - abs(deltaPosition.cy);
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value = sizeShape.cx/2 - abs(deltaPosition.cx);
			break;
		case HTSVINNERBOTTOMRIGHT:
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value = sizeShape.cx/2 - abs(deltaPosition.cx);
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value = sizeShape.cy/2 - abs(deltaPosition.cy);
			break;


		case HTSVOUTERTOP:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value +=  abs(deltaPosition.cy) - mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2; break;
		case HTSVOUTERBOTTOM:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value +=  abs(deltaPosition.cy) - mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyHeightGuid ].value =  abs(deltaPosition.cy) * 2; break;
		case HTSVOUTERLEFT:
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value +=  abs(deltaPosition.cx) - mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
		case HTSVOUTERRIGHT:
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value +=  abs(deltaPosition.cx) - mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  abs(deltaPosition.cx) * 2; break;
		case HTSVOUTERTOPLEFT:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value +=  abs(deltaPosition.cy) - mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value +=  abs(deltaPosition.cx) - mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2;
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
		case HTSVOUTERBOTTOMLEFT:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value +=  abs(deltaPosition.cy) - mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value +=  abs(deltaPosition.cx) - mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyHeightGuid ].value =  abs(deltaPosition.cy) * 2;
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value = abs(deltaPosition.cx) * 2; break;
		case HTSVOUTERTOPRIGHT:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value +=  abs(deltaPosition.cy) - mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value +=  abs(deltaPosition.cx) - mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyHeightGuid ].value = abs(deltaPosition.cy) * 2;
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  abs(deltaPosition.cx) * 2; break;
		case HTSVOUTERBOTTOMRIGHT:
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value +=  abs(deltaPosition.cy) - mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value +=  abs(deltaPosition.cx) - mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2;
			mapProperties[ SVShapeMaskPropertyHeightGuid ].value =  abs(deltaPosition.cy) * 2;
			mapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  abs(deltaPosition.cx) * 2; break;

		default:
			return SVMaskShapeFigureEditor::MouseMove(nFlags, point);
	}// end switch ( m_HitType.nHitTest )

	mapProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( abs( mapProperties[ SVShapeMaskPropertyHeightGuid ].value ), 10 );
	mapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( abs( mapProperties[ SVShapeMaskPropertyWidthGuid  ].value ), 10 );
	mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value =  std::max< long >( mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value, 1 );
	mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value =  std::max< long >( mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value, 1 );
	mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value =  std::min< long >( mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value, mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2 - 1 );
	mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value =  std::min< long >( mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value, mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2 - 1 );
	//mapProperties[ SVShapeMaskPropertyHeightGuid ].value =  max ( mapProperties[ SVShapeMaskPropertyHeightGuid ].value, mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value * 2 + 2 );
	//mapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  max ( mapProperties[ SVShapeMaskPropertyWidthGuid  ].value, mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value * 2 + 2);

	m_pMaskShape->SetProperties( mapProperties );

	return hr;
}// SVMaskShapeDoughnutFigureEditor::MouseMove

/* this should be SVFigureEditor::SVHitTestStruct but for some reason it won't compile (VC6 bug?) */
SVMaskShapeDoughnutFigureEditor::SVHitTestStruct SVMaskShapeDoughnutFigureEditor::HitTest( CPoint point )	// for cursor display
{
	SVHitTestStruct HitTest = HTNOWHERE;

	if ( m_pMaskShape->IsAutoResize() )	// the GUI resizing doesn't work in this mode
		return HitTest;

	CPoint ptCenter = m_pMaskShape->GetCenter();
	CSize offset = point - ptCenter;
	CSize sizeShape = m_pMaskShape->GetSize();

	CRect rectHotSpot = GetHotSpot( point );
	SVMaskShape::MapType mapProperties;
	m_pMaskShape->GetProperties( mapProperties );

	double dAngleRadians = g_svPi.GetAngleInRadians(offset.cy, offset.cx);
	double dAngleDegrees = g_svPi.RadiansToDegrees( dAngleRadians );

	double dRadiusMouse = g_svPi.GetHypotenuse(offset.cy, offset.cx);

	double A = (sizeShape.cx / 2.0);	// horizontal axis
	double B = (sizeShape.cy / 2.0);	// vertical axis
	double dCos = ::cos( dAngleRadians );
	double dSin = ::sin( dAngleRadians );


	// short circuit the computations if a dimension is 0.
	if ( A == 0 )	// width is 0
	{
		if ( abs(offset.cy) <= abs(mapProperties[ SVShapeMaskPropertyHeightGuid ].value / 2.0) )
			return HTSVOUTERRIGHT;
		else
			return HTNOWHERE;
	}
	else if ( B == 0 ) // height is 0
	{
		if ( abs(offset.cx) <= abs(mapProperties[ SVShapeMaskPropertyWidthGuid ].value / 2.0) )
			return HTSVOUTERBOTTOM;
		else
			return HTNOWHERE;
	}



	// formula for radius given the angle from the center obtained at:
	// http://mathworld.wolfram.com/Ellipse.html
	// r'² = b²a² / b²cos²+a²sin²
	double dDenominator = (B*B*dCos*dCos + A*A*dSin*dSin);	// check for 0 to prevent divide by error
	double dRadiusOuter = dDenominator == 0 ? 0.0 : sqrt( (A*A * B*B) / dDenominator );

	// inner ellipse computations...
	double A1 = A - mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value;      // horizontal axis
	double B1 = B - mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value; // vertical axis
	dDenominator = (B1*B1*dCos*dCos + A1*A1*dSin*dSin);// check for 0 to prevent divide by error
	double dRadiusInner = dDenominator == 0.0 ? 0.0 : sqrt( (A1*A1 * B1*B1) / dDenominator );

	// now figure out what angle will be the cutoff for the hot spots (compute [0-90])
	double dAngle3;//     __3__
	double dAngle2;//   2/\   /\ 
	double dAngle1;//  1|`.\ /.'|
	double dInnerAngle3;
	double dInnerAngle2;
	double dInnerAngle1;

	// Outer ellipse
	if ( A >= B )   // major axis is horizontal
	{
		dAngle1 = g_svPi.RadiansToDegrees( atan2(abs(B)/2, abs(A)) );
		dAngle2 = g_svPi.RadiansToDegrees( atan2(abs(B), abs(A)) );
		dAngle3 = 180 - (2*(dAngle1 + dAngle2));
	}
	else            // major axis is vertical
	{
		dAngle3 = g_svPi.RadiansToDegrees( atan2(abs(A)/2, abs(B)) ) * 2;
		dAngle2 = g_svPi.RadiansToDegrees( atan2(abs(A), abs(B)) );
		dAngle1 = (180 - (dAngle3 + 2*dAngle2))/2;
	}

	// Inner ellipse
	if ( A1 >= B1 )   // major axis is horizontal
	{
		dInnerAngle1 = g_svPi.RadiansToDegrees( atan2(abs(B1)/2, abs(A1)) );
		dInnerAngle2 = g_svPi.RadiansToDegrees( atan2(abs(B1), abs(A1)) );
		dInnerAngle3 = 180 - (2*(dInnerAngle1 + dInnerAngle2));
	}
	else            // major axis is vertical
	{
		dInnerAngle3 = g_svPi.RadiansToDegrees( atan2(abs(A1)/2, abs(B1)) ) * 2;
		dInnerAngle2 = g_svPi.RadiansToDegrees( atan2(abs(A1), abs(B1)) );
		dInnerAngle1 = (180 - (dInnerAngle3 + 2*dInnerAngle2))/2;
	}
	//TRACE("Angle 1=%f, Angle 2=%f, Angle 3=%f\n", dAngle1, dAngle2, dAngle3);

	if ( dRadiusMouse > dRadiusOuter + SVFigureEditor::f_iHotSpotSizeX )
		HitTest = HTNOWHERE;
	else if ( dRadiusMouse < dRadiusInner - SVFigureEditor::f_iHotSpotSizeX )
		HitTest = HTOBJECT;
	else
	{
		// priority: outer edge, inner edge, inside loop
		if ( dRadiusMouse > dRadiusOuter - SVFigureEditor::f_iHotSpotSizeX * 2 )	// hit outer edge
		{
			if ( dAngleDegrees < (-180.0 + dAngle1) || dAngleDegrees >= (180 - dAngle1) )
				HitTest = HTSVOUTERLEFT;
			else if ( dAngleDegrees >= (-180 + dAngle1) && dAngleDegrees < (-180 + dAngle1 + dAngle2 ) )
				HitTest = HTSVOUTERTOPLEFT;
			else if ( dAngleDegrees >= (-180 + dAngle1 + dAngle2 ) && dAngleDegrees < (-180 + dAngle1 + dAngle2 + dAngle3) )
				HitTest = HTSVOUTERTOP;
			else if ( dAngleDegrees >= (-180 + dAngle1 + dAngle2 + dAngle3) && dAngleDegrees < (-180 + dAngle1 + 2*dAngle2 + dAngle3) )
				HitTest = HTSVOUTERTOPRIGHT;
			else if ( dAngleDegrees >= (-180 + dAngle1 + 2*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 2*dAngle2 + dAngle3) )
				HitTest = HTSVOUTERRIGHT;
			else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 2*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 3*dAngle2 + dAngle3) )
				HitTest = HTSVOUTERBOTTOMRIGHT;
			else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 3*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 3*dAngle2 + 2*dAngle3) )
				HitTest = HTSVOUTERBOTTOM;
			else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 3*dAngle2 + 2*dAngle3) && dAngleDegrees < (180-dAngle1) )
				HitTest = HTSVOUTERBOTTOMLEFT;
		}
		else if ( dRadiusMouse < dRadiusInner + SVFigureEditor::f_iHotSpotSizeX * 2)	// hit inner edge
		{
			if ( dAngleDegrees < (-180.0 + dInnerAngle1) || dAngleDegrees >= (180 - dInnerAngle1) )
				HitTest = HTSVINNERLEFT;
			else if ( dAngleDegrees >= (-180 + dInnerAngle1) && dAngleDegrees < (-180 + dInnerAngle1 + dInnerAngle2 ) )
				HitTest = HTSVINNERTOPLEFT;
			else if ( dAngleDegrees >= (-180 + dInnerAngle1 + dInnerAngle2 ) && dAngleDegrees < (-180 + dInnerAngle1 + dInnerAngle2 + dInnerAngle3) )
				HitTest = HTSVINNERTOP;
			else if ( dAngleDegrees >= (-180 + dInnerAngle1 + dInnerAngle2 + dInnerAngle3) && dAngleDegrees < (-180 + dInnerAngle1 + 2*dInnerAngle2 + dInnerAngle3) )
				HitTest = HTSVINNERTOPRIGHT;
			else if ( dAngleDegrees >= (-180 + dInnerAngle1 + 2*dInnerAngle2 + dInnerAngle3) && dAngleDegrees < (-180 + 3*dInnerAngle1 + 2*dInnerAngle2 + dInnerAngle3) )
				HitTest = HTSVINNERRIGHT;
			else if ( dAngleDegrees >= (-180 + 3*dInnerAngle1 + 2*dInnerAngle2 + dInnerAngle3) && dAngleDegrees < (-180 + 3*dInnerAngle1 + 3*dInnerAngle2 + dInnerAngle3) )
				HitTest = HTSVINNERBOTTOMRIGHT;
			else if ( dAngleDegrees >= (-180 + 3*dInnerAngle1 + 3*dInnerAngle2 + dInnerAngle3) && dAngleDegrees < (-180 + 3*dInnerAngle1 + 3*dInnerAngle2 + 2*dInnerAngle3) )
				HitTest = HTSVINNERBOTTOM;
			else if ( dAngleDegrees >= (-180 + 3*dInnerAngle1 + 3*dInnerAngle2 + 2*dInnerAngle3) && dAngleDegrees < (180-dInnerAngle1) )
				HitTest = HTSVINNERBOTTOMLEFT;
		}
		else	// inside loop; use outer ellipse to determine hit test
		{
			if ( dAngleDegrees < (-180.0 + dAngle1) || dAngleDegrees >= (180 - dAngle1) )
				HitTest = HTLEFT;
			else if ( dAngleDegrees >= (-180 + dAngle1) && dAngleDegrees < (-180 + dAngle1 + dAngle2 ) )
				HitTest = HTTOPLEFT;
			else if ( dAngleDegrees >= (-180 + dAngle1 + dAngle2 ) && dAngleDegrees < (-180 + dAngle1 + dAngle2 + dAngle3) )
				HitTest = HTTOP;
			else if ( dAngleDegrees >= (-180 + dAngle1 + dAngle2 + dAngle3) && dAngleDegrees < (-180 + dAngle1 + 2*dAngle2 + dAngle3) )
				HitTest = HTTOPRIGHT;
			else if ( dAngleDegrees >= (-180 + dAngle1 + 2*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 2*dAngle2 + dAngle3) )
				HitTest = HTRIGHT;
			else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 2*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 3*dAngle2 + dAngle3) )
				HitTest = HTBOTTOMRIGHT;
			else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 3*dAngle2 + dAngle3) && dAngleDegrees < (-180 + 3*dAngle1 + 3*dAngle2 + 2*dAngle3) )
				HitTest = HTBOTTOM;
			else if ( dAngleDegrees >= (-180 + 3*dAngle1 + 3*dAngle2 + 2*dAngle3) && dAngleDegrees < (180-dAngle1) )
				HitTest = HTBOTTOMLEFT;
		}
	}// end else

		//TRACE("Angle = %f, HT=%d, rm=%f, rs=%f\n", dAngleDegrees, HitTest.nHitTest, dRadiusMouse, dRadiusShape);

	return HitTest;
}// SVMaskShapeDoughnutFigureEditor::HitTest

HRESULT SVMaskShapeSymmetricTrapezoidFigureEditor::MouseMove(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;
	SVMaskShape::MapType mapProperties;
	m_pMaskShape->GetProperties( mapProperties );
	SVMaskShapeSymmetricTrapezoid::SymmetryOrientation eOrientation = static_cast<SVMaskShapeSymmetricTrapezoid::SymmetryOrientation> (mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value);
	
	// figure out mouse position differences

	//CSize deltaPosition = CalculateDeltaPos( point );
	CSize deltaPosition = point - m_ptReference;
	CSize sizeShape = m_pMaskShape->GetSize();

	bool bHandled = false;

	deltaPosition.cx = std::min< long >( deltaPosition.cx, sizeShape.cx / 2 );
	deltaPosition.cy = std::min< long >( deltaPosition.cy, sizeShape.cy / 2 );
	//deltaPosition.cx = max( 0, abs(deltaPosition.cx) );
	//deltaPosition.cy = max( 0, abs(deltaPosition.cy) );

	switch ( m_HitType.nHitTest )
	{
		case HTTOPLEFT:
			if ( eOrientation == SVMaskShapeSymmetricTrapezoid::VerticalAxisTop )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cx/2 - abs(deltaPosition.cx)),
				bHandled = true;
			else if ( eOrientation == SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cy/2 - abs(deltaPosition.cy)),
				bHandled = true;
			break;
		case HTTOPRIGHT:
			if ( eOrientation == SVMaskShapeSymmetricTrapezoid::VerticalAxisTop )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cx/2 - abs(deltaPosition.cx)),
				bHandled = true;
			else if ( eOrientation == SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cy/2 - abs(deltaPosition.cy)),
				bHandled = true;
			break;
		case HTBOTTOMLEFT:
			if ( eOrientation == SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cx/2 - abs(deltaPosition.cx)),
				bHandled = true;
			else if ( eOrientation == SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cy/2 - abs(deltaPosition.cy)),
				bHandled = true;
			break;
		case HTBOTTOMRIGHT:
			if ( eOrientation == SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cx/2 - abs(deltaPosition.cx)),
				bHandled = true;
			else if ( eOrientation == SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight )
				mapProperties[ SVShapeMaskPropertyOffsetGuid ].value = max< long >(0, sizeShape.cy/2 - abs(deltaPosition.cy)),
				bHandled = true;
			break;
	}// end switch ( m_HitType.nHitTest )

	if ( !bHandled )
		hr = SVMaskShapeFigureEditor::MouseMove(nFlags, point);
	else
		m_pMaskShape->SetProperties( mapProperties );
	return hr;
}// SVMaskShapeSymmetricTrapezoidFigureEditor::MouseMove

/* this should be SVFigureEditor::SVHitTestStruct but for some reason it won't compile (VC6 bug?) */
SVMaskShapeSymmetricTrapezoidFigureEditor::SVHitTestStruct SVMaskShapeSymmetricTrapezoidFigureEditor::HitTest( CPoint point )	// for cursor display
{
	SVHitTestStruct HitTest = HTNOWHERE;

	/*
	// we will allow it for this one case because the offset works and that is what the customer wants anyway.
	if ( m_pMaskShape->IsAutoResize() )	// the GUI resizing doesn't work in this mode
		return HitTest;
	*/

	SVMaskShape::MapType mapProperties;
	m_pMaskShape->GetProperties( mapProperties );
	CRect rect = m_pMaskShape->GetRect();

	CPoint ptTopLeft = rect.TopLeft();
	CPoint ptTopRight = CPoint( rect.right, rect.top );
	CPoint ptBottomLeft = CPoint( rect.left, rect.bottom );
	CPoint ptBottomRight = rect.BottomRight();

	CRect rectHotSpot = GetHotSpot( point );

	long lOffset = mapProperties[ SVShapeMaskPropertyOffsetGuid ].value;

	switch ( mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value )
	{
		case SVMaskShapeSymmetricTrapezoid::VerticalAxisTop:
			ptTopLeft.x += lOffset;
			ptTopRight.x -= lOffset;
			break;
		case SVMaskShapeSymmetricTrapezoid::VerticalAxisBottom:
			ptBottomLeft.x += lOffset;
			ptBottomRight.x -= lOffset;
			break;
		case SVMaskShapeSymmetricTrapezoid::HorizontalAxisLeft:
			ptTopLeft.y += lOffset;
			ptBottomLeft.y -= lOffset;
			break;
		case SVMaskShapeSymmetricTrapezoid::HorizontalAxisRight:
			ptTopRight.y += lOffset;
			ptBottomRight.y -= lOffset;
			break;
	}

	CRgn region;
	CPoint apoints[4] = {ptTopRight, ptTopLeft, ptBottomLeft, ptBottomRight};
	region.CreatePolygonRgn(apoints, 4, WINDING);

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

	else if ( region.PtInRegion( point ) )
		HitTest = HTOBJECT;

	return HitTest;
}// SVMaskShapeSymmetricTrapezoidFigureEditor::HitTest

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMaskShapeFigureEditors.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:27:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Mar 2011 15:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Mar 2011 13:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 12:00:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 14:16:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   31 Mar 2006 09:20:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Auto Resize capability to the Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2006 10:05:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  543
 * SCR Title:  Fix crash when editing a Shape Mask through the GUI
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed divide by 0 errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2005 11:20:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Aug 2005 15:39:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed 0 bug in HitTest for Oval and Doughnut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:28:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
