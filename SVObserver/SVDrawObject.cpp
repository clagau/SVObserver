//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawObject
//* .File Name       : $Workfile:   SVDrawObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Mar 2014 09:43:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDrawObject.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDrawObjectClass::SVDrawObjectClass()
{
	oldPen  = nullptr;
	drawPen = nullptr;

	m_BUseThisPen = false;
	m_PenStyle = PS_SOLID; 
	m_PenWidth = 1; 
	m_PenColor = RGB( 255, 255, 255 );
}

SVDrawObjectClass::SVDrawObjectClass( const SVDrawObjectClass &p_rsvObject )
{
	*this = p_rsvObject;
}

SVDrawObjectClass::~SVDrawObjectClass()
{
	if( drawPen )
	{
		DeleteObject( drawPen );
	}
}

const SVDrawObjectClass &SVDrawObjectClass::operator=( const SVDrawObjectClass &p_rsvObject )
{
	oldPen  = nullptr;
	drawPen = nullptr;

	points.RemoveAll();
	calcPoints.RemoveAll();

	SetDrawPen( p_rsvObject.m_BUseThisPen, 
		          p_rsvObject.m_PenStyle, 
							p_rsvObject.m_PenWidth, 
							p_rsvObject.m_PenColor );

	points.Copy( p_rsvObject.points );
	calcPoints.Copy( p_rsvObject.calcPoints );

	return *this;
}

void SVDrawObjectClass::AddExtentLineData( SVExtentLineStruct p_svLine, int PenStyle )
{
	long l_lCount = p_svLine.m_svPointArray.GetSize();

	for( long l = 0; l < l_lCount; l++ )
	{
		AddPoint( p_svLine.m_svPointArray[ l ] );
	}

	SetDrawPen( TRUE, PenStyle, 1, p_svLine.m_dwColor );
}

BOOL SVDrawObjectClass::Draw( SVDrawContext* PDrawContext )
{
	BOOL BRetVal = false;
	HDC DC = PDrawContext->DC;

	if( beginDraw( PDrawContext ) )
	{
		int l_iSize = calcPoints.GetSize();
		
		// Draw...
		if( l_iSize == 1  )
		{
			::SetPixel( DC, calcPoints[0].x, calcPoints[0].y, m_PenColor );
		}
		else
		{
			if( l_iSize > 1 )
			{
				BRetVal = ::Polyline( DC, calcPoints.GetData(), calcPoints.GetSize() );
			}
		}
	}
	endDraw( DC );
	return BRetVal;
}

BOOL SVDrawObjectClass::DrawHatch( SVDrawContext* PDrawContext, int& LastY )
{
	BOOL BRetVal = FALSE;
	HDC DC = PDrawContext->DC;
	SVCPointArray l_NewPoints;
	static int LastRowY=InvalidPoint;
	if( beginDraw( PDrawContext ) )
	{
		int l_iSize = calcPoints.GetSize();
		if( l_iSize > 1 )
		{
			// Logic to reset the lastY point 
			// if we are drawing a second line in the same row.
			if( calcPoints[0].y <= LastY)
			{
				LastY = LastRowY;
			}
			else
			{
				LastRowY = LastY;
			}
			// Draw the first line.
			if(  LastY == InvalidPoint )
			{
				BRetVal = ::Polyline( DC, calcPoints.GetData(), calcPoints.GetSize() );
				LastY = calcPoints[0].y;
			}
			else
			{
				// Draw the second line and fill in every other line
				// up to the current line.
				if( l_iSize > 1 && LastY != InvalidPoint )
				{
					long distance = calcPoints[0].y - LastY;
					if( distance >= 0 )
					{
						POINT Points[2];
						for( long y = 0 ; y <= distance ; y+=2 )
						{
							Points[0].y = LastY + y;
							Points[1].y = Points[0].y;
							Points[0].x = calcPoints[0].x;
							Points[1].x = calcPoints[1].x;
							BRetVal = ::Polyline( DC, Points, 2 );  
						}
						// Remember the last point as a starting point 
						// for the next time to fill in between lines.
						LastY = Points[0].y;
					}
				}
			}
		}
	}
	endDraw( DC );
	return BRetVal;
}

BOOL SVDrawObjectClass::SetDrawPen( BOOL BUseThisPen, int PenStyle, int PenWidth, COLORREF PenColor )
{
	if( drawPen )
	{
		DeleteObject( drawPen );
		drawPen = nullptr;
	}

	m_BUseThisPen = BUseThisPen;
	m_PenStyle = PenStyle; 
	m_PenWidth = PenWidth; 
	m_PenColor = PenColor;

	return true;
}

int SVDrawObjectClass::AddPoint( const POINT p_oPoint )
{
	return static_cast< int >( points.Add( p_oPoint ) );
}

int SVDrawObjectClass::AddPoint( const CPoint& RPoint )
{
	return static_cast< int >( points.Add( RPoint ) );
}

void SVDrawObjectClass::SetPointAtGrow( int Index, const CPoint& RPoint )
{
	points.SetAtGrow( Index, RPoint );
}

void SVDrawObjectClass::SetPointAtGrow( int Index, POINT p_oPoint )
{
	points.SetAtGrow( Index, p_oPoint );
}

void SVDrawObjectClass::SetListSize( int NewSize )
{
	points.SetSize( NewSize );
}

CPoint SVDrawObjectClass::GetPointAt( int Index )
{
	return points.GetAt(Index);
}

const SVCPointArray& SVDrawObjectClass::GetPointArray()
{
	return points;
}
	
HGDIOBJ SVDrawObjectClass::GetDrawPen()
{ 
	if( m_BUseThisPen && nullptr == drawPen )
	{
		drawPen = CreatePen( m_PenStyle, m_PenWidth, m_PenColor );
	}

	return drawPen;
}

BOOL SVDrawObjectClass::beginDraw( SVDrawContext* PDrawContext )
{
	// Set Pen...
	if( m_BUseThisPen && nullptr == drawPen )
	{
		drawPen = CreatePen( m_PenStyle, m_PenWidth, m_PenColor );
	}

	if( drawPen )
	{
		oldPen = ::SelectObject( PDrawContext->DC, drawPen );
	}
	calcPoints.SetSize( points.GetSize() );

	// Transform Input points to Output points
	PDrawContext->Transform( points.GetData(), calcPoints.GetData(), points.GetSize() );
	
	return true;
}

void SVDrawObjectClass::endDraw( HDC DC )
{
	// Deselect default pen if exists...
	if( drawPen && oldPen )
	{
		::SelectObject( DC, oldPen );
	}
	if( drawPen )
	{
		DeleteObject( drawPen );
		drawPen = nullptr;
	}
}

void SVDrawObjectClass::Transform( SVDrawContext* PDrawContext )
{
	calcPoints.SetSize( points.GetSize() );

	// Transform Input points to Output points
	PDrawContext->Transform( points.GetData(), calcPoints.GetData(), points.GetSize() );
}
