//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawContext
//* .File Name       : $Workfile:   SVDrawContext.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Jan 2014 11:30:30  $
//******************************************************************************

#pragma once

enum { FromViewPort, ToViewPort };

class SVDrawContext
{
public:
	HDC DC;					// GDI Device Context
	double ScaleFactor;		// Scale Factor (not used yet - always 1)

	POINT ViewPortOffset;	// View Port Normalization

	long m_lIndex;

public:
	explicit SVDrawContext( HDC Hdc );
	explicit SVDrawContext( HDC Hdc, double dScale );
	void Transform( POINT* PInPoints, POINT* POutPoints, int Cnt );
	void InverseTransform( POINT* PInPoints, POINT* POutPoints, int Cnt );
	void Scale( POINT& RPoint, double ScaleValue );

	// Usage p_lIndex is used with the y component of the first point of a polyline
	// when drawing horizontal lines to skip lines when zoomed in.
	// called from SVDrawObjectList::Draw when used for drawing mask overlays.
	bool ShouldDrawPoints( long& p_lLastPointDrawn, long p_lIndex )
	{
		bool l_bOk = false;

		if( ScaleFactor != 0.0 )
		{
			long lFactor = static_cast<long>((1/ScaleFactor) * 2);
			if( lFactor == 0 )
			{
				l_bOk = true;
			}
			else
			if( ( p_lIndex % lFactor ) == 0 )
			{
				l_bOk = true;
			}
			else if( p_lIndex - p_lLastPointDrawn > p_lIndex % lFactor )
			{
				l_bOk = true;
			}
		}
		else
		{
			l_bOk = true;
		}

		if( l_bOk )
		{
			p_lLastPointDrawn = p_lIndex;
		}
		return l_bOk;
	};

	// for Drawing/Moving/Sizing
	void ViewPortNormalize( POINT& RPoint, BOOL Direction = ToViewPort );

	void Initialize();
};


