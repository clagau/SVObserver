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
//Moved to precompiled header: #include <math.h>
//Moved to precompiled header: #include <map>
#include "SVImageExtentClass.h"
#include "SVGeometryClass.h"
#include "SVFileSystemLibrary\SVFileNameClass.h"
#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"

struct ExtentPropertyMapInit
{
	ExtentPropertyMapInit();
	SVExtentPropertyMapType map;
};

ExtentPropertyMapInit g_svExtentPropertyMap;

ExtentPropertyMapInit::ExtentPropertyMapInit()
{
	map[ SvDef::SVExtentPropertyPositionPointX ]        = _T("X Position");
	map[ SvDef::SVExtentPropertyPositionPointY ]        = _T("Y Position");
	map[ SvDef::SVExtentPropertyTranslationOffsetX ]    = _T("X Offset");
	map[ SvDef::SVExtentPropertyTranslationOffsetY ]    = _T("Y Offset");
	map[ SvDef::SVExtentPropertyRotationAngle ]         = _T("Rotation Angle");
	map[ SvDef::SVExtentPropertyOutputPositionPointX ]  = _T("Output X Position");
	map[ SvDef::SVExtentPropertyOutputPositionPointY ]  = _T("Output Y Position");
	map[ SvDef::SVExtentPropertyWidth ]        = _T("Width");
	map[ SvDef::SVExtentPropertyHeight ]       = _T("Height");
	map[ SvDef::SVExtentPropertyStartAngle ]   = _T("Start Angle");
	map[ SvDef::SVExtentPropertyEndAngle ]     = _T("End Angle");
	map[ SvDef::SVExtentPropertyInnerRadius ]  = _T("Inner Radius");
	map[ SvDef::SVExtentPropertyOuterRadius ]  = _T("Outer Radius");
	map[ SvDef::SVExtentPropertyOutputWidth ]  = _T("Width");
	map[ SvDef::SVExtentPropertyOutputHeight ] = _T("Height");
};

