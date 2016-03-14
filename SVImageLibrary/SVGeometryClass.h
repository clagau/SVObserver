//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGeometryClass
//* .File Name       : $Workfile:   SVGeometryClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:26  $
//******************************************************************************

#ifndef SVGEOMETRYCLASS_H__
#define SVGEOMETRYCLASS_H__
#pragma once

#include <math.h>
//const double g_svPI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
//const double g_svRadian = 180.0 / g_svPI;

//extern SVGeometryClass g_svPi;	// declared below

class SVGeometryClass
{
public:
	SVGeometryClass()
	{
		m_dPi = asin( 0.5 ) * 6.0;
	};

	inline double DegreesToRadians( double p_dDegrees )
	{
		return m_dPi * p_dDegrees / 180.0;
	};

	inline double RadiansToDegrees( double p_dRadians )
	{
		return p_dRadians * 180.0 / m_dPi;
	};

	inline double GetArcTanInDegrees( double p_dY, double p_dX )
	{
		return RadiansToDegrees( atan2( p_dY, p_dX ) );
	};

	inline double GetSinFromDegrees( double p_dAngle )
	{
		double l_dResult = 0.0;

		if( fmod( p_dAngle, 180.0 ) != 0.0 )
		{
			l_dResult = ::sin( DegreesToRadians( p_dAngle ) ) ;
		}

		return l_dResult;
	};

	inline double GetCosFromDegrees( double p_dAngle )
	{
		double l_dResult = 0.0;

		if( fmod( p_dAngle, 90.0 ) != 0.0 ||
		    fmod( p_dAngle, 180.0 ) == 0.0 )
		{
			l_dResult = ::cos( DegreesToRadians( p_dAngle ) ) ;
		}

		return l_dResult;
	};

	inline double GetCircumference( double p_dRadius )
	{
		return 2.0 * m_dPi * p_dRadius;
	};

	inline double GetAngleInRadians( double p_cy, double p_cx )	// expressed in degrees, range is (-180, 180]
	{
		return ::atan2( p_cy, p_cx );
	};

	inline double GetHypotenuse( double p_cy, double p_cx )
	{
		return sqrt( p_cy * p_cy + p_cx * p_cx );
	};

	double pi() { return m_dPi; }

private:
	double m_dPi;
};

extern SVGeometryClass g_svPi;

#endif	//#ifndef SVGEOMETRYCLASS_H__

