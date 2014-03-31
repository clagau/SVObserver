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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDrawObject.cpp_v  $
 * 
 *    Rev 1.3   26 Mar 2014 09:43:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  882
 * SCR Title:  Fix Mask - Zoom bug (e109)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix DrawHatch to handle multiple lines in the same row.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Jan 2014 11:32:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  882
 * SCR Title:  Fix Mask - Zoom bug (e109)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DrawHatch function for drawing mask overlay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 14:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:21:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   27 Jul 2012 08:13:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   08 Nov 2010 14:35:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate container types.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   16 Dec 2009 09:01:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix the include and comment issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   31 Dec 2008 11:34:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  644
   SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added DrawSinglePoints method
   Added GetPenColor method
   Revised assignment operator to preserve drawSinglePoints flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   30 Oct 2007 15:09:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  609
   SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added check to the Draw function to insure the insure that the array is not empty before calling polyline.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   21 Jun 2007 12:27:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   24 Aug 2005 16:09:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed problem with draw pen.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   16 Aug 2005 15:26:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     modified Draw of SVDrawObjectListClass so zoom will work with the overlayed mask figure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   18 May 2005 14:03:18   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Made changes to draw single pixels with a draw object.  To support drawing the mask.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   18 Mar 2005 09:05:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   09 Mar 2005 13:22:46   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  478
   SCR Title:  Add a new Perspective Warp Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Hexagon Shape to AddExtentFigrueData(.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   17 Feb 2005 13:52:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Apr 2003 09:16:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   10 Jan 2003 14:08:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes so the SVDrawObjectClass can work in a consistent manner with SVDrawObjectListClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   19 Nov 2002 12:47:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Cosmetic changes – why “w” Hungarian? Should be “d”
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   04 Oct 2001 14:15:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   Mar 01 2000 11:56:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised to use SVCPointArray type.
   Revised to have virtual destructors.
   Added another Copy( PointArray ) function.
   Removed obsolete comment blocks.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   25 Feb 2000 16:56:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SVDrawObjectClass::beginDraw to use SVDrawContext.
   Revised all calls to beginDraw, passing only the pointer to the draw context.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   16 Feb 2000 16:57:12   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Modified Draw functions to use the DrawContext structure
   and removed unused methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   08 Feb 2000 11:00:50   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  96
   SCR Title:  Scroll/Zoom images displayed in the inspection document.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Modified the functions SVCalculateRotation,
   beginDraw, and all the Draw functions to accept a scaling factor (long
   lZoomFactor) which can be a value from -16 to 16 (except 0). If the calling
   function does not supply this parameter, it defaults to 0 (no scaling).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 11:27:28   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Oct 04 1999 09:20:20   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Put some checking inside to block drawing while loading.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Sep 30 1999 13:38:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  41
   SCR Title:  Version 3.00 Beta 5 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixed sqrt(0) problem.
   Block drawing while loading.
   Completed FriendList Locks.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   Sep 10 1999 17:50:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Bub fix - too many drawing points.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   28 Aug 1999 19:44:00   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Modified Draw Objects to be embedable.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