std::string SVImageExtentClass::GetExtentPropertyName( SvDef::SVExtentPropertyEnum p_eProperty )
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

	m_eTranslation = SvDef::SVExtentTranslationUnknown;

	if ( S_OK != m_svPosition.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svDimensions.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != ClearOutputData() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SvDef::SVExtentTranslationEnum SVImageExtentClass::GetTranslation() const
{
	return m_eTranslation;
}

HRESULT SVImageExtentClass::SetTranslation( SvDef::SVExtentTranslationEnum p_eTranslation )
{
	HRESULT l_hrOk = S_FALSE;

	switch( p_eTranslation )
	{
		case SvDef::SVExtentTranslationUnknown:
		case SvDef::SVExtentTranslationNone:
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationPolarUnwrap:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationCylindricalWarpH:
		case SvDef::SVExtentTranslationCylindricalWarpV:
		case SvDef::SVExtentTranslationVerticalPerspective:
		case SvDef::SVExtentTranslationHorizontalPerspective:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFigureShift:
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
		case SvDef::SVExtentTranslationNone:
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFigureShift:
		{
			m_svDimensions = p_svDimensions;
			l_hrOk = ClearOutputData();
			break;
		}
		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			m_svDimensions = p_svDimensions;

			double dEndAngle = 0.0;
			if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyEndAngle, dEndAngle ))
			{
				l_hrOk = SetExtentProperty( SvDef::SVExtentPropertyEndAngle, dEndAngle );
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
		case SvDef::SVExtentTranslationNone:
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFigureShift:
		{
			m_svPosition = p_svPosition;
			l_hrOk = ClearOutputData();
			break;
		}
		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			m_svPosition = p_svPosition;
			double dRotationAngle = 0.0;
			if ( S_OK  == GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, dRotationAngle ))
			{
				l_hrOk = SetExtentProperty( SvDef::SVExtentPropertyRotationAngle, dRotationAngle );
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

HRESULT SVImageExtentClass::DisableExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty )
{
	HRESULT l_hrOk = m_svPosition.DisableExtentProperty( p_eProperty );

	if ( S_OK == m_svDimensions.DisableExtentProperty( p_eProperty ) )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	double l_dValue = 0.0;

	l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( S_OK == l_hrOk )
	{
		p_rlValue = (long)l_dValue;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	if( ( p_eProperty & ~( SvDef::SVExtentPropertyPositionsAll ) ) == 0 )
	{
		l_hrOk = m_svPosition.GetExtentProperty( p_eProperty, p_rdValue );
	}
	else if( ( p_eProperty & ~( SvDef::SVExtentPropertyDimensionsAll ) ) == 0 )
	{
		l_hrOk = m_svDimensions.GetExtentProperty( p_eProperty, p_rdValue );
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svValue;

	l_hrOk = GetExtentProperty( p_eProperty, l_svValue );

	if ( S_OK == l_hrOk )
	{
		p_roValue = l_svValue;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svPosition.GetExtentProperty( p_eProperty, p_rsvValue );

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetExtentPropertyList( SvDef::SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyMapType& p_rPropertyMap ) const
{
	p_rPropertyMap.clear();
	const SVExtentPropertyMapType& rMap = g_svExtentPropertyMap.map;

	SVExtentPropertyMapType::const_iterator iter;
	for ( iter = rMap.begin(); iter != rMap.end(); ++iter )
	{
		bool bPositionEnabled   = m_svPosition  .IsEnabled( (SvDef::SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		bool bDimensionsEnabled = m_svDimensions.IsEnabled( (SvDef::SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		if ( bPositionEnabled || bDimensionsEnabled )
			p_rPropertyMap.insert( *iter );
	}
	return S_OK;
}

HRESULT SVImageExtentClass::GetExtentPropertyList( SvDef::SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyListType& p_rPropertyList ) const
{
	const SVExtentPropertyMapType& rMap = g_svExtentPropertyMap.map;

	SVExtentPropertyMapType::const_iterator iter;
	for ( iter = rMap.begin(); iter != rMap.end(); ++iter )
	{
		bool bPositionEnabled   = m_svPosition  .IsEnabled( (SvDef::SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		bool bDimensionsEnabled = m_svDimensions.IsEnabled( (SvDef::SVExtentPropertyEnum) (iter->first & p_eWhichProperties) );
		if ( bPositionEnabled || bDimensionsEnabled )
			p_rPropertyList.push_back( iter->first );
	}
	return S_OK;
}

HRESULT SVImageExtentClass::SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	SvDef::SVExtentPropertyEnum l_ePosProperty = (SvDef::SVExtentPropertyEnum)(p_eProperty & SvDef::SVExtentPropertyPositionsAll);
	SvDef::SVExtentPropertyEnum l_eDimProperty = (SvDef::SVExtentPropertyEnum)(p_eProperty & SvDef::SVExtentPropertyDimensionsAll);

	if( l_ePosProperty != 0 )
	{
		l_hrOk = S_OK;

		// special case to handle polar unwrap
		if ( m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
		{
			if ( p_eProperty & SvDef::SVExtentPropertyRotationAngle )
			{
				double dEndAngle = 0.0;
				double dStartAngle = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyEndAngle, dEndAngle );
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, dStartAngle );
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyStartAngle,
					                                     dStartAngle + (p_dValue - dEndAngle) );
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyEndAngle, p_dValue );
				}
			}
		}

		HRESULT hrSet = m_svPosition.SetExtentProperty( l_ePosProperty, p_dValue );
		if ( S_OK == l_hrOk )
		{
			l_hrOk = hrSet;
		}
	}

	if( l_eDimProperty != 0 )
	{
		// special case to handle polar unwrap rotation
		if ( m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
		{
			if ( p_eProperty & SvDef::SVExtentPropertyEndAngle )
			{
				HRESULT hrSet = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyRotationAngle, p_dValue );
			}
		}

		if ( S_OK == m_svDimensions.SetExtentProperty( l_eDimProperty, p_dValue ) )
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

	if ( S_OK != ClearOutputData() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svPosition.SetExtentProperty( p_eProperty, p_svValue );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = ClearOutputData();
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::UpdateData()
{
	HRESULT l_hrOk = BuildOutputDimensions();

	if ( S_OK == l_hrOk )
	{
		l_hrOk = BuildFigure();
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::GetFigure( SVExtentFigureStruct &p_rsvFigure ) const
{
	HRESULT l_hrOk = p_rsvFigure.Initialize();

	if ( S_OK == l_hrOk )
	{
		switch( m_svFigure.m_eShape )
		{
			case SvDef::SVExtentShapePoint:
			case SvDef::SVExtentShapeArrow:
			case SvDef::SVExtentShapeLine:
			case SvDef::SVExtentShapeLineHorizontal:
			case SvDef::SVExtentShapeLineVertical:
			case SvDef::SVExtentShapeRectangle:
			case SvDef::SVExtentShapePie:
			case SvDef::SVExtentShapeCircle:
			case SvDef::SVExtentShapeDoubleCircle:
			case SvDef::SVExtentShapeHexagon:
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

SvDef::SVExtentLocationPropertyEnum SVImageExtentClass::GetLocationPropertyAt( SVExtentPointStruct p_svPoint )
{
	SvDef::SVExtentLocationPropertyEnum l_eLocation = SvDef::SVExtentLocationPropertyUnknown;

	SVExtentPointStruct l_svOutputPoint;

	RECT l_oRect;
	bool l_bValid = false;

	if ( m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
	{
		double l_dValue;
		m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPointX, l_dValue);
		l_oRect.left = static_cast< long >( l_dValue - 2 );
		l_oRect.right = static_cast< long >( l_dValue + 2 );
		m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPointY, l_dValue);
		l_oRect.top = static_cast< long >( l_dValue - 2 );
		l_oRect.bottom = static_cast< long >( l_dValue + 2 );
		if ( ::PtInRect( &l_oRect, p_svPoint ) )
		{
			l_eLocation = SvDef::SVExtentLocationPropertyCenter;
		}
	}

	switch( m_eTranslation )
	{
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFigureShift:
		{
			l_svOutputPoint = p_svPoint;
			l_bValid = S_OK == GetRectangle( l_oRect );
			break;
		}

		default:
		{
			l_bValid = S_OK == TranslateToOutputSpace( p_svPoint, l_svOutputPoint ) &&
				S_OK == GetOutputRectangle( l_oRect );
		}
	}

	if ( l_bValid )
	{
		RECT l_oTemp = l_oRect;

		// Check special case for Polar Unwrap tool when > 360
		if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown && m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
		{
			double l_dStartAngle, l_dEndAngle, l_dOuterRadius, l_ldInnerRadius;

			if( S_OK == GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle) &&
			S_OK == GetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle) &&
			S_OK == GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_ldInnerRadius )  &&
			l_dEndAngle - l_dStartAngle > 360 )
			{
				GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius );
				double l_dCircum =  g_svPi.GetCircumference( l_dOuterRadius );
				if( l_oRect.right > l_dCircum && l_oRect.right > 4 )
				{
					l_oTemp.right = l_oRect.right - (long)l_dCircum;
					l_oTemp.left = l_oTemp.right;
					l_oTemp.top = (long)-l_ldInnerRadius;
					if ( ::InflateRect( &l_oTemp, 2, -2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SvDef::SVExtentLocationPropertyRight;
					}
				}
			}
		}


		long l_lInflateX = -2;
		long l_lInflateY = -2;

		if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
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
				if( m_eTranslation == SvDef::SVExtentTranslationRotate ||
				    m_eTranslation == SvDef::SVExtentTranslationFlippedRotate ||
				    m_eTranslation == SvDef::SVExtentTranslationProfile )
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
						l_eLocation = SvDef::SVExtentLocationPropertyRotate;
					}
				}

				if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
				{
					l_eLocation = SvDef::SVExtentLocationPropertyCenter;
				}
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationLine &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
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
				l_eLocation = SvDef::SVExtentLocationPropertyTop;
			}
		}

		if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
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
				l_eLocation = SvDef::SVExtentLocationPropertyRight;
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationLine &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
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
				l_eLocation = SvDef::SVExtentLocationPropertyBottom;
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationProfile &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
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
				l_eLocation = SvDef::SVExtentLocationPropertyLeft;
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationProfile &&
		     m_eTranslation != SvDef::SVExtentTranslationLine &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.right = l_oRect.left;
			l_oTemp.bottom = l_oRect.top;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTopLeft;
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationLine &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.left = l_oRect.right;
			l_oTemp.bottom = l_oRect.top;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTopRight;
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationProfile &&
		     m_eTranslation != SvDef::SVExtentTranslationLine &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.right = l_oRect.left;
			l_oTemp.top = l_oRect.bottom;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottomLeft;
			}
		}

		if ( m_eTranslation != SvDef::SVExtentTranslationLine &&
			   l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
		{
			l_oTemp = l_oRect;
		
			l_oTemp.left = l_oRect.right;
			l_oTemp.top = l_oRect.bottom;

			if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottomRight;
			}
		}

		if ( m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
		{
			long l_lInnerRadius = 0;

			if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_lInnerRadius ) )
			{
				if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
				{
					if ( 0 < l_oRect.right - 4 && 0 < l_lInnerRadius - 4 )
					{
						l_oTemp = l_oRect;
						l_oTemp.bottom = 0;
						l_oTemp.top = -l_lInnerRadius;
						l_svOutputPoint.m_dPositionX =  l_svOutputPoint.m_dPositionX ;

						if ( ::InflateRect( &l_oTemp, -2, -2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
						{
							l_eLocation = SvDef::SVExtentLocationPropertyCenter;
						}
					}
				}

				if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
				{
					l_oTemp = l_oRect;
				
					l_oTemp.bottom = -l_lInnerRadius;
					l_oTemp.top = -l_lInnerRadius;

					if ( ::InflateRect( &l_oTemp, 2, 2 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SvDef::SVExtentLocationPropertyCenter;
					}
				}

				if ( m_eTranslation != SvDef::SVExtentTranslationProfile &&
				     l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
				{
					l_oTemp = l_oRect;
				
					l_oTemp.right = l_oRect.left;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if ( ::InflateRect( &l_oTemp, 2, 0 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SvDef::SVExtentLocationPropertyLeft;
					}
				}

				if ( l_eLocation == SvDef::SVExtentLocationPropertyUnknown )
				{
					l_oTemp = l_oRect;
				
					l_oTemp.left = l_oRect.right;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if ( ::InflateRect( &l_oTemp, 2, 0 ) && ::PtInRect( &l_oTemp, l_svOutputPoint ) )
					{
						l_eLocation = SvDef::SVExtentLocationPropertyRight;
					}
				}
			}
		}
		if ( m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
		{
			double l_dOuterRadius, l_ldInnerRadius;
			if( l_eLocation == SvDef::SVExtentLocationPropertyTop ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottom )
			{
				if( S_OK == GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_ldInnerRadius ) &&
					S_OK == GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius ) )
				{
					if( l_dOuterRadius < l_ldInnerRadius )
					{
						if( l_eLocation == SvDef::SVExtentLocationPropertyTop )
						{
							l_eLocation = SvDef::SVExtentLocationPropertyBottom;
						}
						else
						{
							l_eLocation = SvDef::SVExtentLocationPropertyTop;
						}
					}
				}
			}
		}
		if( m_eTranslation == SvDef::SVExtentTranslationVerticalPerspective )
		{
			if( l_eLocation == SvDef::SVExtentLocationPropertyTopRight ||
				l_eLocation == SvDef::SVExtentLocationPropertyTopLeft)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTop;
			}
			if( l_eLocation == SvDef::SVExtentLocationPropertyBottomRight ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottomLeft )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottom;
			}
			if( l_eLocation == SvDef::SVExtentLocationPropertyCenter )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyUnknown;
			}
		}
		if( m_eTranslation == SvDef::SVExtentTranslationHorizontalPerspective )
		{
			if( l_eLocation == SvDef::SVExtentLocationPropertyTopRight ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottomRight )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyRight;
			}
			if( l_eLocation == SvDef::SVExtentLocationPropertyTopLeft ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottomLeft )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyLeft;
			}
			if( l_eLocation == SvDef::SVExtentLocationPropertyCenter )
			{
				l_eLocation = SvDef::SVExtentLocationPropertyUnknown;
			}
		}
	}

	return l_eLocation;
}

// Translate the point to be relative (local space).  Usually this relates to the ROI of the parent image.
HRESULT SVImageExtentClass::TranslateToLocalSpace(const SVExtentPointStruct& rValue, SVExtentPointStruct& rResult)
{
	SVExtentPointStruct svPosition;
	HRESULT hr = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, svPosition);

	if (S_OK == hr)
	{
		rResult.m_dPositionX = rValue.m_dPositionX - svPosition.m_dPositionX;
		rResult.m_dPositionY = rValue.m_dPositionY - svPosition.m_dPositionY;
		
		// optional...
		SVExtentPointStruct svDisplacement;
		if (S_OK == m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, svDisplacement))
		{
			rResult.m_dPositionX -= svDisplacement.m_dPositionX;
			rResult.m_dPositionY -= svDisplacement.m_dPositionY;
		}
	}
	return hr;
}

