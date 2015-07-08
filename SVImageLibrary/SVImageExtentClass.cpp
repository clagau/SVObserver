//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageExtentClass
//* .File Name       : $Workfile:   SVImageExtentClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   24 Sep 2013 15:47:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageExtentClass.h"

#include <math.h>
#include <map>

#include "SVGeometryClass.h"

struct ExtentPropertyMapInit
{
	ExtentPropertyMapInit();
	SVExtentPropertyMapType map;
};

ExtentPropertyMapInit g_svExtentPropertyMap;

ExtentPropertyMapInit::ExtentPropertyMapInit()
{
	map[ SVExtentPropertyPositionPointX ]        = _T("X Position");
	map[ SVExtentPropertyPositionPointY ]        = _T("Y Position");
	//SVExtentPropertyPositionPoint        = 0x00000003,
	map[ SVExtentPropertyTranslationOffsetX ]    = _T("X Offset");
	map[ SVExtentPropertyTranslationOffsetY ]    = _T("Y Offset");
	//SVExtentPropertyTranslationOffset    = 0x0000000C,
	map[ SVExtentPropertyRotationAngle ]         = _T("Rotation Angle");
	map[ SVExtentPropertyOutputPositionPointX ]  = _T("Output X Position");
	map[ SVExtentPropertyOutputPositionPointY ]  = _T("Output Y Position");
	//SVExtentPropertyOutputPositionPoint  = 0x000000C0,

	map[ SVExtentPropertyOldRotationAngle ]      = _T("");
	map[ SVExtentPropertyOldPositionPointX ]     = _T("");
	map[ SVExtentPropertyOldPositionPointY ]     = _T("");
	//SVExtentPropertyOldPositionPoint      = 0x00000300,
	map[ SVExtentPropertyOldAbsPositionPointX ]  = _T("");
	map[ SVExtentPropertyOldAbsPositionPointY ]  = _T("");
	//SVExtentPropertyOldAbsPositionPoint   = 0x00000C00,
	map[ SVExtentPropertyOldRotationPointX ]     = _T("");
	map[ SVExtentPropertyOldRotationPointY ]     = _T("");
	//SVExtentPropertyOldRotationPoint      = 0x00003000,
	map[ SVExtentPropertyOldTranslationOffsetX ] = _T("");
	map[ SVExtentPropertyOldTranslationOffsetY ] = _T("");
	//SVExtentPropertyOldTranslationOffset  = 0x0000C000,
	
	//map[ SVExtentPropertyPoint ]        = _T("Point");
	//map[ SVExtentPropertyLine ]         = _T("Line");
	//map[ SVExtentPropertyRectangle ]    = _T("Rectangle");
	//map[ SVExtentPropertyPie ]          = _T("Pie");
	map[ SVExtentPropertyWidth ]        = _T("Width");
	map[ SVExtentPropertyHeight ]       = _T("Height");
	map[ SVExtentPropertyStartAngle ]   = _T("Start Angle");
	map[ SVExtentPropertyEndAngle ]     = _T("End Angle");
	map[ SVExtentPropertyInnerRadius ]  = _T("Inner Radius");
	map[ SVExtentPropertyOuterRadius ]  = _T("Outer Radius");
	map[ SVExtentPropertyOutputWidth ]  = _T("Width");
	map[ SVExtentPropertyOutputHeight ] = _T("Height");
};

CString SVImageExtentClass::GetExtentPropertyName( SVExtentPropertyEnum p_eProperty )
{
	return g_svExtentPropertyMap.map[p_eProperty];
}

double SVGetRadius( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint )
{
	double l_dRadius = 0.0;

	double l_dX = p_svPoint.m_dPositionX - p_svCenter.m_dPositionX;
	double l_dY = p_svPoint.m_dPositionY - p_svCenter.m_dPositionY;

	l_dRadius = sqrt( ( l_dX * l_dX ) + ( l_dY * l_dY ) );

	return l_dRadius;
}

double SVGetRotationAngle( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint )
{
	double l_dAngle = 0.0;

	double l_dX = p_svPoint.m_dPositionX - p_svCenter.m_dPositionX;
	double l_dY = p_svPoint.m_dPositionY - p_svCenter.m_dPositionY;

	if ( l_dX == 0.0 )
	{
		if ( l_dY < 0.0 )
		{
			l_dAngle = 270.0;
		}
		else
		{
			l_dAngle = 90.0;
		}
	}
	else if ( l_dY == 0.0 )
	{
		if ( l_dX < 0.0 )
		{
			l_dAngle = 180.0;
		}
		else
		{
			l_dAngle = 0.0;
		}
	}
	else
	{
		l_dAngle = g_svPi.GetArcTanInDegrees( l_dY, l_dX );

		if ( l_dAngle < 0.0 )
		{
			l_dAngle += 360.0;
		}
	}

	return l_dAngle;
}

SVExtentPointStruct SVRotatePoint( SVExtentPointStruct p_svCenter, double p_dRadius, double p_dAngle )
{
	SVExtentPointStruct l_svPoint;

	double l_dXCos = g_svPi.GetCosFromDegrees( p_dAngle );
	double l_dYSin = g_svPi.GetSinFromDegrees( p_dAngle );

	l_svPoint.m_dPositionX = p_svCenter.m_dPositionX + p_dRadius * l_dXCos;
	l_svPoint.m_dPositionY = p_svCenter.m_dPositionY + p_dRadius * l_dYSin;

	return l_svPoint;
}

SVExtentPointStruct SVRotatePoint( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svStart, double p_dAngle )
{
	double l_dRadius = SVGetRadius( p_svCenter, p_svStart );
	double l_dAngle = SVGetRotationAngle( p_svCenter, p_svStart ) + p_dAngle;

	return SVRotatePoint( p_svCenter, l_dRadius, l_dAngle );
}

double SVGetFlippedRotationAngle( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint )
{
	double l_dAngle = 0.0;

	double l_dX = p_svPoint.m_dPositionX - p_svCenter.m_dPositionX;
	double l_dY = - ( p_svPoint.m_dPositionY - p_svCenter.m_dPositionY );

	if ( l_dX == 0.0 )
	{
		if ( l_dY < 0.0 )
		{
			l_dAngle = 270.0;
		}
		else
		{
			l_dAngle = 90.0;
		}
	}
	else if ( l_dY == 0.0 )
	{
		if ( l_dX < 0.0 )
		{
			l_dAngle = 180.0;
		}
		else
		{
			l_dAngle = 0.0;
		}
	}
	else
	{
		l_dAngle = g_svPi.GetArcTanInDegrees( l_dY, l_dX );

		if ( l_dAngle < 0.0 )
		{
			l_dAngle += 360.0;
		}
	}

	return l_dAngle;
}

SVExtentPointStruct SVRotateAndFlipPoint( SVExtentPointStruct p_svCenter, double p_dRadius, double p_dAngle )
{
	SVExtentPointStruct l_svPoint;

	double l_dXCos = g_svPi.GetCosFromDegrees( p_dAngle );
	double l_dYSin = g_svPi.GetSinFromDegrees( p_dAngle );

	l_svPoint.m_dPositionX = p_svCenter.m_dPositionX + p_dRadius * l_dXCos;
	l_svPoint.m_dPositionY = p_svCenter.m_dPositionY - p_dRadius * l_dYSin;

	return l_svPoint;
}

SVExtentPointStruct SVRotateAndFlipPoint( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svStart, double p_dAngle )
{
	double l_dRadius = SVGetRadius( p_svCenter, p_svStart );
	double l_dAngle = SVGetFlippedRotationAngle( p_svCenter, p_svStart ) + p_dAngle;

	return SVRotateAndFlipPoint( p_svCenter, l_dRadius, l_dAngle );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVImageExtentClass::SVImageExtentClass()
{
	Initialize();
}

SVImageExtentClass::SVImageExtentClass( const SVImageExtentClass &p_rsvValue )
{
	Initialize();

	*this = p_rsvValue;
}

SVImageExtentClass::~SVImageExtentClass()
{
	Initialize();
}

const SVImageExtentClass &SVImageExtentClass::operator=( const SVImageExtentClass &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		m_eTranslation = p_rsvValue.m_eTranslation;
		m_svPosition = p_rsvValue.m_svPosition;
		m_svDimensions = p_rsvValue.m_svDimensions;
		m_svFigure = p_rsvValue.m_svFigure;
	}
	return *this;
}

HRESULT SVImageExtentClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_eTranslation = SVExtentTranslationUnknown;

	if ( m_svPosition.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svDimensions.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( ClearOutputData() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVExtentTranslationEnum SVImageExtentClass::GetTranslation() const
{
	return m_eTranslation;
}

HRESULT SVImageExtentClass::SetTranslation( SVExtentTranslationEnum p_eTranslation )
{
	HRESULT l_hrOk = S_FALSE;

	switch( p_eTranslation )
	{
		case SVExtentTranslationUnknown:
		case SVExtentTranslationNone:
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationPolarUnwrap:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationCylindricalWarpH:
		case SVExtentTranslationCylindricalWarpV:
		case SVExtentTranslationVerticalPerspective:
		case SVExtentTranslationHorizontalPerspective:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFigureShift:
		{
			m_eTranslation = p_eTranslation;

			l_hrOk = S_OK;

			break;
		}
	}

	if( ClearOutputData() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

const SVExtentDimensionsClass &SVImageExtentClass::GetDimensions() const
{
	return m_svDimensions;
}

HRESULT SVImageExtentClass::SetDimensions( SVExtentDimensionsClass p_svDimensions )
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationNone:
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFigureShift:
		{
			m_svDimensions = p_svDimensions;
			l_hrOk = ClearOutputData();
			break;
		}
		case SVExtentTranslationPolarUnwrap:
		{
			m_svDimensions = p_svDimensions;

			double dEndAngle = 0.0;
			if ( S_OK == GetExtentProperty( SVExtentPropertyEndAngle, dEndAngle ))
			{
				l_hrOk = SetExtentProperty( SVExtentPropertyEndAngle, dEndAngle );
			}
			l_hrOk = ClearOutputData();
			break;
		}
	}

	return l_hrOk;
}

const SVExtentPositionClass &SVImageExtentClass::GetPosition() const
{
	return m_svPosition;
}

HRESULT SVImageExtentClass::SetPosition( SVExtentPositionClass p_svPosition )
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationNone:
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFigureShift:
		{
			m_svPosition = p_svPosition;
			l_hrOk = ClearOutputData();
			break;
		}
		case SVExtentTranslationPolarUnwrap:
		{
			m_svPosition = p_svPosition;
			double dRotationAngle = 0.0;
			if ( S_OK  == GetExtentProperty( SVExtentPropertyRotationAngle, dRotationAngle ))
			{
				l_hrOk = SetExtentProperty( SVExtentPropertyRotationAngle, dRotationAngle );
			}
			if ( S_OK == l_hrOk )
			{
				l_hrOk = ClearOutputData();
			}
			break;
		}
	}
	return l_hrOk;
}

HRESULT SVImageExtentClass::ClearOldProperties()
{
	return m_svPosition.DisableExtentProperty( SVExtentPropertyOldPositionsAll );
}

