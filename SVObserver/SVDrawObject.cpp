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

#include "stdafx.h"
#include "SVDrawObject.h"
#include "SVImageLibrary/SVImageExtentClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double wPI = 3.1415926f;

BOOL SVCalculateRotation( POINT* PSourcePointVector, POINT* PDestinyPointVector, int Count, const POINT& RRotationCenter, double RotationAngle, long lZoomFactor )
{
	if( PSourcePointVector && PDestinyPointVector )
	{
		double radGrad = wPI / 180.0;
		double angle = RotationAngle * radGrad;
		long   xr, yr; 
		double r;
		double beta;
		double res;
		for( int i = 0; i < Count; ++i )
		{
      if (angle != 0.0)
      {
			  xr = PSourcePointVector[ i ].x - RRotationCenter.x;
			  yr = PSourcePointVector[ i ].y - RRotationCenter.y;
			  res = xr * xr + yr * yr;
			  r = ( res > 0.0 ) ? sqrt( res ) : 0;
			  beta = atan2( (double)yr, (double)xr );
      
			  PDestinyPointVector[ i ].x = RRotationCenter.x + ( long ) ( r * cos( angle + beta ) );
			  PDestinyPointVector[ i ].y = RRotationCenter.y + ( long ) ( r * sin( angle + beta ) );
      }
      else
      {
			  PDestinyPointVector[ i ].x = PSourcePointVector[i].x;
			  PDestinyPointVector[ i ].y = PSourcePointVector[i].y;
      }

      if ((lZoomFactor != 0) && (lZoomFactor >= -16) && (lZoomFactor <= 16))
      {
        if (lZoomFactor < 0)
        {
            PDestinyPointVector[i].x = PDestinyPointVector[i].x / (-lZoomFactor);
            PDestinyPointVector[i].y = PDestinyPointVector[i].y / (-lZoomFactor);
        }
        else
        {
            PDestinyPointVector[i].x = PDestinyPointVector[i].x * lZoomFactor;
            PDestinyPointVector[i].y = PDestinyPointVector[i].y * lZoomFactor;
        }
      }

		}
		return TRUE;
	}
	return FALSE;
}

POINT SVCalculateRotation( const POINT& RSourcePoint, const POINT& RRotationCenter, double RotationAngle, long lZoomFactor )
{
	double radGrad = wPI / 180.0;
	double angle = RotationAngle * radGrad;
  POINT DestinyPoint;

  if (angle != 0.0)
  {
	  long   xr = RSourcePoint.x - RRotationCenter.x;
	  long   yr = RSourcePoint.y - RRotationCenter.y;
	  double res = xr * xr + yr * yr;
	  double r = ( res > 0.0 ) ? sqrt( res ) : 0;
	  double beta = atan2( (double)yr, (double)xr );
	  DestinyPoint.x = RRotationCenter.x + ( long ) ( r * cos( angle + beta ) );
	  DestinyPoint.y = RRotationCenter.y + ( long ) ( r * sin( angle + beta ) );
  }
  else
  {
    DestinyPoint.x = RSourcePoint.x;
	  DestinyPoint.y = RSourcePoint.y;
  }

  if ((lZoomFactor != 0) && (lZoomFactor >= -16) && (lZoomFactor <= 16))
  {
    if (lZoomFactor < 0)
    {
        DestinyPoint.x = DestinyPoint.x / (-lZoomFactor);
        DestinyPoint.y = DestinyPoint.y / (-lZoomFactor);
    }
    else
    {
        DestinyPoint.x = DestinyPoint.x * lZoomFactor;
        DestinyPoint.y = DestinyPoint.y * lZoomFactor;
    }
  }
	return DestinyPoint;
}


SVDrawObjectClass::SVDrawObjectClass()
{
	oldPen  = NULL;
	drawPen = NULL;

	m_bDrawSinglePoints = false;
	m_BUseThisPen = FALSE;
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
		DeleteObject( drawPen );
}