HRESULT SVImageExtentClass::Update( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap )
	{
		l_hrOk = UpdatePolar( p_eLocation, p_svStart, p_svEnd );
	}
	else if ( m_eTranslation == SvDef::SVExtentTranslationLine )
	{
		l_hrOk = UpdateLine( p_eLocation, p_svStart, p_svEnd );
	}
	else if( m_eTranslation == SvDef::SVExtentTranslationVerticalPerspective )
	{
		l_hrOk = UpdateVerticalPerspective( p_eLocation, p_svStart, p_svEnd );
	}
	else if( m_eTranslation == SvDef::SVExtentTranslationHorizontalPerspective )
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
			case SvDef::SVExtentTranslationResize:
			{
				//@WARNING [Jim][8 July 2015] No identification of error cases.
				// GetExtentProperty () only l_bValid is identified, which is 
				// not very helpful. Currently none of these cases give much 
				// help with error identification.
				
				// Get mouse start position relative to input image ROI.
				l_bValid = (S_OK == TranslateToLocalSpace(p_svStart, l_svOutputStart)) &&
				// Get mouse end/current position relative to input image ROI.
								(S_OK == TranslateToLocalSpace(p_svEnd, l_svOutputEnd)) &&
				// Get ROI rectangle.								
								(S_OK == GetRectangle(l_oRect));
								
								
				if (l_bValid)
				{
					if (p_eLocation == SvDef::SVExtentLocationPropertyBottomLeft)
					{
						l_svOutputEnd.m_dPositionY = p_svEnd.m_dPositionY;
					}
					else if (p_eLocation == SvDef::SVExtentLocationPropertyTopRight)
					{
						l_svOutputEnd.m_dPositionX = p_svEnd.m_dPositionX;
					}
					else if (p_eLocation != SvDef::SVExtentLocationPropertyTop &&
						p_eLocation != SvDef::SVExtentLocationPropertyLeft &&
						p_eLocation != SvDef::SVExtentLocationPropertyTopLeft)
					{	
						l_svOutputStart = p_svStart;
						l_svOutputEnd = p_svEnd;
					}
				}
				break;
			}
			// Note: These cases do not work correctly (cannot drag the left or top using the mouse in the gui), 
			// a possible solution might be to use the same logic as the case above (SvDef::SVExtentTranslationResize)
			// however for these cases the position (not on the GUI) is always fixed to 0,0, so that might not 
			// work. 
			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationFlipHorizontal:
			case SvDef::SVExtentTranslationFlipVertical:
			case SvDef::SVExtentTranslationFigureShift:
			{
				l_svOutputStart = p_svStart;
				l_svOutputEnd = p_svEnd;
				l_bValid = S_OK == GetRectangle( l_oRect );
				break;
			}

			default:
			{
				l_bValid = S_OK == TranslateToOutputSpace( p_svStart, l_svOutputStart ) &&
					S_OK == TranslateToOutputSpace( p_svEnd, l_svOutputEnd ) &&
					S_OK == GetOutputRectangle( l_oRect );
			}
		}

		if ( l_bValid )
		{
			switch( p_eLocation )
			{
				case SvDef::SVExtentLocationPropertyTopLeft:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SvDef::SVExtentLocationPropertyTopRight:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX - l_oRect.right, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SvDef::SVExtentLocationPropertyBottomRight:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX - l_oRect.right, (long)l_svOutputEnd.m_dPositionY - l_oRect.bottom );

					break;
				}
				case SvDef::SVExtentLocationPropertyBottomLeft:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY - l_oRect.bottom );

					break;
				}
				case SvDef::SVExtentLocationPropertyLeft:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SvDef::SVExtentLocationPropertyRight:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX - l_oRect.right, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SvDef::SVExtentLocationPropertyTop:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY );

					break;
				}
				case SvDef::SVExtentLocationPropertyBottom:
				{
					l_hrOk = UpdateFromOutputSpace( p_eLocation, (long)l_svOutputEnd.m_dPositionX, (long)l_svOutputEnd.m_dPositionY - l_oRect.bottom );

					break;
				}
				case SvDef::SVExtentLocationPropertyCenter:
				{
					long l_lX = (long)( p_svEnd.m_dPositionX - p_svStart.m_dPositionX );
					long l_lY = (long)( p_svEnd.m_dPositionY - p_svStart.m_dPositionY );

					l_hrOk = UpdateFromOutputSpace( p_eLocation, l_lX, l_lY );

					break;
				}
				case SvDef::SVExtentLocationPropertyRotate:
				{
					SVExtentPointStruct l_svPosition;

					if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) )
					{
						double l_dAngle = SVGetRotationAngle( l_svPosition, p_svEnd );

						if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle ) )
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

