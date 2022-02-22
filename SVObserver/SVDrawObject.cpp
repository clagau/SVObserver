//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVDrawObject::SVDrawObject()
{
	oldPen  = nullptr;
	drawPen = nullptr;

	m_BUseThisPen = false;
	m_PenStyle = PS_SOLID; 
	m_PenWidth = 1; 
	m_PenColor = RGB( 255, 255, 255 );
}

SVDrawObject::SVDrawObject( const SVDrawObject &p_rsvObject )
{
	*this = p_rsvObject;
}

SVDrawObject::~SVDrawObject()
{
	if( drawPen )
	{
		DeleteObject( drawPen );
	}
}

SVDrawObject &SVDrawObject::operator=( const SVDrawObject& rObject )
{
	oldPen  = nullptr;
	drawPen = nullptr;

	m_Points.clear();
	m_CalcPoints.clear();

	SetDrawPen( rObject.m_BUseThisPen, 
		          rObject.m_PenStyle, 
							rObject.m_PenWidth, 
							rObject.m_PenColor );

	m_Points =  rObject.m_Points;
	m_CalcPoints = rObject.m_CalcPoints;

	return *this;
}

void SVDrawObject::AddExtentLineData( SVExtentLineStruct p_svLine, int PenStyle )
{
	long l_lCount = static_cast<long> (p_svLine.m_PointVector.size());

	for( long l = 0; l < l_lCount; l++ )
	{
		AddPoint(static_cast<POINT> (p_svLine.m_PointVector[l]));
	}

	SetDrawPen( TRUE, PenStyle, 1, p_svLine.m_dwColor );
}

BOOL SVDrawObject::Draw( SVDrawContext* PDrawContext )
{
	BOOL BRetVal = false;
	HDC DC = PDrawContext->DC;

	if( beginDraw( PDrawContext ) )
	{
		int Size = static_cast<int> (m_CalcPoints.size());
		
		// Draw...
		if( 1 == Size )
		{
			::SetPixel( DC, m_CalcPoints[0].x, m_CalcPoints[0].y, m_PenColor );
		}
		else if( 1 < Size )
		{
			BRetVal = ::Polyline( DC, m_CalcPoints.data(), Size);
		}
	}
	endDraw( DC );
	return BRetVal;
}

BOOL SVDrawObject::DrawHatch( SVDrawContext* PDrawContext, int& LastY )
{
	BOOL BRetVal = FALSE;
	HDC DC = PDrawContext->DC;
	std::vector<POINT> l_NewPoints;
	static int LastRowY=InvalidPoint;
	if( beginDraw( PDrawContext ) )
	{
		int Size = static_cast<int> (m_CalcPoints.size());
		if( 1 < Size )
		{
			// Logic to reset the lastY point 
			// if we are drawing a second line in the same row.
			if( m_CalcPoints[0].y <= LastY)
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
				BRetVal = ::Polyline( DC, m_CalcPoints.data(), Size );
				LastY = m_CalcPoints[0].y;
			}
			else
			{
				// Draw the second line and fill in every other line
				// up to the current line.
				if( 1 < Size && LastY != InvalidPoint )
				{
					long distance = m_CalcPoints[0].y - LastY;
					if( distance >= 0 )
					{
						POINT Points[2];
						for( long y = 0 ; y <= distance ; y+=2 )
						{
							Points[0].y = LastY + y;
							Points[1].y = Points[0].y;
							Points[0].x = m_CalcPoints[0].x;
							Points[1].x = m_CalcPoints[1].x;
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

BOOL SVDrawObject::SetDrawPen( BOOL BUseThisPen, int PenStyle, int PenWidth, COLORREF PenColor )
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

int SVDrawObject::AddPoint( const POINT Point )
{
	m_Points.push_back(Point);
	return static_cast<int>(m_Points.size() - 1);
}

void SVDrawObject::SetPointAtGrow( int Index, POINT Point )
{
	if (Index >= static_cast<int> (m_Points.size()))
	{
		m_Points.resize(Index + 1);
	}
	m_Points[Index] = Point;
}

void SVDrawObject::SetListSize( int NewSize )
{
	m_Points.resize( NewSize );
}

POINT SVDrawObject::GetPointAt( int Index )
{
	return (Index < static_cast<int> (m_Points.size())) ? m_Points[Index] : POINT();
}

const std::vector<POINT>& SVDrawObject::GetPointArray()
{
	return m_Points;
}
	
HGDIOBJ SVDrawObject::GetDrawPen()
{ 
	if( m_BUseThisPen && nullptr == drawPen )
	{
		drawPen = CreatePen( m_PenStyle, m_PenWidth, m_PenColor );
	}

	return drawPen;
}

BOOL SVDrawObject::beginDraw( SVDrawContext* pDrawContext )
{
	// Set Pen...
	if( m_BUseThisPen && nullptr == drawPen )
	{
		drawPen = CreatePen( m_PenStyle, m_PenWidth, m_PenColor );
	}

	if( drawPen )
	{
		oldPen = ::SelectObject( pDrawContext->DC, drawPen );
	}
	m_CalcPoints.resize(m_Points.size());

	// Transform Input points to Output points
	pDrawContext->Transform(m_Points.data(), m_CalcPoints.data(), static_cast<int> (m_Points.size()));
	
	return true;
}

void SVDrawObject::endDraw( HDC DC )
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

void SVDrawObject::Transform( SVDrawContext* pDrawContext )
{
	m_CalcPoints.resize( m_Points.size() );

	// Transform Input points to Output points
	pDrawContext->Transform( m_Points.data(), m_CalcPoints.data(), static_cast<int> (m_Points.size()) );
}