const SVDrawObjectClass &SVDrawObjectClass::operator=( const SVDrawObjectClass &p_rsvObject )
{
	oldPen  = NULL;
	drawPen = NULL;

	points.RemoveAll();
	calcPoints.RemoveAll();

	SetDrawPen( p_rsvObject.m_BUseThisPen, 
		          p_rsvObject.m_PenStyle, 
							p_rsvObject.m_PenWidth, 
							p_rsvObject.m_PenColor );

	points.Copy( p_rsvObject.points );
	calcPoints.Copy( p_rsvObject.calcPoints );

	m_bDrawSinglePoints = p_rsvObject.m_bDrawSinglePoints;

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

void SVDrawObjectClass::AddExtentFigureData( SVExtentFigureStruct p_svFigure )
{
	double l_dAngle;

	switch( p_svFigure.m_eShape )
	{
		case SVExtentShapeArrow:
		{
			AddPoint( p_svFigure.m_svTopLeft );
			AddPoint( p_svFigure.m_svBottomLeft );
			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svCenterRight );
			AddPoint( p_svFigure.m_svTopRight );
			AddPoint( p_svFigure.m_svCenterRight );
			AddPoint( p_svFigure.m_svBottomRight );

			break;
		}
		case SVExtentShapeLine:
		case SVExtentShapeLineHorizontal:
		{
			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svCenterRight );

			break;
		}
		case SVExtentShapeLineVertical:
		{
			AddPoint( p_svFigure.m_svTopCenter );
			AddPoint( p_svFigure.m_svBottomCenter );

			break;
		}
		case SVExtentShapeRectangle:
		{
			AddPoint( p_svFigure.m_svTopLeft );
			AddPoint( p_svFigure.m_svTopRight );
			AddPoint( p_svFigure.m_svBottomRight );
			AddPoint( p_svFigure.m_svBottomLeft );
			AddPoint( p_svFigure.m_svTopLeft );

			break;
		}
		case SVExtentShapePie:
		{
			SVExtentPointStruct l_svPoint;

			double l_dStartAngle = SVGetRotationAngle( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( p_svFigure.m_svTopCenter, p_svFigure.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomRight );

			if ( l_dStartAngle < l_dEndAngle )
			{
				l_dEndAngle -= 360;
			}

			double l_dStep = ( l_dEndAngle - l_dStartAngle ) / ( l_dInnerRadius >= 1 ? l_dInnerRadius : 1 );
			double l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svTopCenter );
			AddPoint( p_svFigure.m_svCenterRight );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			for( l_dAngle = l_dMinAngle + l_dLoopStep; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle += l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dInnerRadius, l_dLoopAngle );

				AddPoint( l_svPoint );
			}

			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svBottomLeft );

			l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dOuterRadius;
			l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			l_dLoopAngle = l_dEndAngle;

			for( l_dAngle = l_dMinAngle + l_dLoopStep; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dOuterRadius, l_dLoopAngle );

				AddPoint( l_svPoint );
			}

			AddPoint( p_svFigure.m_svBottomRight );
			AddPoint( p_svFigure.m_svCenterRight );

			break;
		}
		case SVExtentShapeCircle:
		{
			SVExtentPointStruct l_svPoint;

			double l_dStartAngle = SVGetRotationAngle( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( p_svFigure.m_svTopCenter, p_svFigure.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomRight );

			if ( l_dStartAngle < l_dEndAngle )
			{
				l_dEndAngle -= 360;
			}

			l_dStartAngle += 360.0;

			double l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dInnerRadius;
			double l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svTopCenter );
			AddPoint( p_svFigure.m_svCenterRight );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			for( l_dAngle = l_dMinAngle + l_dLoopStep; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle += l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dInnerRadius + (l_dAngle > l_dMinAngle + 360 ? 4 : 0) , l_dLoopAngle );

				AddPoint( l_svPoint );
			}

			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svBottomLeft );

			l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dOuterRadius;
			l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			l_dLoopAngle = l_dEndAngle;

			for( l_dAngle = l_dMinAngle + l_dLoopStep; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dOuterRadius + (l_dAngle > l_dMinAngle + 360 ? 4 : 0), l_dLoopAngle );

				AddPoint( l_svPoint );
			}

			AddPoint( p_svFigure.m_svBottomRight );
			AddPoint( p_svFigure.m_svCenterRight );

			break;
		}
		case SVExtentShapeDoubleCircle:
		{
			SVExtentPointStruct l_svPoint;

			double l_dStartAngle = SVGetRotationAngle( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( p_svFigure.m_svTopCenter, p_svFigure.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( p_svFigure.m_svTopCenter, p_svFigure.m_svBottomRight );

			if ( l_dStartAngle < l_dEndAngle )
			{
				l_dEndAngle -= 360;
			}

			l_dStartAngle += 360.0;

			double l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dInnerRadius;
			double l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svTopCenter );
			AddPoint( p_svFigure.m_svCenterRight );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			for( l_dAngle = 0; l_dAngle < 360.0; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle += l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dInnerRadius  , l_dLoopAngle );
				AddPoint( l_svPoint );
			}

			for( l_dAngle = 0; l_dAngle < 360.0; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle += l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dInnerRadius +  4 , l_dLoopAngle );
				AddPoint( l_svPoint );
			}

			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svBottomLeft );

			l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dOuterRadius;
			l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			l_dLoopAngle = l_dEndAngle;

			for( l_dAngle = 0 ; l_dAngle < 360.0 ; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dOuterRadius , l_dLoopAngle );
				AddPoint( l_svPoint );
			}

			for( l_dAngle = 0 ; l_dAngle < 360.0 ; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				l_svPoint = SVRotatePoint( p_svFigure.m_svTopCenter, l_dOuterRadius + 4 , l_dLoopAngle );
				AddPoint( l_svPoint );
			}

			AddPoint( p_svFigure.m_svBottomRight );
			AddPoint( p_svFigure.m_svCenterRight );

			break;
		}
		case SVExtentShapeHexagon:
		{
			AddPoint( p_svFigure.m_svTopLeft );
			AddPoint( p_svFigure.m_svTopCenter );
			AddPoint( p_svFigure.m_svTopRight );
			AddPoint( p_svFigure.m_svCenterRight );
			AddPoint( p_svFigure.m_svBottomRight );
			AddPoint( p_svFigure.m_svBottomCenter );
			AddPoint( p_svFigure.m_svBottomLeft );
			AddPoint( p_svFigure.m_svCenterLeft );
			AddPoint( p_svFigure.m_svTopLeft );
			break;
		}
	}
}