HRESULT SVImageExtentClass::DisableExtentProperty( SVExtentPropertyEnum p_eProperty )
{
	HRESULT l_hrOk = m_svPosition.DisableExtentProperty( p_eProperty );

	if ( m_svDimensions.DisableExtentProperty( p_eProperty ) == S_OK )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	double l_dValue = 0.0;

	l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( l_hrOk == S_OK )
	{
		p_rlValue = (long)l_dValue;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	if( ( p_eProperty & ~( SVExtentPropertyPositionsAll ) ) == 0 )
	{
		l_hrOk = m_svPosition.GetExtentProperty( p_eProperty, p_rdValue );
	}
	else if( ( p_eProperty & ~( SVExtentPropertyDimensionsAll ) ) == 0 )
	{
		l_hrOk = m_svDimensions.GetExtentProperty( p_eProperty, p_rdValue );
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svValue;

	l_hrOk = GetExtentProperty( p_eProperty, l_svValue );

	if ( l_hrOk == S_OK )
	{
		p_roValue = l_svValue;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svPosition.GetExtentProperty( p_eProperty, p_rsvValue );

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentPropertyList( SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyMapType& p_rPropertyMap ) const
{
	p_rPropertyMap.clear();
	const SVExtentPropertyMapType& rMap = g_svExtentPropertyMap.map;

	SVExtentPropertyMapType::const_iterator iter;
	for ( iter = rMap.begin(); iter != rMap.end(); ++iter )
	{
		bool bPositionEnabled   = m_svPosition  .IsEnabled( (SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		bool bDimensionsEnabled = m_svDimensions.IsEnabled( (SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		if ( bPositionEnabled || bDimensionsEnabled )
			p_rPropertyMap.insert( *iter );
	}
	return S_OK;
}

HRESULT SVImageExtentClass::GetExtentPropertyList( SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyListType& p_rPropertyList ) const
{
	const SVExtentPropertyMapType& rMap = g_svExtentPropertyMap.map;

	SVExtentPropertyMapType::const_iterator iter;
	for ( iter = rMap.begin(); iter != rMap.end(); ++iter )
	{
		bool bPositionEnabled   = m_svPosition  .IsEnabled( (SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		bool bDimensionsEnabled = m_svDimensions.IsEnabled( (SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		if ( bPositionEnabled || bDimensionsEnabled )
			p_rPropertyList.push_back( iter->first );
	}
	return S_OK;
}

HRESULT SVImageExtentClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPropertyEnum l_ePosProperty = (SVExtentPropertyEnum)(p_eProperty & SVExtentPropertyPositionsAll);
	SVExtentPropertyEnum l_eDimProperty = (SVExtentPropertyEnum)(p_eProperty & SVExtentPropertyDimensionsAll);

	if( l_ePosProperty != 0 )
	{
		l_hrOk = S_OK;

		// special case to handle polar unwrap
		if ( m_eTranslation == SVExtentTranslationPolarUnwrap )
		{
			if ( p_eProperty & SVExtentPropertyRotationAngle )
			{
				double dEndAngle = 0.0;
				double dStartAngle = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyEndAngle, dEndAngle );
				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, dStartAngle );

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyStartAngle,
					                                     dStartAngle + (p_dValue - dEndAngle) );
				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyEndAngle, p_dValue );

			}
		}

		HRESULT hrSet = m_svPosition.SetExtentProperty( l_ePosProperty, p_dValue );
		if ( l_hrOk == S_OK )
			l_hrOk = hrSet;
	}

	if( l_eDimProperty != 0 )
	{
		// special case to handle polar unwrap rotation
		if ( m_eTranslation == SVExtentTranslationPolarUnwrap )
		{
			if ( p_eProperty & SVExtentPropertyEndAngle )
			{
				HRESULT hrSet = m_svPosition.SetExtentProperty( SVExtentPropertyRotationAngle, p_dValue );
			}
		}

		if ( m_svDimensions.SetExtentProperty( l_eDimProperty, p_dValue ) == S_OK )
		{
			if ( l_ePosProperty == 0 )
			{
				l_hrOk = S_OK;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if ( ClearOutputData() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svPosition.SetExtentProperty( p_eProperty, p_svValue );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = ClearOutputData();
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::UpdateData()
{
	HRESULT l_hrOk = BuildOutputDimensions();

	if ( l_hrOk == S_OK )
	{
		l_hrOk = BuildFigure();
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetFigure( SVExtentFigureStruct &p_rsvFigure ) const
{
	HRESULT l_hrOk = p_rsvFigure.Initialize();

	if ( l_hrOk == S_OK )
	{
		switch( m_svFigure.m_eShape )
		{
			case SVExtentShapePoint:
			case SVExtentShapeArrow:
			case SVExtentShapeLine:
			case SVExtentShapeLineHorizontal:
			case SVExtentShapeLineVertical:
			case SVExtentShapeRectangle:
			case SVExtentShapePie:
			case SVExtentShapeCircle:
			case SVExtentShapeDoubleCircle:
			case SVExtentShapeHexagon:
			{
				p_rsvFigure = m_svFigure;

				l_hrOk = S_OK;

				break;
			}
			default:
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

SVExtentLocationPropertyEnum SVImageExtentClass::GetLocationPropertyAt( SVExtentPointStruct p_svPoint )
{
	SVExtentLocationPropertyEnum l_eLocation = SVExtentLocationPropertyUnknown;

	SVExtentPointStruct l_svOutputPoint;

	RECT l_oRect;
	bool l_bValid = false;

	if ( m_eTranslation == SVExtentTranslationPolarUnwrap )
	{
		double l_dValue;
		m_svPosition.GetExtentProperty( SVExtentPropertyPositionPointX, l_dValue);
		l_oRect.left = static_cast< long >( l_dValue - 2 );
		l_oRect.right = static_cast< long >( l_dValue + 2 );
		m_svPosition.GetExtentProperty( SVExtentPropertyPositionPointY, l_dValue);
		l_oRect.top = static_cast< long >( l_dValue - 2 );
		l_oRect.bottom = static_cast< long >( l_dValue + 2 );
		if ( ::PtInRect( &l_oRect, p_svPoint ) )
		{
			l_eLocation = SVExtentLocationPropertyCenter;
		}
	}

	switch( m_eTranslation )
	{
		case SVExtentTranslationResize:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFigureShift:
		{
			l_svOutputPoint = p_svPoint;
			l_bValid = GetRectangle( l_oRect ) == S_OK;
			break;
		}

		default:
		{
			l_bValid = TranslateToOutputSpace( p_svPoint, l_svOutputPoint ) == S_OK &&
				GetOutputRectangle( l_oRect ) == S_OK;
		}
	}

	if ( l_bValid )
	{
		RECT l_oTemp = l_oRect;

		// Check special case for Polar Unwrap tool when > 360
		if ( l_eLocation == SVExtentLocationPropertyUnknown && m_eTranslation == SVExtentTranslationPolarUnwrap )
		{
			double l_dStartAngle, l_dEndAngle, l_dOuterRadius, l_ldInnerRadius;

			if( GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle) == S_OK &&
			GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle) == S_OK &&
			GetExtentProperty( SVExtentPropertyInnerRadius, l_ldInnerRadius ) == S_OK &&
			l_dEndAngle - l_dStartAngle > 360 )
			{
				GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius );
				double l_dCircum =  g_svPi.GetCircumference( l_dOuterRadius );
				if( l_oRect.right > l_dCircum && l_oRect.right > 4 )
				{
					l_oTemp.right = l_oRect.right - (long)l_dCircum;
					l_oTemp.left = l_oTemp.right;
					l_oTemp.top = (long)-l_ldInnerRadius;
					if ( ::InflateRect( &l_oTemp, 2, -2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SVExtentLocationPropertyRight;
					}
				}
			}
		}


		long l_lInflateX = -2;
		long l_lInflateY = -2;

		if ( l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;

			if ( l_oRect.bottom - 4 < 9 )
			{
				l_lInflateY = 1;
			}
			
			if ( l_oRect.right - 4 < 9 )
			{
				l_lInflateX = 1;
			}

			if ( ::InflateRect( &l_oTemp, l_lInflateX, l_lInflateY ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				if( m_eTranslation == SVExtentTranslationRotate ||
				    m_eTranslation == SVExtentTranslationFlippedRotate ||
				    m_eTranslation == SVExtentTranslationProfile )
				{
					l_oTemp = l_oRect;
				
					if ( l_lInflateY < 1 )
					{
						l_lInflateY = 2;
					}

					if ( l_oRect.right < 1 )
					{
						l_lInflateX = 0;

						l_oTemp.left = l_oRect.right + 1;
						l_oTemp.right = l_oRect.right + 1;
					}
					else if ( l_oRect.right < 9 )
					{
						l_lInflateX = 1;

						l_oTemp.left = l_oRect.right;
						l_oTemp.right = l_oRect.right;
					}
					else if ( l_oRect.right < 16 )
					{
						l_lInflateX = 1;

						l_oTemp.left = l_oRect.right - 2;
						l_oTemp.right = l_oRect.right - 2;
					}
					else
					{
						l_lInflateX = 2;
						//Use a quarter of the width for the rotating pick area
						l_oTemp.left = l_oRect.right - (abs(l_oRect.right - l_oRect.left)/4);
						l_oTemp.right = l_oRect.right - 5;
					}

					l_oTemp.top = l_oRect.bottom / 2;
					l_oTemp.bottom = l_oRect.bottom / 2;

					if ( ::InflateRect( &l_oTemp, l_lInflateX, l_lInflateY ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SVExtentLocationPropertyRotate;
					}
				}

				if ( l_eLocation == SVExtentLocationPropertyUnknown )
				{
					l_eLocation = SVExtentLocationPropertyCenter;
				}
			}
		}

		if ( m_eTranslation != SVExtentTranslationLine &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.bottom = l_oRect.top;

			l_lInflateX = -2;

			if ( l_oRect.right < 9 )
			{
				l_oTemp.right = l_oRect.left;

				l_lInflateX = 2;
			}

			if ( ::InflateRect( &l_oTemp, l_lInflateX, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyTop;
			}
		}

		if ( l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.left = l_oRect.right;

			l_lInflateY = -2;

			if ( l_oRect.bottom < 9 )
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateY = 2;
			}

			if ( ::InflateRect( &l_oTemp, 2, l_lInflateY ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyRight;
			}
		}

		if ( m_eTranslation != SVExtentTranslationLine &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.top = l_oRect.bottom;

			l_lInflateX = -2;

			if ( l_oRect.right < 9 )
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateX = 2;
			}

			if ( ::InflateRect( &l_oTemp, l_lInflateX, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyBottom;
			}
		}

		if ( m_eTranslation != SVExtentTranslationProfile &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.right = l_oRect.left;

			l_lInflateY = -2;

			if ( l_oRect.bottom < 9 )
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateY = 2;
			}

			if ( ::InflateRect( &l_oTemp, 2, l_lInflateY ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyLeft;
			}
		}

		if ( m_eTranslation != SVExtentTranslationProfile &&
		     m_eTranslation != SVExtentTranslationLine &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.right = l_oRect.left;
			l_oTemp.bottom = l_oRect.top;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyTopLeft;
			}
		}

		if ( m_eTranslation != SVExtentTranslationLine &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.left = l_oRect.right;
			l_oTemp.bottom = l_oRect.top;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyTopRight;
			}
		}

		if ( m_eTranslation != SVExtentTranslationProfile &&
		     m_eTranslation != SVExtentTranslationLine &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.right = l_oRect.left;
			l_oTemp.top = l_oRect.bottom;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyBottomLeft;
			}
		}

		if ( m_eTranslation != SVExtentTranslationLine &&
			   l_eLocation == SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.left = l_oRect.right;
			l_oTemp.top = l_oRect.bottom;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SVExtentLocationPropertyBottomRight;
			}
		}

		if ( m_eTranslation == SVExtentTranslationPolarUnwrap )
		{
			long l_lInnerRadius = 0;

			if ( GetExtentProperty( SVExtentPropertyInnerRadius, l_lInnerRadius ) == S_OK )
			{
				if ( l_eLocation == SVExtentLocationPropertyUnknown )
				{
					if ( 0 < l_oRect.right - 4 && 0 < l_lInnerRadius - 4 )
					{
						l_oTemp = l_oRect;
						l_oTemp.bottom = 0;
						l_oTemp.top = -l_lInnerRadius;
						l_svOutputPoint.m_dPositionX =  l_svOutputPoint.m_dPositionX ;

						if ( ::InflateRect( &l_oTemp, -2, -2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
						{
							l_eLocation = SVExtentLocationPropertyCenter;
						}
					}
				}

				if ( l_eLocation == SVExtentLocationPropertyUnknown )
				{
					l_oTemp = l_oRect;
				
					l_oTemp.bottom = -l_lInnerRadius;
					l_oTemp.top = -l_lInnerRadius;

					if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SVExtentLocationPropertyCenter;
					}
				}

				if ( m_eTranslation != SVExtentTranslationProfile &&
				     l_eLocation == SVExtentLocationPropertyUnknown )
				{
					l_oTemp = l_oRect;
				
					l_oTemp.right = l_oRect.left;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if ( ::InflateRect( &l_oTemp, 2, 0 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SVExtentLocationPropertyLeft;
					}
				}

				if ( l_eLocation == SVExtentLocationPropertyUnknown )
				{
					l_oTemp = l_oRect;
				
					l_oTemp.left = l_oRect.right;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if ( ::InflateRect( &l_oTemp, 2, 0 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SVExtentLocationPropertyRight;
					}
				}
			}
		}
		if ( m_eTranslation == SVExtentTranslationPolarUnwrap )
		{
			double l_dOuterRadius, l_ldInnerRadius;
			if( l_eLocation == SVExtentLocationPropertyTop ||
				l_eLocation == SVExtentLocationPropertyBottom )
			{
				if( GetExtentProperty( SVExtentPropertyInnerRadius, l_ldInnerRadius ) == S_OK &&
					GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius ) == S_OK )
				{
					if( l_dOuterRadius < l_ldInnerRadius )
					{
						if( l_eLocation == SVExtentLocationPropertyTop )
						{
							l_eLocation = SVExtentLocationPropertyBottom;
						}
						else
						{
							l_eLocation = SVExtentLocationPropertyTop;
						}
					}
				}
			}
		}
		if( m_eTranslation == SVExtentTranslationVerticalPerspective )
		{
			if( l_eLocation == SVExtentLocationPropertyTopRight ||
				l_eLocation == SVExtentLocationPropertyTopLeft)
			{
				l_eLocation = SVExtentLocationPropertyTop;
			}
			if( l_eLocation == SVExtentLocationPropertyBottomRight ||
				l_eLocation == SVExtentLocationPropertyBottomLeft )
			{
				l_eLocation = SVExtentLocationPropertyBottom;
			}
			if( l_eLocation == SVExtentLocationPropertyCenter )
			{
				l_eLocation = SVExtentLocationPropertyUnknown;
			}
		}
		if( m_eTranslation == SVExtentTranslationHorizontalPerspective )
		{
			if( l_eLocation == SVExtentLocationPropertyTopRight ||
				l_eLocation == SVExtentLocationPropertyBottomRight )
			{
				l_eLocation = SVExtentLocationPropertyRight;
			}
			if( l_eLocation == SVExtentLocationPropertyTopLeft ||
				l_eLocation == SVExtentLocationPropertyBottomLeft )
			{
				l_eLocation = SVExtentLocationPropertyLeft;
			}
			if( l_eLocation == SVExtentLocationPropertyCenter )
			{
				l_eLocation = SVExtentLocationPropertyUnknown;
			}
		}
	}

	return l_eLocation;
}

// Translate the point to be relative (local space) 
HRESULT SVImageExtentClass::TranslateToLocalSpace(const SVExtentPointStruct& rValue, SVExtentPointStruct& rResult)
{
	SVExtentPointStruct svPosition;
	HRESULT hr = m_svPosition.GetExtentProperty(SVExtentPropertyPositionPoint, svPosition);

	if (S_OK == hr)
	{
		rResult.m_dPositionX = rValue.m_dPositionX - svPosition.m_dPositionX;
		rResult.m_dPositionY = rValue.m_dPositionY - svPosition.m_dPositionY;
		
		// optional...
		SVExtentPointStruct svDisplacement;
		if (S_OK == m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, svDisplacement))
		{
			rResult.m_dPositionX -= svDisplacement.m_dPositionX;
			rResult.m_dPositionY -= svDisplacement.m_dPositionY;
		}
	}
	return hr;
}

HRESULT SVImageExtentClass::Update( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_eTranslation == SVExtentTranslationPolarUnwrap )
	{
		l_hrOk = UpdatePolar( p_eLocation, p_svStart, p_svEnd );
	}
	else if ( m_eTranslation == SVExtentTranslationLine )
	{
		l_hrOk = UpdateLine( p_eLocation, p_svStart, p_svEnd );
	}
	else if( m_eTranslation == SVExtentTranslationVerticalPerspective )
	{
		l_hrOk = UpdateVerticalPerspective( p_eLocation, p_svStart, p_svEnd );
	}
	else if( m_eTranslation == SVExtentTranslationHorizontalPerspective )
	{
		l_hrOk = UpdateHorizontalPerspective( p_eLocation, p_svStart, p_svEnd );
	}
	else
	{
		SVExtentPointStruct l_svOutputStart;
		SVExtentPointStruct l_svOutputEnd;

		RECT l_oRect;

		bool l_bValid = false;
		switch( m_eTranslation )
		{
			case SVExtentTranslationResize:
			{
				//@WARNING [Jim][8 July 2015] No identification of error cases.
				// GetExtentProperty () only l_bValid is identified, which is 
				// not very helpful. Currently none of these cases give much 
				// help with error identification.
				l_bValid = (S_OK == TranslateToLocalSpace(p_svStart, l_svOutputStart)) &&
								(S_OK == TranslateToLocalSpace(p_svEnd, l_svOutputEnd)) &&
								(S_OK == GetRectangle(l_oRect));
				if (l_bValid)
				{
					if (p_eLocation == SVExtentLocationPropertyBottomLeft)
					{
						l_svOutputEnd.m_dPositionY = p_svEnd.m_dPositionY;
					}
					else if (p_eLocation == SVExtentLocationPropertyTopRight)
					{
						l_svOutputEnd.m_dPositionX = p_svEnd.m_dPositionX;
					}
					else if (p_eLocation != SVExtentLocationPropertyTop &&
						p_eLocation != SVExtentLocationPropertyLeft &&
						p_eLocation != SVExtentLocationPropertyTopLeft)
					{	
						l_svOutputStart = p_svStart;
						l_svOutputEnd = p_svEnd;
					}
				}
				break;
			}
			// Note: These cases do not work correctly (cannot drag the left or top using the mouse in the gui), 
			// a possible solution might be to use the same logic as the case above (SVExtentTranslationResize)
			// however for these cases the position (not on the GUI) is always fixed to 0,0, so that might not 
			// work. 
			case SVExtentTranslationDoubleHeight:
			case SVExtentTranslationFlipHorizontal:
			case SVExtentTranslationFlipVertical:
			case SVExtentTranslationFigureShift:
			{
				l_svOutputStart = p_svStart;
				l_svOutputEnd = p_svEnd;
				l_bValid = GetRectangle( l_oRect ) == S_OK;
				break;
			}

			default:
			{
				l_bValid = TranslateToOutputSpace( p_svStart, l_svOutputStart ) == S_OK &&
					TranslateToOutputSpace( p_svEnd, l_svOutputEnd ) == S_OK &&
					GetOutputRectangle( l_oRect ) == S_OK;
			}
		}

		if ( l_bValid )
		{
			switch( p_eLocation )
			{
				case SVExtentLocationPropertyTopLeft:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SVExtentLocationPropertyTopRight:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX - l_oRect.right, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SVExtentLocationPropertyBottomRight:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX - l_oRect.right, (long)l_svOutputEnd.m_dPositionY - l_oRect.bottom );

					break;
				}
				case SVExtentLocationPropertyBottomLeft:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY - l_oRect.bottom );

					break;
				}
				case SVExtentLocationPropertyLeft:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SVExtentLocationPropertyRight:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX - l_oRect.right, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SVExtentLocationPropertyTop:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SVExtentLocationPropertyBottom:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY - l_oRect.bottom );

					break;
				}
				case SVExtentLocationPropertyCenter:
				{
					long l_lX = (long)( p_svEnd.m_dPositionX - p_svStart.m_dPositionX );
					long l_lY = (long)( p_svEnd.m_dPositionY - p_svStart.m_dPositionY );

					l_hrOk = UpdateFromOutputSpace( p_eLocation, l_lX, l_lY );

					break;
				}
				case SVExtentLocationPropertyRotate:
				{
					SVExtentPointStruct l_svPosition;

					if ( GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK )
					{
						double l_dAngle = SVGetRotationAngle( l_svPosition, p_svEnd );

						if ( SetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle ) == S_OK )
						{
							l_hrOk = UpdateData();
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}

					break;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::UpdateFromOutputSpace( SVExtentLocationPropertyEnum p_eLocation, long p_dX, long p_dY )
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationLine:
		{
			SVExtentPointStruct l_svStart;
			SVExtentPointStruct l_svEnd;

			if ( GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart ) == S_OK &&
			     GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd ) == S_OK )
			{
				switch( p_eLocation )
				{
					case SVExtentLocationPropertyTop:
					{
						l_svStart.m_dPositionX += p_dX;
						l_svStart.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyLeft:
					{
						l_svStart.m_dPositionX += p_dX;
						l_svStart.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyBottom:
					{
						l_svEnd.m_dPositionX += p_dX;
						l_svEnd.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyRight:
					{
						l_svEnd.m_dPositionX += p_dX;
						l_svEnd.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyCenter:
					{
						l_svStart.m_dPositionX += p_dX;
						l_svStart.m_dPositionY += p_dY;

						l_svEnd.m_dPositionX += p_dX;
						l_svEnd.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
				}

				if ( l_hrOk == S_OK )
				{
					if ( SetExtentProperty( SVExtentPropertyPositionPoint, l_svStart ) == S_OK &&
							 SetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd ) == S_OK )
					{
						l_hrOk = UpdateData();
					}
					else
					{
						l_hrOk = S_FALSE;
					}
				}
			}

			break;
		}
		case SVExtentTranslationProfile:
		{
			SVExtentPointStruct l_svPosition;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if ( GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
					 GetExtentProperty( SVExtentPropertyHeight, l_dHeight ) == S_OK &&
					 GetExtentProperty( SVExtentPropertyWidth, l_dWidth ) == S_OK )
			{
				switch( p_eLocation )
				{
					case SVExtentLocationPropertyTopRight:
					{
						l_dWidth += p_dX;
						l_dHeight -= p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += p_dX;
						l_dHeight += p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyRight:
					{
						l_dWidth += p_dX;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyTop:
					{
						l_dHeight -= p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyBottom:
					{
						l_dHeight += p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyCenter:
					{
						l_svPosition.m_dPositionX += p_dX;
						l_svPosition.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
				}

				if ( l_hrOk == S_OK )
				{
					if ( 0.0 < l_dWidth && 0.0 < l_dHeight &&
							 SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
							 SetExtentProperty( SVExtentPropertyHeight, l_dHeight ) == S_OK &&
							 SetExtentProperty( SVExtentPropertyWidth, l_dWidth ) == S_OK )
					{
						l_hrOk = UpdateData();
					}
					else
					{
						l_hrOk = S_FALSE;
					}
				}
			}

			break;
		}
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFigureShift:
		{
			SVExtentPointStruct l_svPosition;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if ( GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
					 GetExtentProperty( SVExtentPropertyHeight, l_dHeight ) == S_OK &&
					 GetExtentProperty( SVExtentPropertyWidth, l_dWidth ) == S_OK )
			{
				switch( p_eLocation )
				{
					case SVExtentLocationPropertyTopLeft:
					{
						l_svPosition.m_dPositionX += p_dX;
						l_svPosition.m_dPositionY += p_dY;

						l_dWidth -= p_dX;
						l_dHeight -= p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyTopRight:
					{
						l_svPosition.m_dPositionY += p_dY;

						l_dWidth += p_dX;
						l_dHeight -= p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += p_dX;
						l_dHeight += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyBottomLeft:
					{
						l_svPosition.m_dPositionX += p_dX;

						l_dWidth -= p_dX;
						l_dHeight += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyLeft:
					{
						l_svPosition.m_dPositionX += p_dX;

						l_dWidth -= p_dX;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyRight:
					{
						l_dWidth += p_dX;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyTop:
					{
						l_svPosition.m_dPositionY += p_dY;

						l_dHeight -= p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyBottom:
					{
						l_dHeight += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SVExtentLocationPropertyCenter:
					{
						l_svPosition.m_dPositionX += p_dX;
						l_svPosition.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
				}

				if ( l_hrOk == S_OK )
				{
					if ( 0.0 < l_dWidth && 0.0 < l_dHeight &&
							 SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
							 SetExtentProperty( SVExtentPropertyHeight, l_dHeight ) == S_OK &&
							 SetExtentProperty( SVExtentPropertyWidth, l_dWidth ) == S_OK )
					{
						l_hrOk = UpdateData();
					}
					else
					{
						l_hrOk = S_FALSE;
					}
				}
			}

			break;
		}

		case SVExtentTranslationPolarUnwrap:
		{
			l_hrOk = UpdatePolarFromOutputSpace( p_eLocation, p_dX, p_dY );
			break;
		}
		case SVExtentTranslationVerticalPerspective:
		{
			switch( p_eLocation )
			{
				case SVExtentLocationPropertyTop:
				case SVExtentLocationPropertyBottom:
				case SVExtentLocationPropertyCenter:
				{
					SVExtentPointStruct l_svPosition;

					if ( GetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK)
					{
						l_svPosition.m_dPositionY += p_dY;
						l_hrOk = S_OK;
					}

					if ( l_hrOk == S_OK )
					{
						if ( SetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK )
						{
							l_hrOk = UpdateData();
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}

					break;
				}
			}
			break;
		}
		case SVExtentTranslationHorizontalPerspective:
		{
			switch( p_eLocation )
			{
				case SVExtentLocationPropertyLeft:
				case SVExtentLocationPropertyCenter:
				case SVExtentLocationPropertyRight:
				{
					SVExtentPointStruct l_svPosition;

					if ( GetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK)
					{
						l_svPosition.m_dPositionX += p_dX;
						l_hrOk = S_OK;
					}

					if ( l_hrOk == S_OK )
					{
						if ( SetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK )
						{
							l_hrOk = UpdateData();
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}

					break;
				}
			}
			break;
		}

	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetRectangle( RECT &p_roRect ) const
{
	HRESULT l_hrOk = S_FALSE;

	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	l_hrOk = GetExtentProperty( SVExtentPropertyPositionPointX, l_oRect.left );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = GetExtentProperty( SVExtentPropertyPositionPointY, l_oRect.top );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = GetExtentProperty( SVExtentPropertyWidth, l_oRect.right );

		l_oRect.right += l_oRect.left;
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = GetExtentProperty( SVExtentPropertyHeight, l_oRect.bottom );

		l_oRect.bottom += l_oRect.top;
	}

	if ( l_hrOk == S_OK )
	{
		p_roRect = l_oRect;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetLogicalRectangle( RECT &p_roRect ) const
{
	HRESULT l_hrOk = S_FALSE;

	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	l_hrOk = GetExtentProperty( SVExtentPropertyWidth, l_oRect.right );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = GetExtentProperty( SVExtentPropertyHeight, l_oRect.bottom );
	}

	if ( l_hrOk == S_OK )
	{
		p_roRect = l_oRect;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetOutputRectangle( RECT &p_roRect ) const
{
	HRESULT l_hrOk = S_FALSE;

	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	l_hrOk = GetExtentProperty( SVExtentPropertyOutputWidth, l_oRect.right );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = GetExtentProperty( SVExtentPropertyOutputHeight, l_oRect.bottom );
	}

	if ( l_hrOk == S_OK )
	{
		p_roRect = l_oRect;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateToOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult )
{
	HRESULT l_hrOk = p_rsvResult.Initialize();

	if ( l_hrOk == S_OK )
	{
		switch( m_eTranslation )
		{
			case SVExtentTranslationNone:
			case SVExtentTranslationFigureShift:
			{
				p_rsvResult = p_svValue;

				break;
			}
			case SVExtentTranslationShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					SVExtentPointStruct l_svDisplacement;

					m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);

					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX -= l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svDisplacement.m_dPositionY;
				}

				break;
			}// end multiple case:
			case SVExtentTranslationProfileShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;
				}

				break;
			}// end multiple case:
			case SVExtentTranslationTransformShift:
			{
				SVExtentPointStruct l_svPosition;
				SVExtentPointStruct l_svDisplacement;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX -= l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svDisplacement.m_dPositionY;
				}

				break;
			}
			case SVExtentTranslationRotate:
			case SVExtentTranslationProfile:
			case SVExtentTranslationBuildReference:
			case SVExtentTranslationLinear:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult = SVRotatePoint( l_svRotation, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;
				}

				break;
			}// end mulitple case:
			case SVExtentTranslationTransformRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;
				SVExtentPointStruct l_svDisplacement;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult = SVRotatePoint( l_svRotation, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svDisplacement.m_dPositionY;
				}

				break;
			}
			case SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;
				SVExtentPointStruct l_svOutputRotation;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					double l_dAngle = SVGetRotationAngle( l_svStart, l_svEnd );

					p_rsvResult = SVRotatePoint( l_svStart, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svStart.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svStart.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;
				}

				break;
			}// end case SVExtentTranslationLine:
			case SVExtentTranslationFlippedRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult = SVRotateAndFlipPoint( l_svRotation, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;
				}

				break;
			}// end case SVExtentTranslationFlippedRotate:

			case SVExtentTranslationPolarUnwrap:
			{
				SVExtentPointStruct l_svCenterPoint;

				double l_dStartAngle = 0.0;
				double l_dOuterRadius = 0.0;
				double l_dInnerRadius = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svCenterPoint );				

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle );
				}

				if ( l_hrOk == S_OK )
				{
					double l_dRadius = SVGetRadius( l_svCenterPoint, p_svValue );
					double l_dAngle = SVGetFlippedRotationAngle( l_svCenterPoint, p_svValue );

					l_dAngle -= l_dStartAngle;
					l_dAngle -= ((long)(l_dAngle) / 360) * 360.0;

					if( l_dOuterRadius > l_dInnerRadius )
					{
						p_rsvResult.m_dPositionX = g_svPi.DegreesToRadians( l_dAngle ) * l_dOuterRadius;
						p_rsvResult.m_dPositionY = l_dRadius - l_dInnerRadius;
					}
					else
					{
						p_rsvResult.m_dPositionX = g_svPi.DegreesToRadians( l_dAngle ) * l_dInnerRadius;
						p_rsvResult.m_dPositionY = l_dRadius - l_dOuterRadius;
					}
				}

				break;
			}// end case SVExtentTranslationPolarUnwrap:

			case SVExtentTranslationCylindricalWarpH:
			{
				double l_dInputWidth = 0.0;
				double l_dOutputWidth = 0.0;
				double l_dAngle = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dInputWidth );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOutputWidth, l_dOutputWidth );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dAngle );
				if( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY;
					double l_dInputRadius = l_dInputWidth/2.0; 
					double l_dRadius = ( l_dInputWidth / sin( l_dAngle / 360.0 * g_svPi.pi() )) / 2.0 ;
					if( p_svValue.m_dPositionX < l_dInputRadius )
					{
						double l_dXFromCenter = l_dInputRadius - p_svValue.m_dPositionX ;
						double l_dAngleAtPoint = 90.0 - acos( l_dXFromCenter / l_dRadius );
						double l_dCircum = l_dAngleAtPoint/180.0 * g_svPi.pi() * l_dRadius ;
						p_rsvResult.m_dPositionX =  l_dOutputWidth/2.0 - l_dCircum ;
					}
					else
					{
						double l_dXFromCenter = p_svValue.m_dPositionX - l_dInputRadius ;
						double l_dAngleAtPoint = 90.0 - acos( l_dXFromCenter / l_dRadius );
						double l_dCircum = l_dAngleAtPoint/180.0 * g_svPi.pi() * l_dRadius ;
						p_rsvResult.m_dPositionX =  l_dOutputWidth/2.0 + l_dCircum ;
					}
				}
				break;
			}// end case SVExtentTranslationCylindricalWarpH:


			case SVExtentTranslationCylindricalWarpV:
			{
				double l_dInputHeight = 0.0;
				double l_dOutputHeight = 0.0;
				double l_dAngle = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dInputHeight );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOutputHeight, l_dOutputHeight );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dAngle );
				if( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX;
					double l_dInputRadius = l_dInputHeight/2.0; // small height
					double l_dRadius = ( l_dInputHeight / sin( l_dAngle / 360.0 * g_svPi.pi()  )) / 2.0 ;
					if( p_svValue.m_dPositionY < l_dInputRadius ) // Quadrant detection
					{
						double l_dYFromCenter = l_dInputRadius - p_svValue.m_dPositionY ;
						double l_dAngleAtPoint = 90.0 - acos( l_dYFromCenter / l_dRadius );
						double l_dCircum = l_dAngleAtPoint/180.0 * g_svPi.pi() * l_dRadius ;
						p_rsvResult.m_dPositionY =  l_dOutputHeight/2.0 - l_dCircum ;
					}
					else
					{
						double l_dYFromCenter = p_svValue.m_dPositionY - l_dInputRadius  ;
						double l_dAngleAtPoint = 90.0 - acos( l_dYFromCenter / l_dRadius );
						double l_dCircum = l_dAngleAtPoint/180.0 * g_svPi.pi() * l_dRadius ;
						p_rsvResult.m_dPositionY =  l_dOutputHeight/2.0 + l_dCircum ;
					}

				}
				break;
			}// end case SVExtentTranslationCylindricalWarpV:
			case SVExtentTranslationVerticalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dyOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dFullWidth );
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dFullHeight );
				}
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dyOffset );
				}
				if( l_hrOk == S_OK )
				{
					p_rsvResult = p_svValue;			// Copy points 1 to 1. all X and Y when center points.
					double l_dHeight = l_dFullHeight/2; // One Quadrants Width
					double l_dWidth = l_dFullWidth/2;	// One Quadrant
					double l_dYhyp;						// Point on hypotnuse

					if( p_svValue.m_dPositionY < l_dHeight &&  p_svValue.m_dPositionX < l_dWidth )
					{
						//******** Upper Left Quadrant ******************
						l_dYhyp = l_dyOffset - (l_dyOffset / l_dWidth) * p_svValue.m_dPositionX;
						p_rsvResult.m_dPositionY = l_dHeight - ( l_dHeight - p_svValue.m_dPositionY )* l_dHeight/( l_dHeight - l_dYhyp );
					}
					else
					if( p_svValue.m_dPositionY > l_dHeight && p_svValue.m_dPositionX < l_dWidth )
					{
						//******** Lower Left Quadrant ******************
						l_dYhyp = l_dFullHeight - l_dyOffset + (l_dyOffset / l_dWidth) * p_svValue.m_dPositionX;
						p_rsvResult.m_dPositionY = l_dHeight + ( l_dHeight - p_svValue.m_dPositionY )* l_dHeight/( l_dHeight - l_dYhyp );
					}
					else
					if( p_svValue.m_dPositionY < l_dHeight && p_svValue.m_dPositionX > l_dWidth )
					{
						//******** Upper Right Quadrant ******************
						l_dYhyp = l_dyOffset - (l_dyOffset / l_dWidth) * ( l_dFullWidth - p_svValue.m_dPositionX );
						p_rsvResult.m_dPositionY = l_dHeight - ( l_dHeight - p_svValue.m_dPositionY )* l_dHeight/( l_dHeight - l_dYhyp );
					}
					else
					if( p_svValue.m_dPositionY > l_dHeight && p_svValue.m_dPositionX > l_dWidth )
					{
						//******** Lower Right Quadrant ******************
						l_dYhyp = l_dFullHeight - (l_dyOffset / l_dWidth) * ( p_svValue.m_dPositionX - l_dWidth );
						p_rsvResult.m_dPositionY = l_dHeight  + ( l_dHeight - p_svValue.m_dPositionY )* l_dHeight/( l_dHeight - l_dYhyp );

					}				
				}

				break;
			}
			case SVExtentTranslationHorizontalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dxOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dFullWidth );
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dFullHeight );
				}
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dxOffset );
				}
				if( l_hrOk == S_OK )
				{
					p_rsvResult = p_svValue;			// Copy points 1 to 1. all X and Y when center points.
					double l_dHeight = l_dFullHeight/2; // One Quadrants Width
					double l_dWidth = l_dFullWidth/2;	// One Quadrant
					double l_dXhyp;						// Point on hypotnuse

					if( p_svValue.m_dPositionY < l_dHeight &&  p_svValue.m_dPositionX < l_dWidth )
					{
						//******** Upper Left Quadrant ******************
						l_dXhyp = l_dxOffset - (l_dxOffset / l_dHeight) * p_svValue.m_dPositionY;
						p_rsvResult.m_dPositionX = l_dWidth - ( l_dWidth - p_svValue.m_dPositionX )* l_dWidth/( l_dWidth - l_dXhyp );
					}
					else
					if( p_svValue.m_dPositionY > l_dHeight && p_svValue.m_dPositionX < l_dWidth )
					{
						//******** Lower Left Quadrant ******************
						l_dXhyp = l_dxOffset - (l_dxOffset / l_dHeight) * ( l_dFullHeight - p_svValue.m_dPositionY);
						p_rsvResult.m_dPositionX = l_dWidth - ( l_dWidth - p_svValue.m_dPositionX )* l_dWidth/( l_dWidth - l_dXhyp );
					}
					else
					if( p_svValue.m_dPositionY < l_dHeight && p_svValue.m_dPositionX > l_dWidth )
					{
						//******** Upper Right Quadrant ******************
						l_dXhyp = l_dFullWidth - l_dxOffset + (l_dxOffset / l_dHeight) * p_svValue.m_dPositionY;
						p_rsvResult.m_dPositionX = l_dWidth + ( l_dWidth - p_svValue.m_dPositionX )* l_dWidth/( l_dWidth - l_dXhyp );
					}
					else
					if( p_svValue.m_dPositionY > l_dHeight && p_svValue.m_dPositionX > l_dWidth )
					{
						//******** Lower Right Quadrant ******************
						l_dXhyp = l_dFullWidth - (l_dxOffset / l_dHeight) * ( p_svValue.m_dPositionY - l_dHeight );
						p_rsvResult.m_dPositionX = l_dWidth  + ( l_dWidth - p_svValue.m_dPositionX )* l_dWidth/( l_dWidth - l_dXhyp );

					}				
				}

				break;
			}

			case SVExtentTranslationDoubleHeight:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = (p_svValue.m_dPositionY - l_svPosition.m_dPositionY) * 2;
				}
			
				break;
			}

			case SVExtentTranslationResize:
			{
				SVExtentPointStruct l_svPosition;
				double heightScaleFactor = 1.0;
				double widthScaleFactor = 1.0;

				//@WARNING [Jim][8 July 2015]  No identification of error cases.  
				// GetExtentProperty () only returns S_FALSE, which is not 
				// very helpful. Currently none of these cases give much help
				// with error identification.
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
				if (l_hrOk == S_OK)
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeightScaleFactor, heightScaleFactor );
				}

				if (l_hrOk == S_OK)
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidthScaleFactor, widthScaleFactor );
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = (p_svValue.m_dPositionX - l_svPosition.m_dPositionX) * widthScaleFactor;
					p_rsvResult.m_dPositionY = (p_svValue.m_dPositionY - l_svPosition.m_dPositionY) * heightScaleFactor;
				}

				break;
			}

			case SVExtentTranslationFlipVertical:
			{
				SVExtentPointStruct l_svPosition;
				double l_dHeight = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
	
				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight );
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = (l_dHeight - 1) - (p_svValue.m_dPositionY - l_svPosition.m_dPositionY) ;
				}
			
				break;			
			}
			case SVExtentTranslationFlipHorizontal:
			{
				SVExtentPointStruct l_svPosition;
				double l_dWidth = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = (l_dWidth - 1) - (p_svValue.m_dPositionX - l_svPosition.m_dPositionX);
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;
				}
			
				break;			
			}
			default:
			{
				l_hrOk = S_FALSE;

				break;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult ) const
{
	HRESULT l_hrOk = p_rsvResult.Initialize();

	if ( l_hrOk == S_OK )
	{
		switch( m_eTranslation )
		{
			case SVExtentTranslationNone:
			{
				p_rsvResult = p_svValue;

				l_hrOk = S_OK;

				break;
			}

			case SVExtentTranslationFigureShift:
			case SVExtentTranslationProfileShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX + l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY + l_svPosition.m_dPositionY;
				}

				break;
			}// end case multiple
			case SVExtentTranslationShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					SVExtentPointStruct l_svDisplacement;

					m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);

					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX + l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY + l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svDisplacement.m_dPositionY;
				}

				break;
			}// end case multiple
			case SVExtentTranslationTransformShift:
			{
				SVExtentPointStruct l_svPosition;
				SVExtentPointStruct l_svDisplacement;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX + l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY + l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svDisplacement.m_dPositionY;
				}

				break;
			}
			case SVExtentTranslationRotate:
			case SVExtentTranslationProfile:
			case SVExtentTranslationBuildReference:
			case SVExtentTranslationLinear:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					if( l_dAngle == 0.0 )
					{
						p_rsvResult = p_svValue;
					}
					else
					{
						p_rsvResult = SVRotatePoint( l_svOutputRotation, p_svValue, l_dAngle );
					}

					p_rsvResult.m_dPositionX -= l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svOutputRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svRotation.m_dPositionY;
				}

				break;
			}// end case multiple
			case SVExtentTranslationTransformRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;
				SVExtentPointStruct l_svDisplacement;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( l_hrOk == S_OK )
				{
					if( l_dAngle == 0.0 )
					{
						p_rsvResult = p_svValue;
					}
					else
					{
						p_rsvResult = SVRotatePoint( l_svOutputRotation, p_svValue, l_dAngle );
					}

					p_rsvResult.m_dPositionX -= l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svOutputRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svDisplacement.m_dPositionY;
				}

				break;
			}
			case SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;
				SVExtentPointStruct l_svOutputRotation;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					double l_dAngle = SVGetRotationAngle( l_svStart, l_svEnd );

					if( l_dAngle == 0.0 )
					{
						p_rsvResult = p_svValue;
					}
					else
					{
						p_rsvResult = SVRotatePoint( l_svOutputRotation, p_svValue, l_dAngle );
					}

					p_rsvResult.m_dPositionX -= l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svOutputRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svStart.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svStart.m_dPositionY;
				}

				break;
			}// end case SVExtentTranslationLine:

			case SVExtentTranslationFlippedRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( l_hrOk == S_OK )
				{
					if( l_dAngle == 0.0 )
					{
						p_rsvResult = p_svValue;
					}
					else
					{
						p_rsvResult = SVRotateAndFlipPoint( l_svOutputRotation, p_svValue, l_dAngle );
					}

					p_rsvResult.m_dPositionX -= l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svOutputRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svRotation.m_dPositionY;
				}

				break;
			}// end case SVExtentTranslationFlippedRotate:

			case SVExtentTranslationPolarUnwrap:
			{
				SVExtentPointStruct l_svCenterPoint;

				double l_dStartAngle = 0.0;
				double l_dOuterRadius = 0.0;
				double l_dInnerRadius = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svCenterPoint );				

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle );
				}

				if ( l_hrOk == S_OK )
				{
					double l_dRadius;
					double l_dAngle;
					if( l_dOuterRadius > l_dInnerRadius )
					{
						l_dRadius = p_svValue.m_dPositionY + l_dInnerRadius;
						l_dAngle = g_svPi.RadiansToDegrees( p_svValue.m_dPositionX / l_dOuterRadius ) + l_dStartAngle;
					}
					else
					{
						l_dRadius = l_dInnerRadius - p_svValue.m_dPositionY;
						l_dAngle = g_svPi.RadiansToDegrees(  p_svValue.m_dPositionX / l_dInnerRadius ) + l_dStartAngle;
					}

					p_rsvResult = SVRotateAndFlipPoint( l_svCenterPoint, l_dRadius, l_dAngle );
				}

				break;
			}// end case SVExtentTranslationPolarUnwrap:

			case SVExtentTranslationCylindricalWarpH:
			{
				double l_dInputWidth = 0.0;
				double l_dWarpAngle = 0.0;
				double l_dOutputWidth = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dInputWidth );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dWarpAngle );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOutputWidth, l_dOutputWidth );
				if( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY;

					double l_dwidth = l_dOutputWidth/2.0; // small width
					double l_dRadius = ( l_dInputWidth / sin( l_dWarpAngle / 360.0 * g_svPi.pi() )) / 2.0 ;
					if( p_svValue.m_dPositionX < l_dwidth )
					{
						double l_dXFromCenter = l_dwidth - p_svValue.m_dPositionX ;
						double l_dAngleAtPoint = (l_dXFromCenter * 180.0 ) / ( g_svPi.pi() * l_dRadius );
						p_rsvResult.m_dPositionX = ( l_dInputWidth - 1 )/2.0 - cos( (90.0 - l_dAngleAtPoint)/180.0 * g_svPi.pi() ) * l_dRadius ;
					}
					else
					{
						double l_dXFromCenter =  p_svValue.m_dPositionX - l_dwidth;
						double l_dAngleAtPoint = (l_dXFromCenter * 180.0 ) / ( g_svPi.pi() * l_dRadius );
						p_rsvResult.m_dPositionX = ( l_dInputWidth - 1 )/2.0 + cos( ( 90.0 - l_dAngleAtPoint)/180.0 * g_svPi.pi() ) * l_dRadius ;
					}

				}
				break;
			}// end case SVExtentTranslationCylindricalWarpH:


			case SVExtentTranslationCylindricalWarpV:
			{
				double l_dInputHeight = 0.0;
				double l_dWarpAngle = 0.0;
				double l_dOutputHeight = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dInputHeight );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dWarpAngle );
				if( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOutputHeight, l_dOutputHeight );
				if( l_hrOk == S_OK )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX;
					double l_dheight = l_dOutputHeight/2.0; // small height
					double l_dRadius = ( l_dInputHeight / sin( l_dWarpAngle / 360.0 * g_svPi.pi())) / 2 ;
					if( p_svValue.m_dPositionY < l_dheight )
					{
						double l_dYFromCenter = l_dheight - p_svValue.m_dPositionY ;
						double l_dAngleAtPoint = (l_dYFromCenter * 180.0 ) / ( g_svPi.pi() * l_dRadius );
						p_rsvResult.m_dPositionY = ( l_dInputHeight - 1 )/2.0 - cos( (90.0 - l_dAngleAtPoint)/180.0 * g_svPi.pi() ) * l_dRadius ;
					}
					else
					{
						double l_dYFromCenter =  p_svValue.m_dPositionY - l_dheight;
						double l_dAngleAtPoint = (l_dYFromCenter * 180.0 ) / ( g_svPi.pi() * l_dRadius );
						p_rsvResult.m_dPositionY = ( l_dInputHeight - 1 )/2.0 + cos( ( 90.0 - l_dAngleAtPoint)/180.0 * g_svPi.pi() ) * l_dRadius ;
					}
				}
				break;
			}// end case SVExtentTranslationCylindricalWarpV:
			case SVExtentTranslationVerticalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dyOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dFullWidth );
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dFullHeight );
				}
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dyOffset );
				}
				if( l_hrOk == S_OK )
				{
					p_rsvResult = p_svValue;			// Copy points 1 to 1. all X and Y when center points.
					double l_dHeight = l_dFullHeight/2; // One Quadrants Width
					double l_dWidth = l_dFullWidth/2;	// One Quadrant
					double l_dYhyp;	// Point on hypotnuse


					if( p_svValue.m_dPositionY < l_dHeight &&  p_svValue.m_dPositionX < l_dWidth )
					{
						//******** Upper Left Quadrant ******************
						l_dYhyp = l_dyOffset - (l_dyOffset / l_dWidth) * p_svValue.m_dPositionX;
						p_rsvResult.m_dPositionY = l_dHeight - (( l_dHeight - l_dYhyp )*(l_dHeight - p_svValue.m_dPositionY))/l_dHeight;
					}
					else
					if( p_svValue.m_dPositionY > l_dHeight && p_svValue.m_dPositionX < l_dWidth )
					{
						//******** Lower Left Quadrant ******************
						l_dYhyp = l_dFullHeight - l_dyOffset + (l_dyOffset / l_dWidth) * p_svValue.m_dPositionX;
						p_rsvResult.m_dPositionY = l_dHeight - (( l_dHeight - l_dYhyp )*( p_svValue.m_dPositionY - l_dHeight ))/l_dHeight;
					}
					else
					if( p_svValue.m_dPositionY < l_dHeight && p_svValue.m_dPositionX > l_dWidth )
					{
						//******** Upper Right Quadrant ******************
						l_dYhyp = l_dyOffset - (l_dyOffset / l_dWidth) * ( l_dFullWidth - p_svValue.m_dPositionX );
						p_rsvResult.m_dPositionY = l_dHeight - ((l_dHeight - p_svValue.m_dPositionY)*( l_dHeight - l_dYhyp ))/l_dHeight;
					}
					else
					if( p_svValue.m_dPositionY > l_dHeight && p_svValue.m_dPositionX > l_dWidth )
					{
						//******** Lower Right Quadrant ******************
						l_dYhyp = l_dFullHeight - (l_dyOffset / l_dWidth) * ( p_svValue.m_dPositionX - l_dWidth );
						p_rsvResult.m_dPositionY = l_dHeight - ((p_svValue.m_dPositionY - l_dHeight)*( l_dHeight - l_dYhyp ))/l_dHeight;
					}
				}
				break;
			}// end case SVExtentTranslationVerticalPerspective:

			case SVExtentTranslationHorizontalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dxOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dFullWidth );
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dFullHeight );
				}
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dxOffset );
				}
				if( l_hrOk == S_OK )
				{
					p_rsvResult = p_svValue;			// Copy points 1 to 1. all X and Y when center points.
					double l_dWidth = l_dFullWidth/2;	// One Quadrants Width
					double l_dHeight = l_dFullHeight/2;	// One Quadrant
					double l_dXhyp;	// Point on hypotnuse


					if( p_svValue.m_dPositionX < l_dWidth &&  p_svValue.m_dPositionY < l_dHeight )
					{
						//******** Upper Left Quadrant ******************
						l_dXhyp = l_dxOffset - (l_dxOffset / l_dHeight) * p_svValue.m_dPositionY;
						p_rsvResult.m_dPositionX = l_dWidth - (( l_dWidth - l_dXhyp )*(l_dWidth - p_svValue.m_dPositionX))/l_dWidth;
					}
					else
					if( p_svValue.m_dPositionX < l_dWidth && p_svValue.m_dPositionY > l_dHeight )
					{
						//******** Lower Left Quadrant ******************
						l_dXhyp = l_dxOffset - (l_dxOffset / l_dHeight) * ( l_dFullHeight - p_svValue.m_dPositionY);
						p_rsvResult.m_dPositionX = l_dWidth - ((l_dWidth - p_svValue.m_dPositionX)*( l_dWidth - l_dXhyp ))/l_dWidth ;
						//p_rsvResult.m_dPositionX = l_dWidth - (( l_dWidth - l_dXhyp )*( p_svValue.m_dPositionX - l_dWidth ))/l_dWidth;
					}
					else
					if( p_svValue.m_dPositionX > l_dWidth && p_svValue.m_dPositionY < l_dHeight )
					{
						//******** Upper Right Quadrant ******************
						l_dXhyp = l_dFullWidth - l_dxOffset + (l_dxOffset / l_dHeight) *  p_svValue.m_dPositionY;
						p_rsvResult.m_dPositionX = l_dWidth - ((p_svValue.m_dPositionX - l_dWidth)*( l_dWidth - l_dXhyp ))/l_dWidth;
					}
					else
					if( p_svValue.m_dPositionX > l_dWidth && p_svValue.m_dPositionY > l_dHeight )
					{
						//******** Lower Right Quadrant ******************
						l_dXhyp = l_dFullWidth - (l_dxOffset / l_dHeight) * ( p_svValue.m_dPositionY - l_dHeight );
						p_rsvResult.m_dPositionX = l_dWidth - ((p_svValue.m_dPositionX - l_dWidth)*( l_dWidth - l_dXhyp ))/l_dWidth;
						//p_rsvResult.m_dPositionX = l_dWidth - ((l_dWidth - p_svValue.m_dPositionX )*( l_dWidth - l_dXhyp ))/l_dWidth;
					}
				}
				break;
			}// end case SVExtentTranslationHorizontalPerspective:

			//new cases for Flip Horizontal & Vertical
			case SVExtentTranslationFlipVertical:
			{
				SVExtentPointStruct l_svPosition;
				double l_dFullHeight = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
				if( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dFullHeight );
				}
				if ( l_hrOk == S_OK ) 
				{ 
					p_rsvResult.m_dPositionY = (l_dFullHeight - 1) - p_svValue.m_dPositionY;
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX;
				}
				break;
			}
			case SVExtentTranslationFlipHorizontal:
			{
				SVExtentPointStruct l_svPosition;
				double l_dFullWidth = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dFullWidth );
				}
				if ( l_hrOk == S_OK ) 
				{ 
					p_rsvResult.m_dPositionX = (l_dFullWidth - 1) - p_svValue.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY;
				}
				break;
			}
			case SVExtentTranslationDoubleHeight:
				{
					SVExtentPointStruct l_svPosition;

					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

					if ( l_hrOk == S_OK )
					{
						p_rsvResult.m_dPositionX = p_svValue.m_dPositionX;
						p_rsvResult.m_dPositionY = p_svValue.m_dPositionY / 2.0;
					}

					break;
				}
			case SVExtentTranslationResize:
				{
					//@WARNING [Jim][8 July 2015] No identification of error cases.  
					// GetExtentProperty () only returns S_FALSE, which is not 
					// very helpful. Currently none of these cases give much help
					// with error identification.

					SVExtentPointStruct l_svPosition;

					double heightScaleFactor = 1.0;
					double widthScaleFactor = 1.0;

					l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyPositionPoint, l_svPosition);
					if (S_OK == l_hrOk)
					{
						l_hrOk = m_svDimensions.GetExtentProperty(SVExtentPropertyHeightScaleFactor, heightScaleFactor);
					}

					if (S_OK == l_hrOk)
					{
						l_hrOk = m_svDimensions.GetExtentProperty(SVExtentPropertyWidthScaleFactor, widthScaleFactor);
					}

					if (S_OK == l_hrOk)
					{
						// Unscale and make relative to the Parent (input) image
						p_rsvResult.m_dPositionX = (p_svValue.m_dPositionX / widthScaleFactor) + l_svPosition.m_dPositionX;
						p_rsvResult.m_dPositionY = (p_svValue.m_dPositionY / heightScaleFactor) + l_svPosition.m_dPositionY;
					}
					break;
				}
		}// end switch( m_eTranslation )
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace( SVExtentFigureStruct p_svValue, SVExtentFigureStruct &p_rsvResult ) const
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationNone:
		{
			p_rsvResult = p_svValue;

			l_hrOk = S_OK;

			break;
		}
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationPolarUnwrap:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationVerticalPerspective:
		case SVExtentTranslationHorizontalPerspective:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFigureShift:
		{
			switch ( p_svValue.m_eShape )
			{
				case SVExtentShapeArrow:
				{
					l_hrOk = S_OK;

					if ( TranslateFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, -90.0, p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 90.0, p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight, -135.0, p_svValue.m_svTopRight, p_rsvResult.m_svTopRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight, 135.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SVExtentShapeRectangle:
				{
					l_hrOk = S_OK;

					if ( TranslateFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 0.0, p_svValue.m_svTopRight, p_rsvResult.m_svTopRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 90.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 90.0, p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svTopRight, p_rsvResult.m_svTopRight, 90.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svTopRight, p_rsvResult.m_svTopRight, 90.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft, 0.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateLineFromOutputSpace( p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft, 0.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SVExtentShapeLineHorizontal:
				{
					l_hrOk = S_OK;

					SVExtentPointStruct l_svRotatePoint;
					SVExtentPointStruct l_svOutputRotationPoint;

					GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotationPoint );

					SVExtentPointStruct l_svRadiusPoint = p_svValue.m_svCenterLeft;

					if( l_svRadiusPoint.m_dPositionY != l_svOutputRotationPoint.m_dPositionY )
					{
						l_svRadiusPoint.m_dPositionY = l_svOutputRotationPoint.m_dPositionY;
					}

					if ( TranslateFromOutputSpace( l_svRadiusPoint, l_svRotatePoint ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					double l_dNewAngle = 0.0;

					if( p_svValue.m_svCenterLeft.m_dPositionY < l_svRadiusPoint.m_dPositionY )
					{
						l_dNewAngle = -180.0;
					}
					else
					{
						l_dNewAngle = 0.0;
					}

					if ( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, 0.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svTopLeft = p_rsvResult.m_svCenterLeft;
					p_rsvResult.m_svBottomLeft = p_rsvResult.m_svCenterLeft;

					if( p_svValue.m_svCenterRight.m_dPositionY < l_svRadiusPoint.m_dPositionY )
					{
						l_dNewAngle = -180.0;
					}
					else
					{
						l_dNewAngle = 0.0;
					}

					if ( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, 0.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svTopRight = p_rsvResult.m_svCenterRight;
					p_rsvResult.m_svBottomRight = p_rsvResult.m_svCenterRight;

					if( p_svValue.m_svTopCenter.m_dPositionY < l_svRadiusPoint.m_dPositionY )
					{
						l_dNewAngle = -180.0;
					}
					else
					{
						l_dNewAngle = 0.0;
					}

					if ( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svBottomCenter = p_rsvResult.m_svTopCenter;

					break;
				}
				case SVExtentShapeLineVertical:
				{
					l_hrOk = S_OK;

					SVExtentPointStruct l_svRotatePoint;
					SVExtentPointStruct l_svOutputRotationPoint;

					GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotationPoint );

					SVExtentPointStruct l_svRadiusPoint = p_svValue.m_svTopCenter;

					if( l_svRadiusPoint.m_dPositionY != l_svOutputRotationPoint.m_dPositionY )
					{
						l_svRadiusPoint.m_dPositionY = l_svOutputRotationPoint.m_dPositionY;
					}

					if ( TranslateFromOutputSpace( l_svRadiusPoint, l_svRotatePoint ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					double l_dNewAngle = 0.0;

					if( p_svValue.m_svTopCenter.m_dPositionY < l_svRadiusPoint.m_dPositionY )
					{
						l_dNewAngle = -90.0;
					}
					else
					{
						l_dNewAngle = 90.0;
					}

					if ( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, l_dNewAngle, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svTopLeft = p_rsvResult.m_svTopCenter;
					p_rsvResult.m_svTopRight = p_rsvResult.m_svTopCenter;

					if( p_svValue.m_svBottomCenter.m_dPositionY < l_svRadiusPoint.m_dPositionY )
					{
						l_dNewAngle = -90.0;
					}
					else
					{
						l_dNewAngle = 90.0;
					}

					if ( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, 90.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svBottomLeft = p_rsvResult.m_svBottomCenter;
					p_rsvResult.m_svBottomRight = p_rsvResult.m_svBottomCenter;

					if( p_svValue.m_svCenterLeft.m_dPositionY < l_svRadiusPoint.m_dPositionY )
					{
						l_dNewAngle = -90.0;
					}
					else
					{
						l_dNewAngle = 90.0;
					}

					if ( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, 90.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svCenterRight = p_rsvResult.m_svCenterLeft;

					break;
				}
				case SVExtentShapeLine:
				case SVExtentShapePoint:
				case SVExtentShapePie:
				case SVExtentShapeCircle:
				case SVExtentShapeDoubleCircle:
				case SVExtentShapeHexagon:
				{
					l_hrOk = S_OK;

					if ( TranslateFromOutputSpace( p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					if ( TranslateFromOutputSpace( p_svValue.m_svTopRight, p_rsvResult.m_svTopRight ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
			}

			break;
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace( SVExtentLineStruct p_svValue, SVExtentLineStruct &p_rsvResult ) const
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationNone:
		{
			p_rsvResult = p_svValue;

			l_hrOk = S_OK;

			break;
		}
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationPolarUnwrap:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFigureShift:
		{
			long l_lCount = p_svValue.m_svPointArray.GetSize();

			l_hrOk = p_rsvResult.Initialize();

			if( l_hrOk == S_OK && 0 < l_lCount )
			{
				double l_dAngle = 0.0;

				bool l_bIsAngle = GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle ) == S_OK;

				p_rsvResult.m_bIsAngleValid = p_svValue.m_bIsAngleValid;
				p_rsvResult.m_dAngle = p_svValue.m_dAngle;

				if( ! l_bIsAngle || l_dAngle == 0.0 || ! p_svValue.m_bIsAngleValid )
				{
					for( long i = 0; l_hrOk == S_OK && i < l_lCount; i++ )
					{
						SVExtentPointStruct l_svPoint = p_svValue.m_svPointArray[ i ];

						if( TranslateFromOutputSpace( l_svPoint, l_svPoint ) == S_OK )
						{
							p_rsvResult.m_svPointArray.SetAtGrow( i, l_svPoint );
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}
				}
				else
				{
					SVExtentPointStruct l_svRotatePoint;
					SVExtentPointStruct l_svOutputRotationPoint;

					GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotationPoint );

					SVExtentPointStruct l_svRadiusPoint = p_svValue.m_svPointArray[ 0 ];

					long l_lStart = 0;

					if( p_svValue.m_dAngle == 0.0 && l_svRadiusPoint.m_dPositionX != l_svOutputRotationPoint.m_dPositionX )
					{
						l_svRadiusPoint.m_dPositionX = l_svOutputRotationPoint.m_dPositionX;
					}
					else if( p_svValue.m_dAngle == 90.0 && l_svRadiusPoint.m_dPositionY != l_svOutputRotationPoint.m_dPositionY )
					{
						l_svRadiusPoint.m_dPositionY = l_svOutputRotationPoint.m_dPositionY;
					}
					else
					{
						l_lStart = 1;
					}

					if( TranslateFromOutputSpace( l_svRadiusPoint, l_svRotatePoint ) == S_OK )
					{
						if( l_lStart == 1 )
						{
							p_rsvResult.m_svPointArray.SetAtGrow( 0, l_svRotatePoint );
						}
					}
					else
					{
						l_hrOk = S_FALSE;
					}

					for( long i = l_lStart; l_hrOk == S_OK && i < l_lCount; i++ )
					{
						SVExtentPointStruct l_svPoint = p_svValue.m_svPointArray[ i ];

						double l_dNewAngle = p_svValue.m_dAngle;

						if( l_dNewAngle == 0.0 )
						{
							if( l_svPoint.m_dPositionX < l_svRadiusPoint.m_dPositionX )
							{
								l_dNewAngle -= 180;
							}
						}
						else if( l_dNewAngle == 90.0 )
						{
							if( l_svPoint.m_dPositionY < l_svRadiusPoint.m_dPositionY )
							{
								l_dNewAngle -= 180;
							}
						}

						if( TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, l_dNewAngle, l_svPoint, l_svPoint ) == S_OK )
						{
							p_rsvResult.m_svPointArray.SetAtGrow( i, l_svPoint );
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}
				}
			}

			break;
		}
	}

	if( l_hrOk == S_OK )
	{
		p_rsvResult.m_dwColor = p_svValue.m_dwColor;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace( SVExtentMultiLineStruct p_svValue, SVExtentMultiLineStruct &p_rsvResult ) const
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationNone:
		{
			p_rsvResult = p_svValue;

			l_hrOk = S_OK;

			break;
		}
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationPolarUnwrap:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFigureShift:
		{
			long l_lCount = p_svValue.m_svLineArray.GetSize();

			l_hrOk = p_rsvResult.Initialize();

			if( TranslateFromOutputSpace( p_svValue.m_StringPoint, p_rsvResult.m_StringPoint ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			for( long i = 0; l_hrOk == S_OK && i < l_lCount; i++ )
			{
				SVExtentLineStruct l_svLine = p_svValue.m_svLineArray[ i ];

				if( TranslateFromOutputSpace( l_svLine, l_svLine ) == S_OK )
				{
					p_rsvResult.m_svLineArray.SetAtGrow( i, l_svLine );
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}

			break;
		}
	}

	if( l_hrOk == S_OK )
	{
		p_rsvResult.m_Color = p_svValue.m_Color;

		p_rsvResult.m_StringPoint = p_svValue.m_StringPoint;
		p_rsvResult.m_csString = p_svValue.m_csString;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateLineFromOutputSpace( SVExtentPointStruct p_svRadiusPoint, SVExtentPointStruct p_svRotatePoint, double p_dLineAngle, SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult ) const
{
	HRESULT l_hrOk = p_rsvResult.Initialize();

	if ( l_hrOk == S_OK )
	{
		switch( m_eTranslation )
		{
			case SVExtentTranslationNone:
			case SVExtentTranslationShift:
			case SVExtentTranslationProfileShift:
			case SVExtentTranslationTransformShift:
			case SVExtentTranslationDoubleHeight:
			case SVExtentTranslationResize:
			case SVExtentTranslationCylindricalWarpH:
			case SVExtentTranslationCylindricalWarpV:
			case SVExtentTranslationVerticalPerspective:
			case SVExtentTranslationHorizontalPerspective:
			case SVExtentTranslationPolarUnwrap:
			case SVExtentTranslationFlipHorizontal:
			case SVExtentTranslationFlipVertical:
			case SVExtentTranslationFigureShift:
			{
				l_hrOk = TranslateFromOutputSpace( p_svValue, p_rsvResult );

				break;
			}// end case multiple

			case SVExtentTranslationRotate:
			case SVExtentTranslationProfile:
			case SVExtentTranslationTransformRotate:
			case SVExtentTranslationBuildReference:
			case SVExtentTranslationLinear:
			{
				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );

				if ( l_hrOk == S_OK )
				{
					double l_dRadius = SVGetRadius( p_svRadiusPoint, p_svValue );

					p_rsvResult = SVRotatePoint( p_svRotatePoint, l_dRadius, l_dAngle + p_dLineAngle );
				}

				break;
			}// end case multiple

			case SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd );
				}

				if ( l_hrOk == S_OK )
				{
					double l_dAngle = SVGetRotationAngle( l_svStart, l_svEnd );

					if ( l_hrOk == S_OK )
					{
						double l_dRadius = SVGetRadius( p_svRadiusPoint, p_svValue );

						p_rsvResult = SVRotatePoint( p_svRotatePoint, l_dRadius, l_dAngle + p_dLineAngle );
					}
				}

				break;
			}// end case SVExtentTranslationLine:

			case SVExtentTranslationFlippedRotate:
			{
				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );

				if ( l_hrOk == S_OK )
				{
					double l_dRadius = SVGetRadius( p_svRadiusPoint, p_svValue );

					p_rsvResult = SVRotateAndFlipPoint( p_svRotatePoint, l_dRadius, l_dAngle - p_dLineAngle );
				}
			}// end case SVExtentTranslationFlippedRotate:

		}// end switch( m_eTranslation )
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const
{
	HRESULT l_hrOk = S_OK;

	SVExtentFigureStruct l_svFigure;

	RECT l_oRect;

	l_hrOk = GetOutputRectangle( l_oRect );

	if( l_hrOk == S_OK )
	{
		if( p_rsvValue.GetFigure( l_svFigure ) == S_OK )
		{
			if( p_rsvValue.m_eTranslation == SVExtentTranslationLine )
			{
				l_oRect.bottom--;
				l_oRect.right--;

				if( ! ::PtInRect( &l_oRect, l_svFigure.m_svCenterLeft ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svCenterRight ) )
				{
					l_hrOk = S_FALSE;
				}
			}
			else
			{
				l_oRect.bottom++;
				l_oRect.right++;

				if( ! ::PtInRect( &l_oRect, l_svFigure.m_svCenterLeft ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svCenterRight ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svBottomLeft ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svBottomCenter ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svBottomRight ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svTopLeft ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svTopCenter ) )
				{
					l_hrOk = S_FALSE;
				}
				else if( ! ::PtInRect( &l_oRect, l_svFigure.m_svTopRight ) )
				{
					l_hrOk = S_FALSE;
				}
			}
		}
		else
		{
			RECT l_oInRect;

			if( p_rsvValue.GetRectangle( l_oInRect ) == S_OK )
			{
				RECT l_oOutRect;

				if( ! ::UnionRect( &l_oOutRect, &l_oRect, &l_oInRect ) ||
				    ! ::EqualRect( &l_oOutRect, &l_oRect ) )
				{
					l_hrOk = S_FALSE;
				}
			}
		}
	}

	return l_hrOk;
}

bool SVImageExtentClass::IsEqualExcludePosition( const SVImageExtentClass &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = l_bOk && m_eTranslation == p_rsvValue.m_eTranslation;
	l_bOk = l_bOk && m_svDimensions == p_rsvValue.m_svDimensions;

	return l_bOk;
}

bool SVImageExtentClass::IsPositionEqual( const SVImageExtentClass &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = l_bOk && m_svPosition == p_rsvValue.m_svPosition;

	return l_bOk;
}

bool SVImageExtentClass::operator==( const SVImageExtentClass &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = l_bOk && IsEqualExcludePosition( p_rsvValue );
	l_bOk = l_bOk && IsPositionEqual( p_rsvValue );

	return l_bOk;
}

bool SVImageExtentClass::operator!=( const SVImageExtentClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

HRESULT SVImageExtentClass::UpdateSourceOffset( SVExtentOffsetStruct& p_rsvOffsetData )
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SVExtentTranslationNone:
		{
			p_rsvOffsetData.m_bIsLinear &= true;

			l_hrOk = S_OK;

			break;
		}

		case SVExtentTranslationShift:
		{
			SVExtentPointStruct l_svPosition;

			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svDisplacement;
				SVExtentPointStruct l_svResult = l_svPosition;

				m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);

				l_svResult += l_svDisplacement;
				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				l_hrOk = p_rsvOffsetData.m_svRotationPoint.Initialize();

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SVExtentTranslationProfileShift:
		{
			SVExtentPointStruct l_svPosition;

			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svResult = l_svPosition;

				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				l_hrOk = p_rsvOffsetData.m_svRotationPoint.Initialize();

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SVExtentTranslationRotate:
		case SVExtentTranslationProfile:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLinear:
		{
			SVExtentPointStruct l_svRotation;
			SVExtentPointStruct l_svOutputRotation;

			double l_dAngle = 0.0;

			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
			}

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svResult = l_svRotation;

				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				p_rsvOffsetData.m_svRotationPoint = l_svOutputRotation;

				p_rsvOffsetData.m_dRotationAngle = ::fmod( ( p_rsvOffsetData.m_dRotationAngle + l_dAngle ), 360.0 );

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SVExtentTranslationTransformShift:
		{
			SVExtentPointStruct l_svDisplacement;

			l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svResult = l_svDisplacement;

				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				l_hrOk = p_rsvOffsetData.m_svRotationPoint.Initialize();

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SVExtentTranslationTransformRotate:
		{
			SVExtentPointStruct l_svRotation;
			SVExtentPointStruct l_svOutputRotation;
			SVExtentPointStruct l_svDisplacement;

			double l_dAngle = 0.0;

			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyTranslationOffset, l_svDisplacement);
			}

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svResult = l_svRotation;

				l_svResult += l_svDisplacement;
				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				p_rsvOffsetData.m_svRotationPoint = l_svOutputRotation;

				p_rsvOffsetData.m_dRotationAngle = ::fmod( ( p_rsvOffsetData.m_dRotationAngle + l_dAngle ), 360.0 );

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}
		case SVExtentTranslationFlippedRotate:
		{
			SVExtentPointStruct l_svRotation;
			SVExtentPointStruct l_svOutputRotation;

			double l_dAngle = 0.0;

			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svRotation );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
			}

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svResult = l_svRotation;

				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				p_rsvOffsetData.m_svRotationPoint = l_svOutputRotation;

				p_rsvOffsetData.m_dRotationAngle = ::fmod( ( p_rsvOffsetData.m_dRotationAngle - l_dAngle ), 360.0 );

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case SVExtentTranslationFlippedRotate:
		// Warp Type Tools
		case SVExtentTranslationVerticalPerspective:
		case SVExtentTranslationHorizontalPerspective:
		case SVExtentTranslationCylindricalWarpH:
		case SVExtentTranslationCylindricalWarpV:
		case SVExtentTranslationFigureShift:
		case SVExtentTranslationPolarUnwrap:
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationResize:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFlipHorizontal:
		{
			SVExtentPointStruct l_svPosition;

			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );

			if ( l_hrOk == S_OK )
			{
				SVExtentPointStruct l_svResult = l_svPosition;

				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				l_hrOk = p_rsvOffsetData.m_svRotationPoint.Initialize();

				p_rsvOffsetData.m_bIsLinear = false;
			}

			break;
		}// end case multiple
		default:
		{
			break;
		}
	}// end switch( m_eTranslation )

	return l_hrOk;
}

HRESULT SVImageExtentClass::UpdatePolarFromOutputSpace( SVExtentLocationPropertyEnum p_eLocation, long p_dX, long p_dY )
{
	HRESULT l_hrOk = S_FALSE;
	SVExtentPointStruct l_svPosition;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if (   GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK
	    && GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle ) == S_OK
	    && GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle ) == S_OK
	    && GetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius ) == S_OK
	    && GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius ) == S_OK )
	{
		switch ( p_eLocation )
		{
			case SVExtentLocationPropertyLeft:
			{
				double l_dNewAngle = l_dStartAngle + g_svPi.RadiansToDegrees( p_dX ) / l_dOuterRadius;

				if ( l_dNewAngle > l_dEndAngle )
					l_dNewAngle = l_dEndAngle;

				l_dStartAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyRight:
			{
				double l_dNewAngle = l_dEndAngle + g_svPi.RadiansToDegrees( p_dX ) / l_dOuterRadius;

				if ( l_dNewAngle < l_dStartAngle )
					l_dNewAngle = l_dStartAngle;

				l_dEndAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyTop:
			{
				l_dInnerRadius += p_dY;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyBottom:
			{
				l_dOuterRadius += p_dY;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyCenter:
			{
				l_svPosition.m_dPositionX += p_dX;
				l_svPosition.m_dPositionY += p_dY;

				l_hrOk = S_OK;

				break;
			}
		}// end switch ( p_eLocation )

		if ( l_hrOk == S_OK )
		{
			if ( SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius ) == S_OK )
			{
				l_hrOk = UpdateData();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVImageExtentClass::UpdateLine( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svStart;
	SVExtentPointStruct l_svEnd;

	if ( GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart ) == S_OK &&
			 GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd ) == S_OK )
	{
		double l_dX = p_svEnd.m_dPositionX - p_svStart.m_dPositionX;
		double l_dY = p_svEnd.m_dPositionY - p_svStart.m_dPositionY;

		switch( p_eLocation )
		{
			case SVExtentLocationPropertyLeft:
			{
				l_svStart.m_dPositionX += l_dX;
				l_svStart.m_dPositionY += l_dY;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyRight:
			{
				l_svEnd.m_dPositionX += l_dX;
				l_svEnd.m_dPositionY += l_dY;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyCenter:
			{
				l_svStart.m_dPositionX += l_dX;
				l_svStart.m_dPositionY += l_dY;

				l_svEnd.m_dPositionX += l_dX;
				l_svEnd.m_dPositionY += l_dY;

				l_hrOk = S_OK;

				break;
			}
		}

		if ( l_hrOk == S_OK )
		{
			if ( SetExtentProperty( SVExtentPropertyPositionPoint, l_svStart ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd ) == S_OK )
			{
				l_hrOk = UpdateData();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::UpdatePolar( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPosition;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if ( GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
			 GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle ) == S_OK &&
			 GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle ) == S_OK &&
			 GetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius ) == S_OK &&
			 GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius ) == S_OK )
	{
		double l_dNewAngle = SVGetFlippedRotationAngle( l_svPosition, p_svEnd ); 
		double l_dNewRadius = SVGetRadius( l_svPosition, p_svEnd ); 

		switch( p_eLocation )
		{
			case SVExtentLocationPropertyTopLeft:
			{
				long l_lMult = (long)( l_dStartAngle ) / 360;

				if ( l_dStartAngle < 0 )
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if ( 180.0 < ::fabs( l_dNewAngle - l_dStartAngle ))
				{
					if( l_dStartAngle < l_dNewAngle )
					{
						l_dStartAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dStartAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dStartAngle = l_dNewAngle;
				}


				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyTopRight:
			{
				long l_lMult = (long)( l_dEndAngle ) / 360;

				if ( l_dEndAngle < 0 )
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if ( 180.0 < ::fabs( l_dNewAngle - l_dEndAngle ))
				{
					if( l_dEndAngle < l_dNewAngle )
					{
						l_dEndAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dEndAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dEndAngle = l_dNewAngle;
				}

				if( l_dEndAngle - l_dStartAngle > 720 )
				{
					l_dStartAngle = l_dEndAngle - 720;
				}


				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyBottomRight:
			{
				long l_lMult = (long)( l_dEndAngle ) / 360;

				if ( l_dEndAngle < 0 )
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if ( 180.0 < ::fabs( l_dNewAngle - l_dEndAngle ))
				{
					if( l_dEndAngle < l_dNewAngle )
					{
						l_dEndAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dEndAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dEndAngle = l_dNewAngle;
				}

				if( l_dEndAngle - l_dStartAngle > 720 )
				{
					l_dStartAngle = l_dEndAngle - 720;
				}


				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyBottomLeft:
			{
				long l_lMult = (long)( l_dStartAngle ) / 360;

				if ( l_dStartAngle < 0 )
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if ( 180.0 < ::fabs( l_dNewAngle - l_dStartAngle ))
				{
					if( l_dStartAngle < l_dNewAngle )
					{
						l_dStartAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dStartAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dStartAngle = l_dNewAngle;
				}


				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyLeft:
			{
				long l_lMult = (long)( l_dStartAngle ) / 360;

				if ( l_dStartAngle < 0 )
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if ( 180.0 < ::fabs( l_dNewAngle - l_dStartAngle ))
				{
					if( l_dStartAngle < l_dNewAngle )
					{
						l_dStartAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dStartAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dStartAngle = l_dNewAngle;
				}

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyRight:
			{
				long l_lMult = (long)( l_dEndAngle ) / 360;

				if ( l_dEndAngle < 0 )
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if ( 180.0 < ::fabs( l_dNewAngle - l_dEndAngle ))
				{
					if( l_dEndAngle < l_dNewAngle )
					{
						l_dEndAngle = l_dNewAngle - 360;
					}
					else
					{
						l_dEndAngle = l_dNewAngle + 360;
					}
				}
				else
				{
					l_dEndAngle = l_dNewAngle;
				}

				if( l_dEndAngle - l_dStartAngle > 720 )
				{
					l_dStartAngle = l_dEndAngle - 720;
				}

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyTop:
			{
				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyBottom:
			{
				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyCenter:
			{
				l_svPosition.m_dPositionX += p_svEnd.m_dPositionX - p_svStart.m_dPositionX;
				l_svPosition.m_dPositionY += p_svEnd.m_dPositionY - p_svStart.m_dPositionY;

				l_hrOk = S_OK;

				break;
			}
		}

		if ( l_hrOk == S_OK )
		{
			if ( SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius ) == S_OK &&
					 SetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius ) == S_OK )
			{
				l_hrOk = UpdateData();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::ClearOutputData()
{
	HRESULT l_hrOk = m_svFigure.Initialize();

	if ( m_svPosition.DisableExtentProperty( SVExtentPropertyPositionsOutput ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svDimensions.DisableExtentProperty( SVExtentPropertyDimentionsOutput ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if( m_eTranslation == SVExtentTranslationLine )
	{
		if ( m_svPosition.DisableExtentProperty( SVExtentPropertyRotationAngle ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}

		if ( m_svDimensions.DisableExtentProperty( SVExtentPropertyWidth ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}

		if ( m_svDimensions.DisableExtentProperty( SVExtentPropertyHeight ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		if ( m_svPosition.DisableExtentProperty( SVExtentPropertyPositionPointEndOfLine ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::BuildOutputDimensions()
{
	HRESULT l_hrOk = ClearOutputData();

	if ( l_hrOk == S_OK )
	{
		switch( m_eTranslation )
		{
			case SVExtentTranslationUnknown:
			{
				break;
			}

			case SVExtentTranslationNone:
			case SVExtentTranslationShift:
			case SVExtentTranslationProfileShift:
			case SVExtentTranslationBuildReference:
			case SVExtentTranslationTransformShift:
			case SVExtentTranslationFlipVertical:
			case SVExtentTranslationFlipHorizontal:
			case SVExtentTranslationFigureShift:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end multiple case:

			case SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;

				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd );				
				}

				if ( l_hrOk == S_OK )
				{
					double l_dWidth = SVGetRadius( l_svStart, l_svEnd );

					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyWidth, l_dWidth );				

					if ( l_hrOk == S_OK )
					{
						l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dWidth );				
					}
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyHeight, 1.0 );				

					if ( l_hrOk == S_OK )
					{
						l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, 1.0 );				
					}
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0.0 );
				}

				break;
			}// end case SVExtentTranslationLine:

			case SVExtentTranslationDoubleHeight:
			case SVExtentTranslationResize:
			{
				//@WARNING [Jim][8 July 2015] No identification of error cases.  
				// GetExtentProperty () only returns S_FALSE, which is not 
				// very helpful. Currently none of these cases give much help
				// with error identification.

				double l_dValue = 0.0;
				double inputHeight = 0.0;
				double inputWidth = 0.0;
				double heightScaleFactor = 1.0;
				double widthScaleFactor = 1.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, inputWidth );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidthScaleFactor, widthScaleFactor );				
				}

				if ( l_hrOk == S_OK )
				{
					l_dValue = inputWidth * widthScaleFactor;
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, inputHeight );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeightScaleFactor, heightScaleFactor );				
				}

				if ( l_hrOk == S_OK )
				{
					l_dValue = inputHeight * heightScaleFactor;

					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end case SVExtentTranslationDoubleHeight:

			case SVExtentTranslationFlippedRotate:
			case SVExtentTranslationRotate:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end case SVExtentTranslationFlippedRotate: case SVExtentTranslationRotate:

			case SVExtentTranslationLinear:
			{
				double l_dHeight = 0.0;
				double l_dWidth = 0.0;
				double l_dAngle = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dWidth );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dHeight );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
				}

				break;
			}// end case SVExtentTranslationLinear:

			case SVExtentTranslationProfile:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					SVExtentPointStruct l_svPoint;

					l_svPoint.m_dPositionY += l_dValue / 2;

					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svPoint );
				}

				break;
			}// end case SVExtentTranslationProfile:

			case SVExtentTranslationPolarUnwrap:
			{
				double l_dInnerRadius = 0.0;
				double l_dOuterRadius = 0.0;
				double l_dStartAngle = 0.0;
				double l_dEndAngle = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyRectangle );

				if ( m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle ) != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				if ( m_svDimensions.GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle ) != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				if ( m_svDimensions.GetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius ) != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				if ( m_svDimensions.GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius ) != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				if ( l_hrOk == S_OK )
				{
					double l_dHeight = fabs( l_dOuterRadius - l_dInnerRadius );
					if( l_dHeight < 1.0 )
					{
						l_dHeight = 1.0;
					}
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dHeight );

					if ( l_hrOk == S_OK )
					{
						double l_dMaxRadius = l_dOuterRadius > l_dInnerRadius ? l_dOuterRadius : l_dInnerRadius;
						double l_dWidth = g_svPi.DegreesToRadians( l_dEndAngle - l_dStartAngle ) * l_dMaxRadius;
						if( l_dWidth < 1.0 )
						{
							l_dWidth = 1.0;
						}
						l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dWidth );
					}
				}

				break;
			}// end case SVExtentTranslationPolarUnwrap:

			case SVExtentTranslationTransformRotate:
			{
				SVExtentPointStruct l_svPosition;

				double l_dHeight = 0.0;
				double l_dWidth = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );				
				}

				if ( l_hrOk == S_OK )
				{
					double l_dMax = sqrt( ( l_dWidth * l_dWidth ) + ( l_dHeight * l_dHeight ) );

					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dMax );

					if ( l_hrOk == S_OK )
					{
						l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dMax );
					}

					if ( l_hrOk == S_OK )
					{
						l_svPosition.m_dPositionX += ( l_dMax - l_dWidth ) / 2;
						l_svPosition.m_dPositionY += ( l_dMax - l_dHeight ) / 2;

						l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svPosition );
					}
				}

				break;
			}// end case SVExtentTranslationTransformRotate:

			case SVExtentTranslationCylindricalWarpH:
			{
				double l_dValue = 0.0;
				double l_dWarpAngle = 0.0;

				l_hrOk = S_OK; 

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dValue );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dWarpAngle );

				if ( l_hrOk == S_OK )
				{
					double l_dRadius = ( l_dValue / sin( l_dWarpAngle / 360.0 * g_svPi.pi())) / 2.0 ;
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dRadius * g_svPi.pi() * ( l_dWarpAngle / 180.0 ) );
				}

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dValue );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPointX, 0.0 );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPointY, 0.0 );				

				break;
			}// end case SVExtentTranslationCylindricalWarpH:

			case SVExtentTranslationCylindricalWarpV:
			{
				double l_dValue = 0.0;
				double l_dWarpAngle = 0.0;

				l_hrOk = S_OK; //m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dValue );

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dWarpAngle );

				if ( l_hrOk == S_OK )
				{
					double l_dRadius = ( l_dValue / sin( l_dWarpAngle / 360.0 * g_svPi.pi()  )) / 2.0 ;
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dRadius * g_svPi.pi() * ( l_dWarpAngle / 180.0 ) );
				}

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dValue );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPointX, 0.0 );				

				if ( l_hrOk == S_OK )
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPointY, 0.0 );				

				break;
			}// end case SVExtentTranslationCylindricalWarpV:

			case SVExtentTranslationVerticalPerspective:
			case SVExtentTranslationHorizontalPerspective:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SVExtentPropertyPie );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end case SVExtentTranslationVerticalPerspective:

			default:
			{
				l_hrOk = S_FALSE;

				break;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::BuildFigure()
{
	HRESULT l_hrOk = m_svFigure.Initialize();

	if ( l_hrOk == S_OK )
	{
		switch( m_eTranslation )
		{
			case SVExtentTranslationUnknown:
			case SVExtentTranslationNone:
			case SVExtentTranslationTransformShift:
			case SVExtentTranslationTransformRotate:
			case SVExtentTranslationCylindricalWarpH:
			case SVExtentTranslationCylindricalWarpV:
			{
				// No figure
				break;
			}
			case SVExtentTranslationShift:
			case SVExtentTranslationRotate:
			case SVExtentTranslationFlippedRotate:
			case SVExtentTranslationProfile:
			case SVExtentTranslationProfileShift:
			case SVExtentTranslationBuildReference:
			{
				double l_dWidth = 0.0;
				double l_dHeight = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );				

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight );				
				}

				if ( l_hrOk == S_OK )
				{
					m_svFigure.m_eShape = SVExtentShapeRectangle;

					m_svFigure.m_svTopCenter.m_dPositionX = (long)( l_dWidth / 2 );

					m_svFigure.m_svTopRight.m_dPositionX = (long)( l_dWidth );

					m_svFigure.m_svCenterLeft.m_dPositionY = (long)( l_dHeight / 2 );

					m_svFigure.m_svCenterRight.m_dPositionY = (long)( l_dHeight / 2 );
					m_svFigure.m_svCenterRight.m_dPositionX = (long)( l_dWidth );

					m_svFigure.m_svBottomLeft.m_dPositionY = (long)( l_dHeight );

					m_svFigure.m_svBottomCenter.m_dPositionY = (long)( l_dHeight );
					m_svFigure.m_svBottomCenter.m_dPositionX = (long)( l_dWidth / 2 );

					m_svFigure.m_svBottomRight.m_dPositionY = (long)( l_dHeight );
					m_svFigure.m_svBottomRight.m_dPositionX = (long)( l_dWidth );

					l_hrOk = TranslateFromOutputSpace( m_svFigure, m_svFigure );
				}

				break;
			}
			case SVExtentTranslationDoubleHeight:
			case SVExtentTranslationResize:
			case SVExtentTranslationFlipHorizontal:
			case SVExtentTranslationFlipVertical:
			case SVExtentTranslationFigureShift:
			{
				double l_dWidth = 0.0;
				double l_dHeight = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );				

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight );				
				}

				SVExtentPointStruct l_svStart;
				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart );
				}

				if ( l_hrOk == S_OK )
				{
					m_svFigure.m_eShape = SVExtentShapeRectangle;

					m_svFigure.m_svTopLeft.m_dPositionX = l_svStart.m_dPositionX;
					m_svFigure.m_svTopLeft.m_dPositionY = l_svStart.m_dPositionY;

					//set top center
					m_svFigure.m_svTopCenter.m_dPositionX = l_svStart.m_dPositionX + (l_dWidth/2.0);
					m_svFigure.m_svTopCenter.m_dPositionY = l_svStart.m_dPositionY;

					//set top right
					m_svFigure.m_svTopRight.m_dPositionX = l_svStart.m_dPositionX + l_dWidth;
					m_svFigure.m_svTopRight.m_dPositionY = l_svStart.m_dPositionY;

					//set center left
					m_svFigure.m_svCenterLeft.m_dPositionX = l_svStart.m_dPositionX;
					m_svFigure.m_svCenterLeft.m_dPositionY = l_svStart.m_dPositionY + (l_dHeight/2.0);

					//set Center Right
					m_svFigure.m_svCenterRight.m_dPositionX = l_svStart.m_dPositionX + l_dWidth;
					m_svFigure.m_svCenterRight.m_dPositionY = l_svStart.m_dPositionY + (l_dHeight/2.0);

					//set Bottom Left
					m_svFigure.m_svBottomLeft.m_dPositionX = l_svStart.m_dPositionX;
					m_svFigure.m_svBottomLeft.m_dPositionY = l_svStart.m_dPositionY + l_dHeight;

					//Set Bottom Center
					m_svFigure.m_svBottomCenter.m_dPositionX = l_svStart.m_dPositionX + (l_dWidth/2.0);
					m_svFigure.m_svBottomCenter.m_dPositionY = l_svStart.m_dPositionY + (l_dHeight/2.0);

					//Set Bottom Right
					m_svFigure.m_svBottomRight.m_dPositionX = l_svStart.m_dPositionX + l_dWidth;
					m_svFigure.m_svBottomRight.m_dPositionY = l_svStart.m_dPositionY + l_dHeight;
					//l_hrOk = TranslateFromOutputSpace( m_svFigure, m_svFigure );
				
				}

				break;
			}

			case SVExtentTranslationLinear:
			{
				double l_dWidth = 0.0;
				double l_dHeight = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );				

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight );				
				}

				if ( l_hrOk == S_OK )
				{
					m_svFigure.m_eShape = SVExtentShapeArrow;

					m_svFigure.m_svTopLeft.m_dPositionY = l_dHeight / 2 - 10;

					m_svFigure.m_svBottomLeft.m_dPositionY = l_dHeight / 2 + 10;

					m_svFigure.m_svCenterLeft.m_dPositionY = l_dHeight / 2;

					m_svFigure.m_svTopCenter.m_dPositionY = l_dHeight / 2;
					m_svFigure.m_svTopCenter.m_dPositionX = (long)( l_dWidth / 2 );

					m_svFigure.m_svBottomCenter.m_dPositionY = l_dHeight / 2;
					m_svFigure.m_svBottomCenter.m_dPositionX = (long)( l_dWidth / 2 );

					m_svFigure.m_svTopRight.m_dPositionY = l_dHeight / 2 - 10;
					m_svFigure.m_svTopRight.m_dPositionX = (long)( l_dWidth - 10 );

					m_svFigure.m_svCenterRight.m_dPositionY = l_dHeight / 2;
					m_svFigure.m_svCenterRight.m_dPositionX = (long)( l_dWidth );

					m_svFigure.m_svBottomRight.m_dPositionY = l_dHeight / 2 + 10;
					m_svFigure.m_svBottomRight.m_dPositionX = (long)( l_dWidth - 10 );

					l_hrOk = TranslateFromOutputSpace( m_svFigure, m_svFigure );
				}

				break;
			}
			case SVExtentTranslationLine:
			{
				double l_dWidth = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth );				

				if ( l_hrOk == S_OK )
				{
					m_svFigure.m_eShape = SVExtentShapeArrow;

					m_svFigure.m_svTopLeft.m_dPositionY = -10;

					m_svFigure.m_svTopCenter.m_dPositionX = (long)( l_dWidth / 2 );

					m_svFigure.m_svTopRight.m_dPositionY = -10;
					m_svFigure.m_svTopRight.m_dPositionX = (long)( l_dWidth - 10 );

					m_svFigure.m_svCenterRight.m_dPositionX = (long)( l_dWidth );

					m_svFigure.m_svBottomLeft.m_dPositionY = 10;

					m_svFigure.m_svBottomCenter.m_dPositionX = (long)( l_dWidth / 2 );

					m_svFigure.m_svBottomRight.m_dPositionY = 10;
					m_svFigure.m_svBottomRight.m_dPositionX = (long)( l_dWidth - 10 );

					l_hrOk = TranslateFromOutputSpace( m_svFigure, m_svFigure );
				}

				break;
			}
			case SVExtentTranslationPolarUnwrap:
			{
				SVExtentPointStruct l_svCenterPoint;

				double l_dStartAngle = 0.0;
				double l_dEndAngle = 0.0;
				double l_dInnerRadius = 0.0;
				double l_dOuterRadius = 0.0;

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, l_svCenterPoint );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle );				
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyInnerRadius, l_dInnerRadius );				
					if( l_dInnerRadius < 1 )
						l_dInnerRadius = 1;
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius );
					if( l_dOuterRadius < 1 )
						l_dOuterRadius = 1;
				}

				if ( l_hrOk == S_OK )
				{
					if ( l_dEndAngle - l_dStartAngle < 360.0 )
					{
						m_svFigure.m_eShape = SVExtentShapePie;
					}
					else if ( l_dEndAngle - l_dStartAngle < 720.0 )
					{
						m_svFigure.m_eShape = SVExtentShapeCircle;
					}
					else
					{
						m_svFigure.m_eShape = SVExtentShapeDoubleCircle;
					}

					m_svFigure.m_svTopLeft = l_svCenterPoint;
					m_svFigure.m_svTopCenter = l_svCenterPoint;
					m_svFigure.m_svTopRight = l_svCenterPoint;

					m_svFigure.m_svBottomRight = SVRotateAndFlipPoint( l_svCenterPoint, l_dOuterRadius, l_dStartAngle );
					m_svFigure.m_svBottomLeft = SVRotateAndFlipPoint( l_svCenterPoint, l_dOuterRadius, l_dEndAngle );
					m_svFigure.m_svCenterRight = SVRotateAndFlipPoint( l_svCenterPoint, l_dInnerRadius, l_dStartAngle );
					m_svFigure.m_svCenterLeft = SVRotateAndFlipPoint( l_svCenterPoint, l_dInnerRadius, l_dEndAngle );
				}

				break;
			}
			case SVExtentTranslationVerticalPerspective:
			{
				//SVExtentPointStruct;
				double l_dWidth;
				double l_dHeight;
				double l_dYPosition;
				if( m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth ) == S_OK &&
					m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight ) == S_OK &&
					m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dYPosition ) == S_OK )
				{
					m_svFigure.m_eShape = SVExtentShapeHexagon;

					m_svFigure.m_svTopLeft =  SVExtentPointStruct( 0,        l_dYPosition );
					m_svFigure.m_svTopRight = SVExtentPointStruct( l_dWidth - 1, l_dYPosition );

					m_svFigure.m_svBottomLeft =  SVExtentPointStruct( 0,        l_dHeight - l_dYPosition -1);
					m_svFigure.m_svBottomRight = SVExtentPointStruct( l_dWidth - 1, l_dHeight - l_dYPosition -1);

					m_svFigure.m_svBottomCenter = SVExtentPointStruct( l_dWidth/2, l_dHeight - 1 );
					m_svFigure.m_svTopCenter =    SVExtentPointStruct( l_dWidth/2, 1 );

					m_svFigure.m_svCenterRight = SVExtentPointStruct( l_dWidth - 1, l_dHeight/2 );
					m_svFigure.m_svCenterLeft =  SVExtentPointStruct( 0,        l_dHeight/2 );
				}
				break;
			}
			case SVExtentTranslationHorizontalPerspective:
			{
				//SVExtentPointStruct;
				double l_dWidth;
				double l_dHeight;
				double l_dXPosition;
				if( m_svDimensions.GetExtentProperty( SVExtentPropertyWidth, l_dWidth ) == S_OK &&
					m_svDimensions.GetExtentProperty( SVExtentPropertyHeight, l_dHeight ) == S_OK &&
					m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dXPosition ) == S_OK )
				{
					m_svFigure.m_eShape = SVExtentShapeHexagon;

					m_svFigure.m_svTopLeft =  SVExtentPointStruct( l_dXPosition,  0 );
					m_svFigure.m_svTopRight = SVExtentPointStruct( l_dWidth - l_dXPosition - 1, 0 );

					m_svFigure.m_svBottomLeft =  SVExtentPointStruct( l_dXPosition,        l_dHeight - 1);
					m_svFigure.m_svBottomRight = SVExtentPointStruct( l_dWidth - l_dXPosition - 1, l_dHeight - 1);

					m_svFigure.m_svBottomCenter = SVExtentPointStruct( l_dWidth/2, l_dHeight - 1 );
					m_svFigure.m_svTopCenter =    SVExtentPointStruct( l_dWidth/2, 1 );

					m_svFigure.m_svCenterRight = SVExtentPointStruct( l_dWidth - 1, l_dHeight/2 );
					m_svFigure.m_svCenterLeft =  SVExtentPointStruct( 0,        l_dHeight/2 );
				}
				break;
			}
			default:
			{
				l_hrOk = S_FALSE;

				break;
			}
		}

		if ( l_hrOk != S_OK )
		{
			m_svFigure.Initialize();
		}
	}

	return l_hrOk;
}

