#pragma once
#include "SVPoint.h"
#include <corecrt_math_defines.h>



namespace SvUl
{
inline double Degrees2Radians(double value) { return (M_PI * value / 180.0); }

inline double Radians2Degrees(double value) { return (value * 180.0 / M_PI); }

inline double SVGetRadius(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint)
{
	SVPoint<double> diffPoint = rPoint - rCenter;

	double result = std::sqrt((diffPoint.m_x * diffPoint.m_x) + (diffPoint.m_y * diffPoint.m_y));

	return result;
}

inline double SVGetRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint)
{
	double result {0.0};

	double x = rPoint.m_x - rCenter.m_x;
	double y = rPoint.m_y - rCenter.m_y;

	if (0.0 == x)
	{
		result = (y < 0.0) ? 270.0 : 90.0;
	}
	else if (0.0 == y)
	{
		result = (x < 0.0) ? 180.0 : 0.0;
	}
	else
	{
		result = Radians2Degrees(std::atan2(y, x));

		if (result < 0.0)
		{
			result += 360.0;
		}
	}

	return result;
}

inline SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, double dRadius, double dAngle)
{
	SVPoint<double> result;

	//The fmod check is to make sure that exactly 0.0 is returned
	bool callCos = (std::fmod(dAngle, 90.0) != 0.0) || fmod(dAngle, 180.0) == 0.0;
	double dXCos = callCos ? std::cos(Degrees2Radians(dAngle)) : 0.0;
	double dYSin = (std::fmod(dAngle, 180.0) != 0.0) ? std::sin(Degrees2Radians(dAngle)) : 0.0;

	result.m_x = rCenter.m_x + dRadius * dXCos;
	result.m_y = rCenter.m_y + dRadius * dYSin;
	return result;
}



inline SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle)
{

	double Angle = fmod(dAngle, 360);
	double dcos, dsin;

	if (Angle < 0)
	{
		Angle += 360;
	}
	if (Angle == 0)
	{
		dcos = 1;
		dsin = 0;
	}
	else if (Angle == 90)
	{
		dcos = 0;
		dsin = 1;
	}
	else if (Angle == 180)
	{
		dcos = -1;
		dsin = 0;
	}
	else if (Angle == 270)
	{
		dcos = 0;
		dsin = -1;
	}
	else
	{
		dcos = cos(Degrees2Radians(dAngle));
		dsin = sin(Degrees2Radians(dAngle));
	}
	SVPoint<double> point = rStartPoint - rCenter;
	SVPoint<double>erg;
	erg.m_x = point.m_x * dcos - point.m_y * dsin;
	erg.m_y = point.m_x * dsin + point.m_y * dcos;
	erg = erg + rCenter;
	return erg;
}



inline double SVGetFlippedRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint)
{
	double result {0.0};

	SVPoint<double> diffPoint = rPoint - rCenter;

	if (diffPoint.m_x == 0.0)
	{
		result = (-diffPoint.m_y < 0.0) ? 270.0 : 90.0;
	}
	else if (diffPoint.m_y == 0.0)
	{
		result = (diffPoint.m_x < 0.0) ? 180.0 : 0.0;
	}
	else
	{
		result = Radians2Degrees(std::atan2(-diffPoint.m_y, diffPoint.m_x));

		if (result < 0.0)
		{
			result += 360.0;
		}
	}

	return result;
}

inline SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, double dRadius, double dAngle)
{
	SVPoint<double> result;

	//The fmod check is to make sure that exactly 0.0 is returned
	bool callCos = (std::fmod(dAngle, 90.0) != 0.0) || fmod(dAngle, 180.0) == 0.0;
	double dXCos = callCos ? std::cos(Degrees2Radians(dAngle)) : 0.0;
	double dYSin = (std::fmod(dAngle, 180.0) != 0.0) ? std::sin(Degrees2Radians(dAngle)) : 0.0;

	result.m_x = rCenter.m_x + dRadius * dXCos;
	result.m_y = rCenter.m_y - dRadius * dYSin;

	return result;
}

inline SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle)
{
	double dRadius = SVGetRadius(rCenter, rStartPoint);
	double dResultAngle = SVGetFlippedRotationAngle(rCenter, rStartPoint) + dAngle;

	return SVRotateAndFlipPoint(rCenter, dRadius, dResultAngle);
}

}