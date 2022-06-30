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
	: DC {Hdc}
{
}

SVDrawContext::SVDrawContext( HDC Hdc, double dScale )
	: DC {Hdc}
	, ScaleFactor {dScale}
{
}

///////////////////////////////////////////////////////////
// Transform a set of points
///////////////////////////////////////////////////////////
void SVDrawContext::Transform(const POINT* const pInPoints, POINT* pOutPoints, int Cnt )
{
	for( int i = 0;i < Cnt;i++ )
	{
		pOutPoints[i] = pInPoints[i];

		ViewPortNormalize( pOutPoints[i] );

		Scale( pOutPoints[i], ScaleFactor );
	}
}

///////////////////////////////////////////////////////////
// Perform Inverse Transform
///////////////////////////////////////////////////////////
void SVDrawContext::InverseTransform(const POINT* const pInPoints, POINT* pOutPoints, int Cnt )
{
	for( int i = 0;i < Cnt;i++ )
	{
		pOutPoints[i] = pInPoints[i];

		Scale (pOutPoints[i], 1.0 / ScaleFactor );

		ViewPortNormalize(pOutPoints[i], FromViewPort);
	}
}

///////////////////////////////////////////////////////////
// Scale a set of points
///////////////////////////////////////////////////////////
void SVDrawContext::Scale( POINT& rPoint, double ScaleValue )
{
	rPoint.x = (long)( (double)rPoint.x * ScaleValue );
	rPoint.y = (long)( (double)rPoint.y * ScaleValue );
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