HRESULT SVImageExtentClass::UpdateFromOutputSpace( SvDef::SVExtentLocationPropertyEnum p_eLocation, long p_dX, long p_dY )
{
	HRESULT l_hrOk = S_FALSE;

	switch( m_eTranslation )
	{
		case SvDef::SVExtentTranslationLine:
		{
			SVExtentPointStruct l_svStart;
			SVExtentPointStruct l_svEnd;

			if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart ) &&
			     S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd ) )
			{
				switch( p_eLocation )
				{
					case SvDef::SVExtentLocationPropertyTop:
					{
						l_svStart.m_dPositionX += p_dX;
						l_svStart.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyLeft:
					{
						l_svStart.m_dPositionX += p_dX;
						l_svStart.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottom:
					{
						l_svEnd.m_dPositionX += p_dX;
						l_svEnd.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyRight:
					{
						l_svEnd.m_dPositionX += p_dX;
						l_svEnd.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyCenter:
					{
						l_svStart.m_dPositionX += p_dX;
						l_svStart.m_dPositionY += p_dY;

						l_svEnd.m_dPositionX += p_dX;
						l_svEnd.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
				}

				if ( S_OK == l_hrOk )
				{
					if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart ) &&
						S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd ) )
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
		case SvDef::SVExtentTranslationProfile:
		{
			SVExtentPointStruct l_svPosition;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
					 S_OK == GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight ) &&
					 S_OK == GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth ) )
			{
				switch( p_eLocation )
				{
					case SvDef::SVExtentLocationPropertyTopRight:
					{
						l_dWidth += p_dX;
						l_dHeight -= p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += p_dX;
						l_dHeight += p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyRight:
					{
						l_dWidth += p_dX;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyTop:
					{
						l_dHeight -= p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottom:
					{
						l_dHeight += p_dY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyCenter:
					{
						l_svPosition.m_dPositionX += p_dX;
						l_svPosition.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
				}

				if ( S_OK == l_hrOk )
				{
					if ( 0.0 < l_dWidth && 0.0 < l_dHeight &&
							 S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
							 S_OK == SetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight ) &&
							 S_OK == SetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth ) )
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
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFigureShift:
		{
			SVExtentPointStruct l_svPosition;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
					 S_OK == GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight ) &&
					 S_OK == GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth ) )
			{
				switch( p_eLocation )
				{
					case SvDef::SVExtentLocationPropertyTopLeft:
					{
						l_svPosition.m_dPositionX += p_dX;
						l_svPosition.m_dPositionY += p_dY;

						l_dWidth -= p_dX;
						l_dHeight -= p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyTopRight:
					{
						l_svPosition.m_dPositionY += p_dY;

						l_dWidth += p_dX;
						l_dHeight -= p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += p_dX;
						l_dHeight += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottomLeft:
					{
						l_svPosition.m_dPositionX += p_dX;

						l_dWidth -= p_dX;
						l_dHeight += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyLeft:
					{
						l_svPosition.m_dPositionX += p_dX;

						l_dWidth -= p_dX;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyRight:
					{
						l_dWidth += p_dX;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyTop:
					{
						l_svPosition.m_dPositionY += p_dY;

						l_dHeight -= p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottom:
					{
						l_dHeight += p_dY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyCenter:
					{
						l_svPosition.m_dPositionX += p_dX;
						l_svPosition.m_dPositionY += p_dY;

						l_hrOk = S_OK;

						break;
					}
				}

				if ( S_OK == l_hrOk )
				{
					if ( 0.0 < l_dWidth && 0.0 < l_dHeight &&
							 S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
							 S_OK == SetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight ) &&
							 S_OK == SetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth )  )
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

		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			l_hrOk = UpdatePolarFromOutputSpace( p_eLocation, p_dX, p_dY );
			break;
		}
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			switch( p_eLocation )
			{
				case SvDef::SVExtentLocationPropertyTop:
				case SvDef::SVExtentLocationPropertyBottom:
				case SvDef::SVExtentLocationPropertyCenter:
				{
					SVExtentPointStruct l_svPosition;

					if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
					{
						l_svPosition.m_dPositionY += p_dY;
						l_hrOk = S_OK;
					}

					if ( S_OK == l_hrOk )
					{
						if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
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
		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			switch( p_eLocation )
			{
				case SvDef::SVExtentLocationPropertyLeft:
				case SvDef::SVExtentLocationPropertyCenter:
				case SvDef::SVExtentLocationPropertyRight:
				{
					SVExtentPointStruct l_svPosition;

					if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
					{
						l_svPosition.m_dPositionX += p_dX;
						l_hrOk = S_OK;
					}

					if ( S_OK == l_hrOk )
					{
						if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
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

	l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyPositionPointX, l_oRect.left );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyPositionPointY, l_oRect.top );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyWidth, l_oRect.right );

		l_oRect.right += l_oRect.left;
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyHeight, l_oRect.bottom );

		l_oRect.bottom += l_oRect.top;
	}

	if ( S_OK == l_hrOk )
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

	l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyWidth, l_oRect.right );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyHeight, l_oRect.bottom );
	}

	if ( S_OK == l_hrOk )
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

	l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_oRect.right );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_oRect.bottom );
	}

	if ( S_OK == l_hrOk )
	{
		p_roRect = l_oRect;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateToOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult )
{
	HRESULT l_hrOk = p_rsvResult.Initialize();

	if ( S_OK == l_hrOk )
	{
		switch( m_eTranslation )
		{
			case SvDef::SVExtentTranslationNone:
			case SvDef::SVExtentTranslationFigureShift:
			{
				p_rsvResult = p_svValue;

				break;
			}
			case SvDef::SVExtentTranslationShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					SVExtentPointStruct l_svDisplacement;

					m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);

					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX -= l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svDisplacement.m_dPositionY;
				}

				break;
			}// end multiple case:
			case SvDef::SVExtentTranslationProfileShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;
				}

				break;
			}// end multiple case:
			case SvDef::SVExtentTranslationTransformShift:
			{
				SVExtentPointStruct l_svPosition;
				SVExtentPointStruct l_svDisplacement;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( S_OK == l_hrOk )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY - l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX -= l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svDisplacement.m_dPositionY;
				}

				break;
			}
			case SvDef::SVExtentTranslationRotate:
			case SvDef::SVExtentTranslationProfile:
			case SvDef::SVExtentTranslationLinear:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
				{
					p_rsvResult = SVRotatePoint( l_svRotation, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;
				}

				break;
			}// end mulitple case:
			case SvDef::SVExtentTranslationTransformRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;
				SVExtentPointStruct l_svDisplacement;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( S_OK == l_hrOk )
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
			case SvDef::SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;
				SVExtentPointStruct l_svOutputRotation;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
				{
					double l_dAngle = SVGetRotationAngle( l_svStart, l_svEnd );

					p_rsvResult = SVRotatePoint( l_svStart, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svStart.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svStart.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;
				}

				break;
			}// end case SvDef::SVExtentTranslationLine:
			case SvDef::SVExtentTranslationFlippedRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
				{
					p_rsvResult = SVRotateAndFlipPoint( l_svRotation, p_svValue, -l_dAngle );

					p_rsvResult.m_dPositionX -= l_svRotation.m_dPositionX;
					p_rsvResult.m_dPositionY -= l_svRotation.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svOutputRotation.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svOutputRotation.m_dPositionY;
				}

				break;
			}// end case SvDef::SVExtentTranslationFlippedRotate:

			case SvDef::SVExtentTranslationPolarUnwrap:
			{
				SVExtentPointStruct l_svCenterPoint;

				double l_dStartAngle = 0.0;
				double l_dOuterRadius = 0.0;
				double l_dInnerRadius = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svCenterPoint );				

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle );
				}

				if ( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationPolarUnwrap:

			case SvDef::SVExtentTranslationCylindricalWarpH:
			{
				double l_dInputWidth = 0.0;
				double l_dOutputWidth = 0.0;
				double l_dAngle = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dInputWidth );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dOutputWidth );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dAngle );
				}
				if( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationCylindricalWarpH:

			case SvDef::SVExtentTranslationCylindricalWarpV:
			{
				double l_dInputHeight = 0.0;
				double l_dOutputHeight = 0.0;
				double l_dAngle = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dInputHeight );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dOutputHeight );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dAngle );
				}
				if( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationCylindricalWarpV:
			case SvDef::SVExtentTranslationVerticalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dyOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dFullWidth );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dFullHeight );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetY, l_dyOffset );
				}
				if( S_OK == l_hrOk )
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
			case SvDef::SVExtentTranslationHorizontalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dxOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dFullWidth );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dFullHeight );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetX, l_dxOffset );
				}
				if( S_OK == l_hrOk )
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

			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationResize:
			{
				SVExtentPointStruct l_svPosition;
				double heightScaleFactor = 1.0;
				double widthScaleFactor = 1.0;

				//@WARNING [Jim][8 July 2015]  No identification of error cases.  
				// GetExtentProperty () only returns S_FALSE, which is not 
				// very helpful. Currently none of these cases give much help
				// with error identification.
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );
				if (S_OK == l_hrOk)
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor );
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidthScaleFactor, widthScaleFactor );
				}

				if ( S_OK == l_hrOk )
				{
					p_rsvResult.m_dPositionX = (p_svValue.m_dPositionX - l_svPosition.m_dPositionX) * widthScaleFactor;
					p_rsvResult.m_dPositionY = (p_svValue.m_dPositionY - l_svPosition.m_dPositionY) * heightScaleFactor;
				}

				break;
			}

			case SvDef::SVExtentTranslationFlipVertical:
			{
				SVExtentPointStruct l_svPosition;
				double l_dHeight = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );
	
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );
				}

				if ( S_OK == l_hrOk )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX - l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = (l_dHeight - 1) - (p_svValue.m_dPositionY - l_svPosition.m_dPositionY) ;
				}
			
				break;			
			}
			case SvDef::SVExtentTranslationFlipHorizontal:
			{
				SVExtentPointStruct l_svPosition;
				double l_dWidth = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );
				}

				if ( S_OK == l_hrOk )
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

	if ( S_OK == l_hrOk )
	{
		switch( m_eTranslation )
		{
			case SvDef::SVExtentTranslationNone:
			{
				p_rsvResult = p_svValue;

				l_hrOk = S_OK;

				break;
			}

			case SvDef::SVExtentTranslationFigureShift:
			case SvDef::SVExtentTranslationProfileShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX + l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY + l_svPosition.m_dPositionY;
				}

				break;
			}// end case multiple
			case SvDef::SVExtentTranslationShift:
			{
				SVExtentPointStruct l_svPosition;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					SVExtentPointStruct l_svDisplacement;

					m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);

					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX + l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY + l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svDisplacement.m_dPositionY;
				}

				break;
			}// end case multiple
			case SvDef::SVExtentTranslationTransformShift:
			{
				SVExtentPointStruct l_svPosition;
				SVExtentPointStruct l_svDisplacement;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( S_OK == l_hrOk )
				{
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX + l_svPosition.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY + l_svPosition.m_dPositionY;

					p_rsvResult.m_dPositionX += l_svDisplacement.m_dPositionX;
					p_rsvResult.m_dPositionY += l_svDisplacement.m_dPositionY;
				}

				break;
			}
			case SvDef::SVExtentTranslationRotate:
			case SvDef::SVExtentTranslationProfile:
			case SvDef::SVExtentTranslationLinear:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
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
			case SvDef::SVExtentTranslationTransformRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;
				SVExtentPointStruct l_svDisplacement;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);
				}

				if ( S_OK == l_hrOk )
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
			case SvDef::SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;
				SVExtentPointStruct l_svOutputRotation;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationLine:

			case SvDef::SVExtentTranslationFlippedRotate:
			{
				SVExtentPointStruct l_svRotation;
				SVExtentPointStruct l_svOutputRotation;

				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
				}

				if ( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationFlippedRotate:

			case SvDef::SVExtentTranslationPolarUnwrap:
			{
				SVExtentPointStruct l_svCenterPoint;

				double l_dStartAngle = 0.0;
				double l_dOuterRadius = 0.0;
				double l_dInnerRadius = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svCenterPoint );				

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle );
				}

				if ( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationPolarUnwrap:

			case SvDef::SVExtentTranslationCylindricalWarpH:
			{
				double l_dInputWidth = 0.0;
				double l_dWarpAngle = 0.0;
				double l_dOutputWidth = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dInputWidth );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dWarpAngle );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dOutputWidth );
				}
				if( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationCylindricalWarpH:

			case SvDef::SVExtentTranslationCylindricalWarpV:
			{
				double l_dInputHeight = 0.0;
				double l_dWarpAngle = 0.0;
				double l_dOutputHeight = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dInputHeight );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dWarpAngle );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dOutputHeight );
				}
				if( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationCylindricalWarpV:
			case SvDef::SVExtentTranslationVerticalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dyOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dFullWidth );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dFullHeight );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetY, l_dyOffset );
				}
				if( S_OK == l_hrOk )
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
			}// end case SvDef::SVExtentTranslationVerticalPerspective:

			case SvDef::SVExtentTranslationHorizontalPerspective:
			{
				double l_dFullHeight = 0.0;
				double l_dFullWidth = 0.0;
				double l_dxOffset = 0.0;
				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dFullWidth );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dFullHeight );
				}
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetX, l_dxOffset );
				}
				if( S_OK == l_hrOk )
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
					}
				}
				break;
			}// end case SvDef::SVExtentTranslationHorizontalPerspective:

			//new cases for Flip Horizontal & Vertical
			case SvDef::SVExtentTranslationFlipVertical:
			{
				SVExtentPointStruct l_svPosition;
				double l_dFullHeight = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );
				if( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dFullHeight );
				}
				if ( S_OK == l_hrOk ) 
				{ 
					p_rsvResult.m_dPositionY = (l_dFullHeight - 1) - p_svValue.m_dPositionY;
					p_rsvResult.m_dPositionX = p_svValue.m_dPositionX;
				}
				break;
			}
			case SvDef::SVExtentTranslationFlipHorizontal:
			{
				SVExtentPointStruct l_svPosition;
				double l_dFullWidth = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dFullWidth );
				}
				if ( S_OK == l_hrOk ) 
				{ 
					p_rsvResult.m_dPositionX = (l_dFullWidth - 1) - p_svValue.m_dPositionX;
					p_rsvResult.m_dPositionY = p_svValue.m_dPositionY;
				}
				break;
			}
			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationResize:
				{
					//@WARNING [Jim][8 July 2015] No identification of error cases.  
					// GetExtentProperty () only returns S_FALSE, which is not 
					// very helpful. Currently none of these cases give much help
					// with error identification.

					SVExtentPointStruct l_svPosition;

					double heightScaleFactor = 1.0;
					double widthScaleFactor = 1.0;

					l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, l_svPosition);
					if (S_OK == l_hrOk)
					{
						l_hrOk = m_svDimensions.GetExtentProperty(SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor);
					}

					if (S_OK == l_hrOk)
					{
						l_hrOk = m_svDimensions.GetExtentProperty(SvDef::SVExtentPropertyWidthScaleFactor, widthScaleFactor);
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
		case SvDef::SVExtentTranslationNone:
		{
			p_rsvResult = p_svValue;

			l_hrOk = S_OK;

			break;
		}
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationPolarUnwrap:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationVerticalPerspective:
		case SvDef::SVExtentTranslationHorizontalPerspective:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFigureShift:
		{
			switch ( p_svValue.m_eShape )
			{
				case SvDef::SVExtentShapeArrow:
				{
					l_hrOk = S_OK;

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, -90.0, p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 90.0, p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight, -135.0, p_svValue.m_svTopRight, p_rsvResult.m_svTopRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight, 135.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SvDef::SVExtentShapeRectangle:
				{
					l_hrOk = S_OK;

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 0.0, p_svValue.m_svTopRight, p_rsvResult.m_svTopRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 90.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 90.0, p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svTopRight, p_rsvResult.m_svTopRight, 90.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svTopRight, p_rsvResult.m_svTopRight, 90.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft, 0.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateLineFromOutputSpace( p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft, 0.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SvDef::SVExtentShapeLineHorizontal:
				{
					l_hrOk = S_OK;

					SVExtentPointStruct l_svRotatePoint;
					SVExtentPointStruct l_svOutputRotationPoint;

					GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotationPoint );

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
				case SvDef::SVExtentShapeLineVertical:
				{
					l_hrOk = S_OK;

					SVExtentPointStruct l_svRotatePoint;
					SVExtentPointStruct l_svOutputRotationPoint;

					GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotationPoint );

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
				case SvDef::SVExtentShapeLine:
				case SvDef::SVExtentShapePoint:
				case SvDef::SVExtentShapePie:
				case SvDef::SVExtentShapeCircle:
				case SvDef::SVExtentShapeDoubleCircle:
				case SvDef::SVExtentShapeHexagon:
				{
					l_hrOk = S_OK;

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft ) )
					{
						l_hrOk = S_FALSE;
					}

					if ( S_OK != TranslateFromOutputSpace( p_svValue.m_svTopRight, p_rsvResult.m_svTopRight ) )
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
		case SvDef::SVExtentTranslationNone:
		{
			p_rsvResult = p_svValue;

			l_hrOk = S_OK;

			break;
		}
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationPolarUnwrap:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFigureShift:
		{
			long l_lCount = static_cast<long> (p_svValue.m_svPointArray.size());

			l_hrOk = p_rsvResult.Initialize();

			if( S_OK == l_hrOk && 0 < l_lCount )
			{
				double l_dAngle = 0.0;

				bool l_bIsAngle = S_OK == GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );

				p_rsvResult.m_bIsAngleValid = p_svValue.m_bIsAngleValid;
				p_rsvResult.m_dAngle = p_svValue.m_dAngle;

				if( ! l_bIsAngle || l_dAngle == 0.0 || ! p_svValue.m_bIsAngleValid )
				{
					for( long i = 0; S_OK == l_hrOk && i < l_lCount; i++ )
					{
						SVExtentPointStruct svPoint = p_svValue.m_svPointArray[ i ];

						if( S_OK == TranslateFromOutputSpace( svPoint, svPoint ) )
						{
							p_rsvResult.m_svPointArray.push_back( svPoint );
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

					GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotationPoint );

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

					if( S_OK == TranslateFromOutputSpace( l_svRadiusPoint, l_svRotatePoint ) )
					{
						if( l_lStart == 1 )
						{
							p_rsvResult.m_svPointArray.push_back( l_svRotatePoint );
						}
					}
					else
					{
						l_hrOk = S_FALSE;
					}

					for( long i = l_lStart; S_OK == l_hrOk && i < l_lCount; i++ )
					{
						SVExtentPointStruct l_svPoint = p_svValue.m_svPointArray[i];

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

						if( S_OK == TranslateLineFromOutputSpace( l_svRadiusPoint, l_svRotatePoint, l_dNewAngle, l_svPoint, l_svPoint ) )
						{
							p_rsvResult.m_svPointArray.push_back( l_svPoint );
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

	if( S_OK == l_hrOk )
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
		case SvDef::SVExtentTranslationNone:
		{
			p_rsvResult = p_svValue;

			l_hrOk = S_OK;

			break;
		}
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationPolarUnwrap:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFigureShift:
		{
			long l_lCount = static_cast<long> (p_svValue.m_svLineArray.size());

			l_hrOk = p_rsvResult.Initialize();

			if( S_OK != TranslateFromOutputSpace( p_svValue.m_StringPoint, p_rsvResult.m_StringPoint ) )
			{
				l_hrOk = S_FALSE;
			}

			for( long i = 0; S_OK == l_hrOk && i < l_lCount; i++ )
			{
				SVExtentLineStruct svLine = p_svValue.m_svLineArray[i];

				if( S_OK == TranslateFromOutputSpace( svLine, svLine ) )
				{
					p_rsvResult.m_svLineArray.push_back( svLine );
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}

			break;
		}
	}

	if( S_OK == l_hrOk )
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

	if ( S_OK == l_hrOk )
	{
		switch( m_eTranslation )
		{
			case SvDef::SVExtentTranslationNone:
			case SvDef::SVExtentTranslationShift:
			case SvDef::SVExtentTranslationProfileShift:
			case SvDef::SVExtentTranslationTransformShift:
			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationResize:
			case SvDef::SVExtentTranslationCylindricalWarpH:
			case SvDef::SVExtentTranslationCylindricalWarpV:
			case SvDef::SVExtentTranslationVerticalPerspective:
			case SvDef::SVExtentTranslationHorizontalPerspective:
			case SvDef::SVExtentTranslationPolarUnwrap:
			case SvDef::SVExtentTranslationFlipHorizontal:
			case SvDef::SVExtentTranslationFlipVertical:
			case SvDef::SVExtentTranslationFigureShift:
			{
				l_hrOk = TranslateFromOutputSpace( p_svValue, p_rsvResult );

				break;
			}// end case multiple

			case SvDef::SVExtentTranslationRotate:
			case SvDef::SVExtentTranslationProfile:
			case SvDef::SVExtentTranslationTransformRotate:
			case SvDef::SVExtentTranslationLinear:
			{
				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );

				if ( S_OK == l_hrOk )
				{
					double l_dRadius = SVGetRadius( p_svRadiusPoint, p_svValue );

					p_rsvResult = SVRotatePoint( p_svRotatePoint, l_dRadius, l_dAngle + p_dLineAngle );
				}

				break;
			}// end case multiple

			case SvDef::SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd );
				}

				if ( S_OK == l_hrOk )
				{
					double l_dAngle = SVGetRotationAngle( l_svStart, l_svEnd );

					if ( S_OK == l_hrOk )
					{
						double l_dRadius = SVGetRadius( p_svRadiusPoint, p_svValue );

						p_rsvResult = SVRotatePoint( p_svRotatePoint, l_dRadius, l_dAngle + p_dLineAngle );
					}
				}

				break;
			}// end case SvDef::SVExtentTranslationLine:

			case SvDef::SVExtentTranslationFlippedRotate:
			{
				double l_dAngle = 0.0;

				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );

				if ( S_OK == l_hrOk )
				{
					double l_dRadius = SVGetRadius( p_svRadiusPoint, p_svValue );

					p_rsvResult = SVRotateAndFlipPoint( p_svRotatePoint, l_dRadius, l_dAngle - p_dLineAngle );
				}
			}// end case SvDef::SVExtentTranslationFlippedRotate:

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

	if( S_OK == l_hrOk )
	{
		if( S_OK == p_rsvValue.GetFigure( l_svFigure ) )
		{
			if( p_rsvValue.m_eTranslation == SvDef::SVExtentTranslationLine )
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

			if( S_OK == p_rsvValue.GetRectangle( l_oInRect ) )
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
		case SvDef::SVExtentTranslationNone:
		{
			p_rsvOffsetData.m_bIsLinear &= true;

			l_hrOk = S_OK;

			break;
		}

		case SvDef::SVExtentTranslationShift:
		{
			SVExtentPointStruct l_svPosition;

			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

			if ( S_OK == l_hrOk )
			{
				SVExtentPointStruct l_svDisplacement;
				SVExtentPointStruct l_svResult = l_svPosition;

				m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);

				l_svResult += l_svDisplacement;
				l_svResult -= p_rsvOffsetData.m_svRotationPoint;
				l_svResult += p_rsvOffsetData.m_svOffset;

				p_rsvOffsetData.m_svOffset = SVRotatePoint( p_rsvOffsetData.m_svOffset, l_svResult, p_rsvOffsetData.m_dRotationAngle );

				l_hrOk = p_rsvOffsetData.m_svRotationPoint.Initialize();

				p_rsvOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationProfileShift:
		{
			SVExtentPointStruct l_svPosition;

			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

			if ( S_OK == l_hrOk )
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
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationLinear:
		{
			SVExtentPointStruct l_svRotation;
			SVExtentPointStruct l_svOutputRotation;

			double l_dAngle = 0.0;

			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
			}

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
			}

			if ( S_OK == l_hrOk )
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
		case SvDef::SVExtentTranslationTransformShift:
		{
			SVExtentPointStruct l_svDisplacement;

			l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);

			if ( S_OK == l_hrOk )
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
		case SvDef::SVExtentTranslationTransformRotate:
		{
			SVExtentPointStruct l_svRotation;
			SVExtentPointStruct l_svOutputRotation;
			SVExtentPointStruct l_svDisplacement;

			double l_dAngle = 0.0;

			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
			}

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
			}

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, l_svDisplacement);
			}

			if ( S_OK == l_hrOk )
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
		case SvDef::SVExtentTranslationFlippedRotate:
		{
			SVExtentPointStruct l_svRotation;
			SVExtentPointStruct l_svOutputRotation;

			double l_dAngle = 0.0;

			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svRotation );

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );
			}

			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svOutputRotation );
			}

			if ( S_OK == l_hrOk )
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
		}// end case SvDef::SVExtentTranslationFlippedRotate:
		// Warp Type Tools
		case SvDef::SVExtentTranslationVerticalPerspective:
		case SvDef::SVExtentTranslationHorizontalPerspective:
		case SvDef::SVExtentTranslationCylindricalWarpH:
		case SvDef::SVExtentTranslationCylindricalWarpV:
		case SvDef::SVExtentTranslationFigureShift:
		case SvDef::SVExtentTranslationPolarUnwrap:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFlipHorizontal:
		{
			SVExtentPointStruct l_svPosition;

			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );

			if ( S_OK == l_hrOk )
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

HRESULT SVImageExtentClass::SetDataFromFile(LPCTSTR pFileName)
{
	HRESULT hrOk = E_FAIL;
	HRESULT Code = E_FAIL;
	long Width(0), Height(0);

	if (0 < strlen(pFileName))
	{
		SVFileNameClass	svfncImageFile(pFileName);
		SVMatroxFileTypeEnum fileformatID = SVMatroxImageInterface::getFileType(svfncImageFile.GetExtension().c_str());

		if (fileformatID != SVFileUnknown && ::SVFileExists(pFileName))
		{

			std::string strFile(pFileName);

			Code = SVMatroxBufferInterface::GetImageSize(strFile, Width, Height);
		}

	}

	if (Code == S_OK)
	{
		SetExtentProperty(SvDef::SVExtentPropertyEnum::SVExtentPropertyWidth, Width);
		SetExtentProperty(SvDef::SVExtentPropertyEnum::SVExtentPropertyHeight, Height);
		SetExtentProperty(SvDef::SVExtentPropertyEnum::SVExtentPropertyPositionPointX, 0);
		SetExtentProperty(SvDef::SVExtentPropertyEnum::SVExtentPropertyPositionPointY, 0);
		SetTranslation(SvDef::SVExtentTranslationNone);
		UpdateData();
		hrOk = S_OK;
	}
	return hrOk;
}

HRESULT SVImageExtentClass::UpdatePolarFromOutputSpace( SvDef::SVExtentLocationPropertyEnum p_eLocation, long p_dX, long p_dY )
{
	HRESULT l_hrOk = S_FALSE;
	SVExtentPointStruct l_svPosition;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if (   S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition )
	    && S_OK == GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle )
	    && S_OK == GetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle )
	    && S_OK == GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius )
	    && S_OK == GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius ) )
	{
		switch ( p_eLocation )
		{
			case SvDef::SVExtentLocationPropertyLeft:
			{
				double l_dNewAngle = l_dStartAngle + g_svPi.RadiansToDegrees( p_dX ) / l_dOuterRadius;

				if ( l_dNewAngle > l_dEndAngle )
					l_dNewAngle = l_dEndAngle;

				l_dStartAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyRight:
			{
				double l_dNewAngle = l_dEndAngle + g_svPi.RadiansToDegrees( p_dX ) / l_dOuterRadius;

				if ( l_dNewAngle < l_dStartAngle )
					l_dNewAngle = l_dStartAngle;

				l_dEndAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyTop:
			{
				l_dInnerRadius += p_dY;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyBottom:
			{
				l_dOuterRadius += p_dY;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyCenter:
			{
				l_svPosition.m_dPositionX += p_dX;
				l_svPosition.m_dPositionY += p_dY;

				l_hrOk = S_OK;

				break;
			}
		}// end switch ( p_eLocation )

		if ( S_OK == l_hrOk )
		{
			if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius ) )
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

HRESULT SVImageExtentClass::UpdateLine( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svStart;
	SVExtentPointStruct l_svEnd;

	if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart ) &&
			 S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd ) )
	{
		double l_dX = p_svEnd.m_dPositionX - p_svStart.m_dPositionX;
		double l_dY = p_svEnd.m_dPositionY - p_svStart.m_dPositionY;

		switch( p_eLocation )
		{
			case SvDef::SVExtentLocationPropertyLeft:
			{
				l_svStart.m_dPositionX += l_dX;
				l_svStart.m_dPositionY += l_dY;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyRight:
			{
				l_svEnd.m_dPositionX += l_dX;
				l_svEnd.m_dPositionY += l_dY;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyCenter:
			{
				l_svStart.m_dPositionX += l_dX;
				l_svStart.m_dPositionY += l_dY;

				l_svEnd.m_dPositionX += l_dX;
				l_svEnd.m_dPositionY += l_dY;

				l_hrOk = S_OK;

				break;
			}
		}

		if ( S_OK == l_hrOk )
		{
			if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd )  )
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

HRESULT SVImageExtentClass::UpdatePolar( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPosition;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if ( S_OK == GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
			 S_OK == GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle ) &&
			 S_OK == GetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle ) &&
			 S_OK == GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius ) &&
			 S_OK == GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius ) )
	{
		double l_dNewAngle = SVGetFlippedRotationAngle( l_svPosition, p_svEnd ); 
		double l_dNewRadius = SVGetRadius( l_svPosition, p_svEnd ); 

		switch( p_eLocation )
		{
			case SvDef::SVExtentLocationPropertyTopLeft:
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
			case SvDef::SVExtentLocationPropertyTopRight:
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
			case SvDef::SVExtentLocationPropertyBottomRight:
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
			case SvDef::SVExtentLocationPropertyBottomLeft:
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
			case SvDef::SVExtentLocationPropertyLeft:
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
			case SvDef::SVExtentLocationPropertyRight:
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
			case SvDef::SVExtentLocationPropertyTop:
			{
				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyBottom:
			{
				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyCenter:
			{
				l_svPosition.m_dPositionX += p_svEnd.m_dPositionX - p_svStart.m_dPositionX;
				l_svPosition.m_dPositionY += p_svEnd.m_dPositionY - p_svStart.m_dPositionY;

				l_hrOk = S_OK;

				break;
			}
		}

		if ( S_OK == l_hrOk )
		{
			if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius ) &&
					 S_OK == SetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius ) )
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

	if ( S_OK != m_svPosition.DisableExtentProperty( SvDef::SVExtentPropertyPositionsOutput ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyDimentionsOutput ) )
	{
		l_hrOk = S_FALSE;
	}

	if( m_eTranslation == SvDef::SVExtentTranslationLine )
	{
		if ( S_OK != m_svPosition.DisableExtentProperty( SvDef::SVExtentPropertyRotationAngle ) )
		{
			l_hrOk = S_FALSE;
		}

		if ( S_OK != m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyWidth ) )
		{
			l_hrOk = S_FALSE;
		}

		if ( S_OK != m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyHeight ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		if ( S_OK != m_svPosition.DisableExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine ) )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::BuildOutputDimensions()
{
	HRESULT l_hrOk = ClearOutputData();

	if ( S_OK == l_hrOk )
	{
		switch( m_eTranslation )
		{
			case SvDef::SVExtentTranslationUnknown:
			{
				break;
			}

			case SvDef::SVExtentTranslationNone:
			case SvDef::SVExtentTranslationShift:
			case SvDef::SVExtentTranslationProfileShift:
			case SvDef::SVExtentTranslationTransformShift:
			case SvDef::SVExtentTranslationFlipVertical:
			case SvDef::SVExtentTranslationFlipHorizontal:
			case SvDef::SVExtentTranslationFigureShift:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end multiple case:

			case SvDef::SVExtentTranslationLine:
			{
				SVExtentPointStruct l_svStart;
				SVExtentPointStruct l_svEnd;

				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPointEndOfLine, l_svEnd );				
				}

				if ( S_OK == l_hrOk )
				{
					double l_dWidth = SVGetRadius( l_svStart, l_svEnd );

					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );				

					if ( S_OK == l_hrOk )
					{
						l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dWidth );				
					}
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyHeight, 1.0 );				

					if ( S_OK == l_hrOk )
					{
						l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, 1.0 );				
					}
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0.0 );
				}

				break;
			}// end case SvDef::SVExtentTranslationLine:

			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationResize:
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

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, inputWidth );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidthScaleFactor, widthScaleFactor );				
				}

				if ( S_OK == l_hrOk )
				{
					l_dValue = inputWidth * widthScaleFactor;
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, inputHeight );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor );				
				}

				if ( S_OK == l_hrOk )
				{
					l_dValue = inputHeight * heightScaleFactor;

					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end case SvDef::SVExtentTranslationDoubleHeight:

			case SvDef::SVExtentTranslationFlippedRotate:
			case SvDef::SVExtentTranslationRotate:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end case SvDef::SVExtentTranslationFlippedRotate: case SvDef::SVExtentTranslationRotate:

			case SvDef::SVExtentTranslationLinear:
			{
				double l_dHeight = 0.0;
				double l_dWidth = 0.0;
				double l_dAngle = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, l_dAngle );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dWidth );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dHeight );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0 );
				}

				break;
			}// end case SvDef::SVExtentTranslationLinear:

			case SvDef::SVExtentTranslationProfile:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					SVExtentPointStruct l_svPoint;

					l_svPoint.m_dPositionY += l_dValue / 2;

					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svPoint );
				}

				break;
			}// end case SvDef::SVExtentTranslationProfile:

			case SvDef::SVExtentTranslationPolarUnwrap:
			{
				double l_dInnerRadius = 0.0;
				double l_dOuterRadius = 0.0;
				double l_dStartAngle = 0.0;
				double l_dEndAngle = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyRectangle );

				if ( S_OK != m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle ) )
				{
					l_hrOk = S_FALSE;
				}

				if ( S_OK != m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle ) )
				{
					l_hrOk = S_FALSE;
				}

				if ( S_OK != m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius ) )
				{
					l_hrOk = S_FALSE;
				}

				if ( S_OK != m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius ) )
				{
					l_hrOk = S_FALSE;
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				if ( S_OK == l_hrOk )
				{
					double l_dHeight = fabs( l_dOuterRadius - l_dInnerRadius );
					if( l_dHeight < 1.0 )
					{
						l_dHeight = 1.0;
					}
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dHeight );

					if ( S_OK == l_hrOk )
					{
						double l_dMaxRadius = l_dOuterRadius > l_dInnerRadius ? l_dOuterRadius : l_dInnerRadius;
						double l_dWidth = g_svPi.DegreesToRadians( l_dEndAngle - l_dStartAngle ) * l_dMaxRadius;
						if( l_dWidth < 1.0 )
						{
							l_dWidth = 1.0;
						}
						l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dWidth );
					}
				}

				break;
			}// end case SvDef::SVExtentTranslationPolarUnwrap:

			case SvDef::SVExtentTranslationTransformRotate:
			{
				SVExtentPointStruct l_svPosition;

				double l_dHeight = 0.0;
				double l_dWidth = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svPosition );				
				}

				if ( S_OK == l_hrOk )
				{
					double l_dMax = sqrt( ( l_dWidth * l_dWidth ) + ( l_dHeight * l_dHeight ) );

					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dMax );

					if ( S_OK == l_hrOk )
					{
						l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dMax );
					}

					if ( S_OK == l_hrOk )
					{
						l_svPosition.m_dPositionX += ( l_dMax - l_dWidth ) / 2;
						l_svPosition.m_dPositionY += ( l_dMax - l_dHeight ) / 2;

						l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, l_svPosition );
					}
				}

				break;
			}// end case SvDef::SVExtentTranslationTransformRotate:

			case SvDef::SVExtentTranslationCylindricalWarpH:
			{
				double l_dValue = 0.0;
				double l_dWarpAngle = 0.0;

				l_hrOk = S_OK; 

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dWarpAngle );
				}
				if ( S_OK == l_hrOk )
				{
					double l_dRadius = ( l_dValue / sin( l_dWarpAngle / 360.0 * g_svPi.pi())) / 2.0 ;
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dRadius * g_svPi.pi() * ( l_dWarpAngle / 180.0 ) );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPointX, 0.0 );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPointY, 0.0 );				
				}
				break;
			}// end case SvDef::SVExtentTranslationCylindricalWarpH:

			case SvDef::SVExtentTranslationCylindricalWarpV:
			{
				double l_dValue = 0.0;
				double l_dWarpAngle = 0.0;

				l_hrOk = S_OK; 

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dWarpAngle );
				}
				if ( S_OK == l_hrOk )
				{
					double l_dRadius = ( l_dValue / sin( l_dWarpAngle / 360.0 * g_svPi.pi()  )) / 2.0 ;
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dRadius * g_svPi.pi() * ( l_dWarpAngle / 180.0 ) );
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPointX, 0.0 );				
				}
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPointY, 0.0 );				
				}
				break;
			}// end case SvDef::SVExtentTranslationCylindricalWarpV:

			case SvDef::SVExtentTranslationVerticalPerspective:
			case SvDef::SVExtentTranslationHorizontalPerspective:
			{
				double l_dValue = 0.0;

				l_hrOk = m_svDimensions.DisableExtentProperty( SvDef::SVExtentPropertyPie );

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0.0 );				
				}

				break;
			}// end case SvDef::SVExtentTranslationVerticalPerspective:

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

	if ( S_OK == l_hrOk )
	{
		switch( m_eTranslation )
		{
			case SvDef::SVExtentTranslationUnknown:
			case SvDef::SVExtentTranslationNone:
			case SvDef::SVExtentTranslationTransformShift:
			case SvDef::SVExtentTranslationTransformRotate:
			case SvDef::SVExtentTranslationCylindricalWarpH:
			case SvDef::SVExtentTranslationCylindricalWarpV:
			{
				// No figure
				break;
			}
			case SvDef::SVExtentTranslationShift:
			case SvDef::SVExtentTranslationRotate:
			case SvDef::SVExtentTranslationFlippedRotate:
			case SvDef::SVExtentTranslationProfile:
			case SvDef::SVExtentTranslationProfileShift:
			{
				double l_dWidth = 0.0;
				double l_dHeight = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );				

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );				
				}

				if ( S_OK == l_hrOk )
				{
					m_svFigure.m_eShape = SvDef::SVExtentShapeRectangle;

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
			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationResize:
			case SvDef::SVExtentTranslationFlipHorizontal:
			case SvDef::SVExtentTranslationFlipVertical:
			case SvDef::SVExtentTranslationFigureShift:
			{
				double l_dWidth = 0.0;
				double l_dHeight = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );				

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );				
				}

				SVExtentPointStruct l_svStart;
				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svStart );
				}

				if ( S_OK == l_hrOk )
				{
					m_svFigure.m_eShape = SvDef::SVExtentShapeRectangle;

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
				}

				break;
			}

			case SvDef::SVExtentTranslationLinear:
			{
				double l_dWidth = 0.0;
				double l_dHeight = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );				

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );				
				}

				if ( S_OK == l_hrOk )
				{
					m_svFigure.m_eShape = SvDef::SVExtentShapeArrow;

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
			case SvDef::SVExtentTranslationLine:
			{
				double l_dWidth = 0.0;

				l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );				

				if ( S_OK == l_hrOk )
				{
					m_svFigure.m_eShape = SvDef::SVExtentShapeArrow;

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
			case SvDef::SVExtentTranslationPolarUnwrap:
			{
				SVExtentPointStruct l_svCenterPoint;

				double l_dStartAngle = 0.0;
				double l_dEndAngle = 0.0;
				double l_dInnerRadius = 0.0;
				double l_dOuterRadius = 0.0;

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_svCenterPoint );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyEndAngle, l_dEndAngle );				
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius );				
					if( l_dInnerRadius < 1 )
						l_dInnerRadius = 1;
				}

				if ( S_OK == l_hrOk )
				{
					l_hrOk = m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius );
					if( l_dOuterRadius < 1 )
					{
						l_dOuterRadius = 1;
					}
				}

				if ( S_OK == l_hrOk )
				{
					if ( l_dEndAngle - l_dStartAngle < 360.0 )
					{
						m_svFigure.m_eShape = SvDef::SVExtentShapePie;
					}
					else if ( l_dEndAngle - l_dStartAngle < 720.0 )
					{
						m_svFigure.m_eShape = SvDef::SVExtentShapeCircle;
					}
					else
					{
						m_svFigure.m_eShape = SvDef::SVExtentShapeDoubleCircle;
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
			case SvDef::SVExtentTranslationVerticalPerspective:
			{
				double l_dWidth;
				double l_dHeight;
				double l_dYPosition;
				if( S_OK == m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth ) &&
					S_OK == m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight ) &&
					S_OK == m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetY, l_dYPosition ) )
				{
					m_svFigure.m_eShape = SvDef::SVExtentShapeHexagon;

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
			case SvDef::SVExtentTranslationHorizontalPerspective:
			{
				double l_dWidth;
				double l_dHeight;
				double l_dXPosition;
				if( S_OK == m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth ) &&
					S_OK == m_svDimensions.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight ) &&
					S_OK == m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetX, l_dXPosition ) )
				{
					m_svFigure.m_eShape = SvDef::SVExtentShapeHexagon;

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

		if ( S_OK != l_hrOk )
		{
			m_svFigure.Initialize();
		}
	}

	return l_hrOk;
}