void SVDrawObjectClass::SetDrawSinglePoints( bool p_bDrawSinglePoints )
{
	m_bDrawSinglePoints = p_bDrawSinglePoints;
}

bool SVDrawObjectClass::DrawSinglePoints() const
{
	return m_bDrawSinglePoints;
}


BOOL SVDrawObjectClass::Draw( SVDrawContext* PDrawContext )
{
	BOOL BRetVal = FALSE;
	HDC DC = PDrawContext->DC;

	if( beginDraw( PDrawContext ) )
	{
		int l_iSize = calcPoints.GetSize();
		if( m_bDrawSinglePoints )
		{
			for( int i = l_iSize - 1 ; i > -1  ; i-- )
			{
				::SetPixel( DC, calcPoints[i].x, calcPoints[i].y, m_PenColor );
			}
		}
		else
		{
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

		drawPen = NULL;
	}

	m_BUseThisPen = BUseThisPen;
	m_PenStyle = PenStyle; 
	m_PenWidth = PenWidth; 
	m_PenColor = PenColor;

	return TRUE;
}

void SVDrawObjectClass::ClearObject()
{
	points.clear();
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

int SVDrawObjectClass::GetListSize()
{
	return points.GetSize();
}

CPoint SVDrawObjectClass::GetPointAt( int Index )
{
	return points.GetAt(Index);
}

CPoint SVDrawObjectClass::GetCalcPointAt( int Index )
{
	return calcPoints.GetAt(Index);
}

const SVCPointArray& SVDrawObjectClass::GetPointArray()
{
	return points;
}
	
HGDIOBJ SVDrawObjectClass::GetDrawPen()
{ 
	if( m_BUseThisPen && drawPen == NULL )
	{
		drawPen = CreatePen( m_PenStyle, m_PenWidth, m_PenColor );
	}

	return drawPen;
}

// Copy another CPoint Array ( Type: SVCPointArray ) into this draw object.
void SVDrawObjectClass::CopyList( const SVCPointArray& RSrc )
{
	points.Copy( RSrc );
}

COLORREF SVDrawObjectClass::GetPenColor() const
{
	return m_PenColor;
}

BOOL SVDrawObjectClass::beginDraw( SVDrawContext* PDrawContext )
{
	// Set Pen...
	if( m_BUseThisPen && drawPen == NULL )
	{
		drawPen = CreatePen( m_PenStyle, m_PenWidth, m_PenColor );
	}

	if( drawPen )
		oldPen = ::SelectObject( PDrawContext->DC, drawPen );

	calcPoints.SetSize( points.GetSize() );

	// Transform Input points to Output points
	PDrawContext->Transform( points.GetData(), calcPoints.GetData(), points.GetSize() );
	
	return TRUE;

	//return SVCalculateRotation( points.GetData(), calcPoints.GetData(), points.GetSize(), RRotationCenter, RotationAngle, lZoomFactor );
}

void SVDrawObjectClass::endDraw( HDC DC )
{
	// Deselect default pen if exists...
	if( drawPen && oldPen )
		::SelectObject( DC, oldPen );

	if( drawPen )
	{
		DeleteObject( drawPen );
		drawPen = NULL;
	}
}

void SVDrawObjectClass::Transform( SVDrawContext* PDrawContext )
{
	calcPoints.SetSize( points.GetSize() );

	// Transform Input points to Output points
	PDrawContext->Transform( points.GetData(), calcPoints.GetData(), points.GetSize() );
}