HRESULT  SVImageExtentClass::UpdateVerticalPerspective( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPosition;

	double l_dY = p_svStart.m_dPositionY - p_svEnd.m_dPositionY;

	if ( m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK )
	{
		switch( p_eLocation )
		{
			case SVExtentLocationPropertyTop:
			case SVExtentLocationPropertyTopRight:
			case SVExtentLocationPropertyTopLeft:
			{
				l_svPosition.m_dPositionY -= l_dY;
				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyBottom:
			case SVExtentLocationPropertyBottomRight:
			case SVExtentLocationPropertyBottomLeft:
			{
				l_svPosition.m_dPositionY += l_dY;
				l_hrOk = S_OK;
				break;
			}
		}

		if ( l_hrOk == S_OK )
		{
			if ( SetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK )
			{
				l_hrOk = UpdateData();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}

HRESULT  SVImageExtentClass::UpdateHorizontalPerspective( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPosition;

	double l_dX = p_svStart.m_dPositionX - p_svEnd.m_dPositionX;

	if ( m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK )
	{
		switch( p_eLocation )
		{
			case SVExtentLocationPropertyBottomLeft:
			case SVExtentLocationPropertyTopLeft:
			case SVExtentLocationPropertyLeft:
			{
				l_svPosition.m_dPositionX -= l_dX;
				l_hrOk = S_OK;

				break;
			}
			case SVExtentLocationPropertyRight:
			case SVExtentLocationPropertyBottomRight:
			case SVExtentLocationPropertyTopRight:
			{
				l_svPosition.m_dPositionX += l_dX;
				l_hrOk = S_OK;
				break;
			}
		}

		if ( l_hrOk == S_OK )
		{
			if ( SetExtentProperty( SVExtentPropertyTranslationOffset, l_svPosition ) == S_OK )
			{
				l_hrOk = UpdateData();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}


HRESULT SVImageExtentClass::GetTitlePoint(POINT &p_ptTitlePoint) const
{
	HRESULT l_hrOk = S_OK;
	SVExtentPointStruct l_svPointStruct;
	l_svPointStruct = p_ptTitlePoint;

	l_hrOk = GetTitlePoint( l_svPointStruct );

	if ( l_hrOk == S_OK )
	{
		p_ptTitlePoint = l_svPointStruct.operator POINT();
	}


	return l_hrOk;
}

HRESULT SVImageExtentClass::GetTitlePoint(SVExtentPointStruct &p_svTitlePoint) const
{
	HRESULT l_hrOk = S_OK;

	SVExtentPointStruct svPositionPoint;

	switch( m_eTranslation )
	{
		// Place the Title above the top of the ROI rectangle
		case SVExtentTranslationResize:
		{
			//@WARNING [Jim][8 July 2015] No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.
			l_hrOk = m_svPosition.GetExtentProperty(SVExtentPropertyPositionPoint, svPositionPoint);

			if (S_OK == l_hrOk )
			{
				p_svTitlePoint.m_dPositionY = (svPositionPoint.m_dPositionY - 10);
				p_svTitlePoint.m_dPositionX = (svPositionPoint.m_dPositionX + 5);
			}
			break;
		}
		// These cases place the Title below the top of the ROI rectangle
		case SVExtentTranslationDoubleHeight:
		case SVExtentTranslationFlipHorizontal:
		case SVExtentTranslationFlipVertical:
		case SVExtentTranslationFigureShift:
		{
			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyPositionPoint, svPositionPoint);

			if( l_hrOk == S_OK )
			{
				p_svTitlePoint.m_dPositionY = (svPositionPoint.m_dPositionY + 5 );
				p_svTitlePoint.m_dPositionX = svPositionPoint.m_dPositionX + 5;
			}
			break;
		}

		case SVExtentTranslationHorizontalPerspective:
		{
			l_hrOk = this->m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffset, svPositionPoint);

			if( l_hrOk == S_OK )
			{
				p_svTitlePoint.m_dPositionY = svPositionPoint.m_dPositionY + 5;
				p_svTitlePoint.m_dPositionX = svPositionPoint.m_dPositionX + 5;
			}

			break;
		}
		case SVExtentTranslationVerticalPerspective:
		{
			l_hrOk = m_svPosition.GetExtentProperty( SVExtentPropertyTranslationOffset, svPositionPoint);

			if( l_hrOk == S_OK )
			{
				p_svTitlePoint.m_dPositionY = svPositionPoint.m_dPositionY + 5;
				p_svTitlePoint.m_dPositionX = svPositionPoint.m_dPositionX + 5;
			}

			break;
		}
		// These cases place the Title above the top of the ROI rectangle
		case SVExtentTranslationShift:
		case SVExtentTranslationRotate:
		case SVExtentTranslationFlippedRotate:
		case SVExtentTranslationProfileShift:
		case SVExtentTranslationTransformShift:
		case SVExtentTranslationTransformRotate:
		case SVExtentTranslationPolarUnwrap:
		case SVExtentTranslationBuildReference:
		case SVExtentTranslationLine:
		case SVExtentTranslationLinear:
		case SVExtentTranslationProfile:
		{
			p_svTitlePoint.m_dPositionX = 0.0;
			p_svTitlePoint.m_dPositionY = -10.0;

			l_hrOk = TranslateFromOutputSpace( p_svTitlePoint, p_svTitlePoint );

			break;
		}
		default:
		{
			l_hrOk = S_FALSE;//everything should have an extent type

			break;
		}
	}

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageExtentClass.cpp_v  $
 * 
 *    Rev 1.1   24 Sep 2013 15:47:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  860
 * SCR Title:  Resize Icons Should Change Based on a Tool's Rotation
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Increased size of region for rotating pick area.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:53:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   11 Jan 2013 14:57:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new methods to compare image information that either includes positional information or excludes positional information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   10 Jan 2013 15:31:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added missing log entry.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   07 Dec 2012 10:31:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   TranslateToOutputSpace : added new case for SVExtentTranslationShift
 * 
 * TranslateFromOutputSpace: added new case for SVExtentTranslationShift
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   04 Oct 2012 09:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34.1.0   05 Dec 2012 11:45:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   TranslateToOutputSpace:
 * added new case for SVExtentTranslationShift
 * TranslateFromOutputSpace:
 * added new case for SVExtentTranslationShift
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   16 Jul 2012 15:44:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Get Title Point to a const method since it does not changes any internal attributes and allows it to be called by a const object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   16 Jul 2012 08:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated attribute names and updated methods to const to make variables more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   02 Jul 2012 15:59:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   16 Mar 2011 13:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   10 Feb 2011 13:48:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   28 Oct 2010 13:32:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   20 Jun 2007 11:09:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   20 Jan 2006 11:27:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added case for each of the following extent types in UpdateSouceOffset(
 * SVExtentTranslationDoubleHeight.
 * SVExtentTranslationFlipVertical
 * SVExtentTranslationFlipHorizontal
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   16 Jan 2006 16:49:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVImageExtentClass to use new SVExtentOffsetStruct for managing the extent offset data. Fixed check in problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2006 16:40:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVImageExtentClass to use new SVExtentOffsetStruct for managing the extent offset data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   15 Sep 2005 10:18:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified UpdateSourceOffset for warp tools.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   13 Sep 2005 09:43:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated UpdateSourceData method to handle the TransformShift translation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   12 Sep 2005 07:50:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified UpdateSourceOffset
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   09 Sep 2005 14:43:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified UpdateSourceOffset to have a valid case for warp tools.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   02 Sep 2005 12:23:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated profile/linear tool height x 2 when updating the extents via the Update method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   01 Sep 2005 12:21:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added UpdateSourceOffset(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   10 Aug 2005 12:13:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed GetLocationProperty and Update 
 * for translation type SVExtentTranslationFigureShift.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   09 Aug 2005 15:48:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update translation for transformation tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   08 Aug 2005 10:37:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed TranslateFromOutputSpace for PolarUnwrap Type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   01 Aug 2005 14:11:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  502
 * SCR Title:  Fix Draw Figure Issue in Polar Unwrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Build Figure
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   01 Aug 2005 09:14:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated BuildFigure and translate functions for handling the tool figure and translation methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   27 Jul 2005 08:56:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified TranslateToOutputSpace, TranslateFromOutputSpace, BuildOutputDimensions for SVExtentTranslationCylindricalWarp?.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   07 Jul 2005 13:54:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug in Build Figure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   28 Jun 2005 16:59:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified Build Figure for Double Height Translation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   20 Jun 2005 10:02:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Jun 2005 13:38:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   using new SVExtentTranslationFigureShift translation type and new method GetTitlePoint
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Jun 2005 13:38:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   using new SVExtentTranslationFigureShift translation type and new method GetTitlePoint
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   20 May 2005 13:30:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new translation types
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 May 2005 14:39:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved SVPiClass to SVGeometryClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Mar 2005 09:32:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change Translate method to handle rotate and flip correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Mar 2005 09:03:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Mar 2005 14:37:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated methods to use predetermined sin and consine values at 0, 90, 180, 360, ... degrees.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Mar 2005 14:41:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change for Perspective Tool Hexagon Shape
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2005 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support the new Perspective Warp Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2005 11:52:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CylindricalWarp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 09:40:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   modified public functions returning HRESULT to use SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
