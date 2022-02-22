//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawContext
//* .File Name       : $Workfile:   SVDrawContext.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDrawContext.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////
SVDrawContext::SVDrawContext( HDC Hdc )
{
	m_lIndex = 1;

	DC = Hdc;

	ScaleFactor = 1.0;

	ViewPortOffset.x = 0;
	ViewPortOffset.y = 0;
}

SVDrawContext::SVDrawContext( HDC Hdc, double dScale )
{
	m_lIndex = 1;

	DC = Hdc;

	ScaleFactor = dScale;

	ViewPortOffset.x = 0;
	ViewPortOffset.y = 0;
}

///////////////////////////////////////////////////////////
// Transform a set of points
///////////////////////////////////////////////////////////
void SVDrawContext::Transform( POINT* PInPoints, POINT* POutPoints, int Cnt )
{
	for( int i = 0;i < Cnt;i++ )
	{
		POutPoints[i] = PInPoints[i];

		ViewPortNormalize( POutPoints[i] );

		Scale( POutPoints[i], ScaleFactor );
	}
}

///////////////////////////////////////////////////////////
// Perform Inverse Transform
///////////////////////////////////////////////////////////
void SVDrawContext::InverseTransform( POINT* PInPoints, POINT* POutPoints, int Cnt )
{
	for( int i = 0;i < Cnt;i++ )
	{
		POutPoints[i] = PInPoints[i];

		Scale ( POutPoints[i], 1.0 / ScaleFactor );

		ViewPortNormalize( POutPoints[i], FromViewPort );
	}
}

///////////////////////////////////////////////////////////
// Scale a set of points
///////////////////////////////////////////////////////////
void SVDrawContext::Scale( POINT& RPoint, double ScaleValue )
{
	RPoint.x = (long)( (double)RPoint.x * ScaleValue );
	RPoint.y = (long)( (double)RPoint.y * ScaleValue );
}

///////////////////////////////////////////////////////////
// Normalize to/From ViewPort
///////////////////////////////////////////////////////////
void SVDrawContext::ViewPortNormalize( POINT& RPoint, BOOL Direction )
{
	if( Direction == FromViewPort )
	{
		RPoint.x -= ViewPortOffset.x;
		RPoint.y -= ViewPortOffset.y;
	}
	else
	{
		RPoint.x += ViewPortOffset.x;
		RPoint.y += ViewPortOffset.y;
	}
}
void SVDrawContext::Initialize()
{
	m_lIndex = 1;

	// Default is no Viewport normalization
	ViewPortOffset.x = 0;
	ViewPortOffset.y = 0;
}