HRESULT  SVImageExtentClass::UpdateVerticalPerspective( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPosition;

	double l_dY = p_svStart.m_dPositionY - p_svEnd.m_dPositionY;

	if ( S_OK == m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
	{
		switch( p_eLocation )
		{
			case SvDef::SVExtentLocationPropertyTop:
			case SvDef::SVExtentLocationPropertyTopRight:
			case SvDef::SVExtentLocationPropertyTopLeft:
			{
				l_svPosition.m_dPositionY -= l_dY;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyBottom:
			case SvDef::SVExtentLocationPropertyBottomRight:
			case SvDef::SVExtentLocationPropertyBottomLeft:
			{
				l_svPosition.m_dPositionY += l_dY;
				l_hrOk = S_OK;
				break;
			}
		}

		if ( S_OK == l_hrOk )
		{
			if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
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

HRESULT  SVImageExtentClass::UpdateHorizontalPerspective( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPosition;

	double l_dX = p_svStart.m_dPositionX - p_svEnd.m_dPositionX;

	if ( S_OK == m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
	{
		switch( p_eLocation )
		{
			case SvDef::SVExtentLocationPropertyBottomLeft:
			case SvDef::SVExtentLocationPropertyTopLeft:
			case SvDef::SVExtentLocationPropertyLeft:
			{
				l_svPosition.m_dPositionX -= l_dX;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyRight:
			case SvDef::SVExtentLocationPropertyBottomRight:
			case SvDef::SVExtentLocationPropertyTopRight:
			{
				l_svPosition.m_dPositionX += l_dX;
				l_hrOk = S_OK;
				break;
			}
		}

		if ( S_OK == l_hrOk )
		{
			if ( S_OK == SetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, l_svPosition ) )
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

	if ( S_OK == l_hrOk )
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
		case SvDef::SVExtentTranslationResize:
		{
			//@WARNING [Jim][8 July 2015] No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.
			l_hrOk = m_svPosition.GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, svPositionPoint);

			if (S_OK == l_hrOk )
			{
				p_svTitlePoint.m_dPositionY = (svPositionPoint.m_dPositionY - 10);
				p_svTitlePoint.m_dPositionX = (svPositionPoint.m_dPositionX + 5);
			}
			break;
		}
		// These cases place the Title below the top of the ROI rectangle
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFigureShift:
		{
			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, svPositionPoint);

			if( S_OK == l_hrOk )
			{
				p_svTitlePoint.m_dPositionY = (svPositionPoint.m_dPositionY + 5 );
				p_svTitlePoint.m_dPositionX = svPositionPoint.m_dPositionX + 5;
			}
			break;
		}

		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			l_hrOk = this->m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, svPositionPoint);

			if( S_OK == l_hrOk )
			{
				p_svTitlePoint.m_dPositionY = svPositionPoint.m_dPositionY + 5;
				p_svTitlePoint.m_dPositionX = svPositionPoint.m_dPositionX + 5;
			}

			break;
		}
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			l_hrOk = m_svPosition.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, svPositionPoint);

			if( S_OK == l_hrOk )
			{
				p_svTitlePoint.m_dPositionY = svPositionPoint.m_dPositionY + 5;
				p_svTitlePoint.m_dPositionX = svPositionPoint.m_dPositionX + 5;
			}

			break;
		}
		// These cases place the Title above the top of the ROI rectangle
		case SvDef::SVExtentTranslationShift:
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationProfileShift:
		case SvDef::SVExtentTranslationTransformShift:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationPolarUnwrap:
		case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationLinear:
		case SvDef::SVExtentTranslationProfile:
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

