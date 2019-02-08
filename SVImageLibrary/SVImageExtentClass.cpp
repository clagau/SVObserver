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
//Moved to precompiled header: #include <cmath>
//Moved to precompiled header: #include <map>
#include "SVImageExtentClass.h"
#include "SVFileSystemLibrary\SVFileNameClass.h"
#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"


//These are the property extents which have values to be saved in the map
constexpr SvDef::SVExtentPropertyEnum cStoredExtentValues[]
{
	SvDef::SVExtentPropertyWidth,
	SvDef::SVExtentPropertyHeight,
	SvDef::SVExtentPropertyStartAngle,
	SvDef::SVExtentPropertyEndAngle,
	SvDef::SVExtentPropertyInnerRadius,
	SvDef::SVExtentPropertyOuterRadius,
	SvDef::SVExtentPropertyWidthScaleFactor,
	SvDef::SVExtentPropertyHeightScaleFactor,
	SvDef::SVExtentPropertyOutputWidth,
	SvDef::SVExtentPropertyOutputHeight,
	SvDef::SVExtentPropertyRotationAngle,
	SvDef::SVExtentPropertyPositionPointX,
	SvDef::SVExtentPropertyPositionPointY,
	SvDef::SVExtentPropertyPositionPointEndOfLineX,
	SvDef::SVExtentPropertyPositionPointEndOfLineY,
	SvDef::SVExtentPropertyTranslationOffsetX,
	SvDef::SVExtentPropertyTranslationOffsetY,
	SvDef::SVExtentPropertyOutputPositionPointX,
	SvDef::SVExtentPropertyOutputPositionPointY,
};

static const SVExtentPropertyStringMap cExtentPropertyNames =
{
	{SvDef::SVExtentPropertyPositionPointX, std::string("X Position")},
	{SvDef::SVExtentPropertyPositionPointY, std::string("Y Position")},
	{SvDef::SVExtentPropertyTranslationOffsetX, std::string("X Offset")},
	{SvDef::SVExtentPropertyTranslationOffsetY, std::string("Y Offset")},
	{SvDef::SVExtentPropertyTranslationOffsetY, std::string("Y Offset")},
	{SvDef::SVExtentPropertyTranslationOffsetY, std::string("Y Offset")},
	{SvDef::SVExtentPropertyRotationAngle, std::string("Rotation Angle")},
	{SvDef::SVExtentPropertyOutputPositionPointX, std::string("Output X Position")},
	{SvDef::SVExtentPropertyOutputPositionPointY, std::string("Output Y Position")},
	{SvDef::SVExtentPropertyWidth, std::string("Width")},
	{SvDef::SVExtentPropertyHeight, std::string("Height")},
	{SvDef::SVExtentPropertyStartAngle, std::string("Start Angle")},
	{SvDef::SVExtentPropertyEndAngle, std::string("End Angle")},
	{SvDef::SVExtentPropertyInnerRadius, std::string("Inner Radius")},
	{SvDef::SVExtentPropertyOuterRadius, std::string("Outer Radius")},
	{SvDef::SVExtentPropertyOutputWidth, std::string("Width")},
	{SvDef::SVExtentPropertyOutputHeight, std::string("Height")},
};

static const SVExtentPropertyPointMap cExtentPropertyPointMap =
{
	{SvDef::SVExtentPropertyPositionPoint, {SvDef::SVExtentPropertyPositionPointX, SvDef::SVExtentPropertyPositionPointY}},
	{SvDef::SVExtentPropertyPositionPointEndOfLine, {SvDef::SVExtentPropertyPositionPointEndOfLineX, SvDef::SVExtentPropertyPositionPointEndOfLineY}},
	{SvDef::SVExtentPropertyTranslationOffset, {SvDef::SVExtentPropertyTranslationOffsetX, SvDef::SVExtentPropertyTranslationOffsetY}},
	{SvDef::SVExtentPropertyOutputPositionPoint, {SvDef::SVExtentPropertyOutputPositionPointX, SvDef::SVExtentPropertyOutputPositionPointY}},
};

double Degrees2Radians(double value) { return (M_PI * value / 180.0); }

double Radians2Degrees(double value) { return (value * 180.0 / M_PI); }

double SVGetRadius(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint)
{
	double result {0.0};

	SVPoint<double> diffPoint = rPoint - rCenter;

	result = std::sqrt((diffPoint.m_x * diffPoint.m_x) + (diffPoint.m_y * diffPoint.m_y));

	return result;
}

double SVGetRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint)
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

SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, double dRadius, double dAngle)
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

SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle)
{
	double dRadius = SVGetRadius(rCenter, rStartPoint);
	double dResultAngle = SVGetRotationAngle(rCenter, rStartPoint) + dAngle;

	return SVRotatePoint(rCenter, dRadius, dResultAngle);
}

double SVGetFlippedRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint)
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

SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, double dRadius, double dAngle)
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

SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle)
{
	double dRadius = SVGetRadius(rCenter, rStartPoint);
	double dResultAngle = SVGetFlippedRotationAngle(rCenter, rStartPoint) + dAngle;

	return SVRotateAndFlipPoint(rCenter, dRadius, dResultAngle);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVImageExtentClass::SVImageExtentClass()
{
	Initialize();
}

SVImageExtentClass::SVImageExtentClass(const SVImageExtentClass& rRhs)
{
	*this = rRhs;
}

SVImageExtentClass::~SVImageExtentClass()
{
}

const SVImageExtentClass &SVImageExtentClass::operator=(const SVImageExtentClass& rRhs)
{
	if (this != &rRhs)
	{
		m_eTranslation = rRhs.m_eTranslation;
		m_extentValues = rRhs.m_extentValues;
		m_figure = rRhs.m_figure;
	}
	return *this;
}

void SVImageExtentClass::Initialize()
{

	m_eTranslation = SvDef::SVExtentTranslationUnknown;
	m_properties = 0UL;

	for (auto extentProperty : cStoredExtentValues)
	{
		m_extentValues[extentProperty] = 0.0;
	}
}

SvDef::SVExtentTranslationEnum SVImageExtentClass::GetTranslation() const
{
	return m_eTranslation;
}

void SVImageExtentClass::SetTranslation(SvDef::SVExtentTranslationEnum eTranslation)
{
	m_eTranslation = eTranslation;

	ClearOutputData();
}

void SVImageExtentClass::DisableExtentProperty(SvDef::SVExtentPropertyEnum eProperty)
{
	m_properties &= static_cast<unsigned long> (~eProperty);
}

HRESULT SVImageExtentClass::GetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, long &rValue) const
{
	double dValue {0.0};

	HRESULT result = GetExtentProperty(eProperty, dValue);

	if (S_OK == result)
	{
		rValue = static_cast<long> (dValue);
	}

	return result;
}

HRESULT SVImageExtentClass::GetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, double &rValue) const
{
	SVExtentPropertyValueMap::const_iterator iter {m_extentValues.find(eProperty)};
	if (m_extentValues.end() != iter)
	{
		rValue = iter->second;
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT SVImageExtentClass::GetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, POINT &rValue) const
{
	SVPoint<double> point;

	HRESULT result = GetExtentProperty(eProperty, point);

	if (S_OK == result)
	{
		rValue = static_cast<POINT> (point);
	}

	return result;
}

HRESULT SVImageExtentClass::GetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, SVPoint<double>& rValue) const
{
	SVExtentPropertyPointMap::const_iterator iter {cExtentPropertyPointMap.find(eProperty)};
	if (cExtentPropertyPointMap.end() != iter)
	{
		GetExtentProperty(iter->second.first, rValue.m_x);
		GetExtentProperty(iter->second.second, rValue.m_y);
		return S_OK;
	}
	return E_INVALIDARG;
}

SVExtentPropertyStringMap SVImageExtentClass::GetExtentPropertyMap(SvDef::SVExtentPropertyEnum eWhichProperties) const
{
	SVExtentPropertyStringMap result;

	SVExtentPropertyStringMap::const_iterator iter;
	for (iter = cExtentPropertyNames.begin(); iter != cExtentPropertyNames.end(); ++iter)
	{
		if (iter->first == (iter->first & eWhichProperties))
		{
			result.insert(*iter);
		}
	}
	return result;
}

SVExtentPropertyVector SVImageExtentClass::GetExtentPropertyVector(SvDef::SVExtentPropertyEnum eWhichProperties) const
{
	SVExtentPropertyVector result;

	SVExtentPropertyStringMap::const_iterator iter;
	for (iter = cExtentPropertyNames.begin(); iter != cExtentPropertyNames.end(); ++iter)
	{
		if (iter->first == (iter->first & eWhichProperties) && isEnabled(iter->first))
		{
			result.emplace_back(iter->first);
		}
	}
	return result;
}

HRESULT SVImageExtentClass::SetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, double dValue, bool clearOutputData /*= true*/)
{
	HRESULT result {S_OK};

	// special case to handle polar unwrap
	if (SvDef::SVExtentTranslationPolarUnwrap == m_eTranslation)
	{
		if (SvDef::SVExtentPropertyRotationAngle == (eProperty & SvDef::SVExtentPropertyRotationAngle))
		{
			double dEndAngle = 0.0;
			double dStartAngle = 0.0;
			result = GetExtentProperty(SvDef::SVExtentPropertyEndAngle, dEndAngle);
			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle);
			}
			if (S_OK == result)
			{
				result = setProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle + (dValue - dEndAngle));
			}
			if (S_OK == result)
			{
				result = setProperty(SvDef::SVExtentPropertyEndAngle, dValue);
			}
		}
		else if (SvDef::SVExtentPropertyEndAngle == (eProperty & SvDef::SVExtentPropertyEndAngle))
		{
			result = setProperty(SvDef::SVExtentPropertyRotationAngle, dValue);
		}

	}

	if (S_OK == result)
	{
		result = setProperty(eProperty, dValue);
		if(clearOutputData)
		{
			ClearOutputData();
		}
	}

	return result;
}

HRESULT SVImageExtentClass::SetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue, bool clearOutputData /*= true*/)
{
	HRESULT result {E_INVALIDARG};

	SVExtentPropertyPointMap::const_iterator iter {cExtentPropertyPointMap.find(eProperty)};
	if (cExtentPropertyPointMap.end() != iter)
	{
		result = SetExtentProperty(iter->second.first, rValue.m_x, clearOutputData);
		if (S_OK == result)
		{
			result = SetExtentProperty(iter->second.second, rValue.m_y, clearOutputData);
		}
	}
	return result;
}

HRESULT SVImageExtentClass::UpdateData()
{
	HRESULT l_hrOk = BuildOutputDimensions();

	if (S_OK == l_hrOk)
	{
		l_hrOk = BuildFigure();
	}

	return l_hrOk;
}

bool SVImageExtentClass::hasFigure() const
{
	bool result {false};

	switch (m_figure.m_eShape)
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
			result = true;
			break;
		}
		default:
		{
			break;
		}
	}
	return result;
}

SvDef::SVExtentLocationPropertyEnum SVImageExtentClass::GetLocationPropertyAt(const SVPoint<double>& rPoint) const
{
	SvDef::SVExtentLocationPropertyEnum l_eLocation = SvDef::SVExtentLocationPropertyUnknown;

	SVPoint<double> outputPoint;

	RECT l_oRect;
	bool l_bValid = false;

	if (m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap)
	{
		double dValue;
		GetExtentProperty(SvDef::SVExtentPropertyPositionPointX, dValue);
		l_oRect.left = static_cast<long>(dValue - 2);
		l_oRect.right = static_cast<long>(dValue + 2);
		GetExtentProperty(SvDef::SVExtentPropertyPositionPointY, dValue);
		l_oRect.top = static_cast<long>(dValue - 2);
		l_oRect.bottom = static_cast<long>(dValue + 2);
		if (::PtInRect(&l_oRect, static_cast<POINT> (rPoint)))
		{
			l_eLocation = SvDef::SVExtentLocationPropertyCenter;
		}
	}

	switch (m_eTranslation)
	{
		case SvDef::SVExtentTranslationResize:
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFigureShift:
		{
			outputPoint = rPoint;
			l_bValid = S_OK == GetRectangle(l_oRect);
			break;
		}

		default:
		{
			l_bValid = S_OK == TranslateToOutputSpace(rPoint, outputPoint) &&
				S_OK == GetOutputRectangle(l_oRect);
		}
	}

	if (l_bValid)
	{
		RECT l_oTemp = l_oRect;

		// Check special case for Polar Unwrap tool when > 360
		if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown && m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap)
		{
			double dStartAngle {0.0};
			double dEndAngle {0.0};
			double dOuterRadius {0.0};
			double dInnerRadius {0.0};

			if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyEndAngle, dEndAngle) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dInnerRadius) &&
				dEndAngle - dStartAngle > 360)
			{
				GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dOuterRadius);
				double l_dCircum = 2.0 * M_PI * dOuterRadius;
				if (l_oRect.right > l_dCircum && l_oRect.right > 4)
				{
					l_oTemp.right = l_oRect.right - (long)l_dCircum;
					l_oTemp.left = l_oTemp.right;
					l_oTemp.top = (long)-dInnerRadius;
					if (::InflateRect(&l_oTemp, 2, -2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvDef::SVExtentLocationPropertyRight;
					}
				}
			}
		}


		long l_lInflateX = -2;
		long l_lInflateY = -2;

		if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			if (l_oRect.bottom - 4 < 9)
			{
				l_lInflateY = 1;
			}

			if (l_oRect.right - 4 < 9)
			{
				l_lInflateX = 1;
			}

			if (::InflateRect(&l_oTemp, l_lInflateX, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				if (m_eTranslation == SvDef::SVExtentTranslationRotate ||
					m_eTranslation == SvDef::SVExtentTranslationFlippedRotate ||
					m_eTranslation == SvDef::SVExtentTranslationProfile)
				{
					l_oTemp = l_oRect;

					if (l_lInflateY < 1)
					{
						l_lInflateY = 2;
					}

					if (l_oRect.right < 1)
					{
						l_lInflateX = 0;

						l_oTemp.left = l_oRect.right + 1;
						l_oTemp.right = l_oRect.right + 1;
					}
					else if (l_oRect.right < 9)
					{
						l_lInflateX = 1;

						l_oTemp.left = l_oRect.right;
						l_oTemp.right = l_oRect.right;
					}
					else if (l_oRect.right < 16)
					{
						l_lInflateX = 1;

						l_oTemp.left = l_oRect.right - 2;
						l_oTemp.right = l_oRect.right - 2;
					}
					else
					{
						l_lInflateX = 2;
						//Use a quarter of the width for the rotating pick area
						l_oTemp.left = l_oRect.right - (abs(l_oRect.right - l_oRect.left) / 4);
						l_oTemp.right = l_oRect.right - 5;
					}

					l_oTemp.top = l_oRect.bottom / 2;
					l_oTemp.bottom = l_oRect.bottom / 2;

					if (::InflateRect(&l_oTemp, l_lInflateX, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvDef::SVExtentLocationPropertyRotate;
					}
				}

				if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
				{
					l_eLocation = SvDef::SVExtentLocationPropertyCenter;
				}
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationLine &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.bottom = l_oRect.top;

			l_lInflateX = -2;

			if (l_oRect.right < 9)
			{
				l_oTemp.right = l_oRect.left;

				l_lInflateX = 2;
			}

			if (::InflateRect(&l_oTemp, l_lInflateX, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTop;
			}
		}

		if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.left = l_oRect.right;

			l_lInflateY = -2;

			if (l_oRect.bottom < 9)
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateY = 2;
			}

			if (::InflateRect(&l_oTemp, 2, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyRight;
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationLine &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.top = l_oRect.bottom;

			l_lInflateX = -2;

			if (l_oRect.right < 9)
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateX = 2;
			}

			if (::InflateRect(&l_oTemp, l_lInflateX, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottom;
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationProfile &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.right = l_oRect.left;

			l_lInflateY = -2;

			if (l_oRect.bottom < 9)
			{
				l_oTemp.bottom = l_oRect.top;

				l_lInflateY = 2;
			}

			if (::InflateRect(&l_oTemp, 2, l_lInflateY) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyLeft;
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationProfile &&
			m_eTranslation != SvDef::SVExtentTranslationLine &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.right = l_oRect.left;
			l_oTemp.bottom = l_oRect.top;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTopLeft;
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationLine &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.left = l_oRect.right;
			l_oTemp.bottom = l_oRect.top;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTopRight;
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationProfile &&
			m_eTranslation != SvDef::SVExtentTranslationLine &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.right = l_oRect.left;
			l_oTemp.top = l_oRect.bottom;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottomLeft;
			}
		}

		if (m_eTranslation != SvDef::SVExtentTranslationLine &&
			l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
		{
			l_oTemp = l_oRect;

			l_oTemp.left = l_oRect.right;
			l_oTemp.top = l_oRect.bottom;

			if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottomRight;
			}
		}

		if (m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap)
		{
			long l_lInnerRadius = 0;

			if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, l_lInnerRadius))
			{
				if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
				{
					if (0 < l_oRect.right - 4 && 0 < l_lInnerRadius - 4)
					{
						l_oTemp = l_oRect;
						l_oTemp.bottom = 0;
						l_oTemp.top = -l_lInnerRadius;

						if (::InflateRect(&l_oTemp, -2, -2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
						{
							l_eLocation = SvDef::SVExtentLocationPropertyCenter;
						}
					}
				}

				if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
				{
					l_oTemp = l_oRect;

					l_oTemp.bottom = -l_lInnerRadius;
					l_oTemp.top = -l_lInnerRadius;

					if (::InflateRect(&l_oTemp, 2, 2) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvDef::SVExtentLocationPropertyCenter;
					}
				}

				if (m_eTranslation != SvDef::SVExtentTranslationProfile &&
					l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
				{
					l_oTemp = l_oRect;

					l_oTemp.right = l_oRect.left;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if (::InflateRect(&l_oTemp, 2, 0) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvDef::SVExtentLocationPropertyLeft;
					}
				}

				if (l_eLocation == SvDef::SVExtentLocationPropertyUnknown)
				{
					l_oTemp = l_oRect;

					l_oTemp.left = l_oRect.right;
					l_oTemp.bottom = 0;
					l_oTemp.top = -l_lInnerRadius;

					if (::InflateRect(&l_oTemp, 2, 0) && ::PtInRect(&l_oTemp, static_cast<POINT> (outputPoint)))
					{
						l_eLocation = SvDef::SVExtentLocationPropertyRight;
					}
				}
			}
		}
		if (m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap)
		{
			if (l_eLocation == SvDef::SVExtentLocationPropertyTop ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottom)
			{
				double dOuterRadius {0.0};
				double dInnerRadius {0.0};
				if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dInnerRadius) &&
					S_OK == GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dOuterRadius))
				{
					if (dOuterRadius < dInnerRadius)
					{
						if (l_eLocation == SvDef::SVExtentLocationPropertyTop)
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
		if (m_eTranslation == SvDef::SVExtentTranslationVerticalPerspective)
		{
			if (l_eLocation == SvDef::SVExtentLocationPropertyTopRight ||
				l_eLocation == SvDef::SVExtentLocationPropertyTopLeft)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyTop;
			}
			if (l_eLocation == SvDef::SVExtentLocationPropertyBottomRight ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottomLeft)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyBottom;
			}
			if (l_eLocation == SvDef::SVExtentLocationPropertyCenter)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyUnknown;
			}
		}
		if (m_eTranslation == SvDef::SVExtentTranslationHorizontalPerspective)
		{
			if (l_eLocation == SvDef::SVExtentLocationPropertyTopRight ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottomRight)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyRight;
			}
			if (l_eLocation == SvDef::SVExtentLocationPropertyTopLeft ||
				l_eLocation == SvDef::SVExtentLocationPropertyBottomLeft)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyLeft;
			}
			if (l_eLocation == SvDef::SVExtentLocationPropertyCenter)
			{
				l_eLocation = SvDef::SVExtentLocationPropertyUnknown;
			}
		}
	}

	return l_eLocation;
}

// Translate the point to be relative (local space).  Usually this relates to the ROI of the parent image.
HRESULT SVImageExtentClass::TranslateToLocalSpace(SVPoint<double> value, SVPoint<double>& rResult)
{
	SVPoint<double> position;
	HRESULT hr = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

	if (S_OK == hr)
	{
		rResult = value - position;

		// optional...
		SVPoint<double> displacement;
		if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement))
		{
			rResult -= displacement;
		}
	}
	return hr;
}

HRESULT SVImageExtentClass::Update(SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStartPoint, const SVPoint<double>& rEndPoint)
{
	HRESULT l_hrOk = S_FALSE;

	if (m_eTranslation == SvDef::SVExtentTranslationPolarUnwrap)
	{
		l_hrOk = UpdatePolar(eLocation, rStartPoint, rEndPoint);
	}
	else if (m_eTranslation == SvDef::SVExtentTranslationLine)
	{
		l_hrOk = UpdateLine(eLocation, rStartPoint, rEndPoint);
	}
	else if (m_eTranslation == SvDef::SVExtentTranslationVerticalPerspective)
	{
		l_hrOk = UpdateVerticalPerspective(eLocation, rStartPoint, rEndPoint);
	}
	else if (m_eTranslation == SvDef::SVExtentTranslationHorizontalPerspective)
	{
		l_hrOk = UpdateHorizontalPerspective(eLocation, rStartPoint, rEndPoint);
	}
	else
	{
		SVPoint<double> outputStart;
		SVPoint<double> outputEnd;

		RECT l_oRect;

		bool l_bValid = false;
		switch (m_eTranslation)
		{
			case SvDef::SVExtentTranslationResize:
			{
				//@WARNING [Jim][8 July 2015] No identification of error cases.
				// GetExtentProperty () only l_bValid is identified, which is 
				// not very helpful. Currently none of these cases give much 
				// help with error identification.

				// Get mouse start position relative to input image ROI.
				l_bValid = (S_OK == TranslateToLocalSpace(rStartPoint, outputStart)) &&
					// Get mouse end/current position relative to input image ROI.
					(S_OK == TranslateToLocalSpace(rEndPoint, outputEnd)) &&
					// Get ROI rectangle.								
					(S_OK == GetRectangle(l_oRect));


				if (l_bValid)
				{
					if (eLocation == SvDef::SVExtentLocationPropertyBottomLeft)
					{
						outputEnd.m_y = rEndPoint.m_y;
					}
					else if (eLocation == SvDef::SVExtentLocationPropertyTopRight)
					{
						outputEnd.m_x = rEndPoint.m_x;
					}
					else if (eLocation != SvDef::SVExtentLocationPropertyTop &&
						eLocation != SvDef::SVExtentLocationPropertyLeft &&
						eLocation != SvDef::SVExtentLocationPropertyTopLeft)
					{
						outputStart = rStartPoint;
						outputEnd = rEndPoint;
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
				outputStart = rStartPoint;
				outputEnd = rEndPoint;
				l_bValid = S_OK == GetRectangle(l_oRect);
				break;
			}

			default:
			{
				l_bValid = S_OK == TranslateToOutputSpace(rStartPoint, outputStart) &&
					S_OK == TranslateToOutputSpace(rEndPoint, outputEnd) &&
					S_OK == GetOutputRectangle(l_oRect);
			}
		}

		if (l_bValid)
		{
			switch (eLocation)
			{
				case SvDef::SVExtentLocationPropertyTopLeft:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvDef::SVExtentLocationPropertyTopRight:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x - l_oRect.right), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvDef::SVExtentLocationPropertyBottomRight:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x - l_oRect.right), static_cast<long> (outputEnd.m_y - l_oRect.bottom));

					break;
				}
				case SvDef::SVExtentLocationPropertyBottomLeft:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y - l_oRect.bottom));

					break;
				}
				case SvDef::SVExtentLocationPropertyLeft:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvDef::SVExtentLocationPropertyRight:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x - l_oRect.right), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvDef::SVExtentLocationPropertyTop:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y));

					break;
				}
				case SvDef::SVExtentLocationPropertyBottom:
				{
					l_hrOk = UpdateFromOutputSpace(eLocation, static_cast<long> (outputEnd.m_x), static_cast<long> (outputEnd.m_y - l_oRect.bottom));

					break;
				}
				case SvDef::SVExtentLocationPropertyCenter:
				{
					long l_lX = static_cast<long> (rEndPoint.m_x - rStartPoint.m_x);
					long l_lY = static_cast<long> (rEndPoint.m_y - rStartPoint.m_y);

					l_hrOk = UpdateFromOutputSpace(eLocation, l_lX, l_lY);

					break;
				}
				case SvDef::SVExtentLocationPropertyRotate:
				{
					SVPoint<double> position;

					if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position))
					{
						double l_dAngle = SVGetRotationAngle(position, rEndPoint);

						if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyRotationAngle, l_dAngle))
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

HRESULT SVImageExtentClass::UpdateFromOutputSpace(SvDef::SVExtentLocationPropertyEnum p_eLocation, long lX, long lY)
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
	{
		case SvDef::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;

			if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint))
			{
				switch (p_eLocation)
				{
					case SvDef::SVExtentLocationPropertyTop:
					{
						startPoint.m_x += lX;
						startPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyLeft:
					{
						startPoint.m_x += lX;
						startPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottom:
					{
						endPoint.m_x += lX;
						endPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyRight:
					{
						endPoint.m_x += lX;
						endPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyCenter:
					{
						startPoint.m_x += lX;
						startPoint.m_y += lY;

						endPoint.m_x += lX;
						endPoint.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint) &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint))
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
			SVPoint<double> position;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyHeight, l_dHeight) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyWidth, l_dWidth))
			{
				switch (p_eLocation)
				{
					case SvDef::SVExtentLocationPropertyTopRight:
					{
						l_dWidth += lX;
						l_dHeight -= lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += lX;
						l_dHeight += lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyRight:
					{
						l_dWidth += lX;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyTop:
					{
						l_dHeight -= lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottom:
					{
						l_dHeight += lY * 2.0;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyCenter:
					{
						position.m_x += lX;
						position.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (0.0 < l_dWidth && 0.0 < l_dHeight &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyHeight, l_dHeight) &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyWidth, l_dWidth))
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
			SVPoint<double> position;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyHeight, l_dHeight) &&
				S_OK == GetExtentProperty(SvDef::SVExtentPropertyWidth, l_dWidth))
			{
				switch (p_eLocation)
				{
					case SvDef::SVExtentLocationPropertyTopLeft:
					{
						position.m_x += lX;
						position.m_y += lY;

						l_dWidth -= lX;
						l_dHeight -= lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyTopRight:
					{
						position.m_y += lY;

						l_dWidth += lX;
						l_dHeight -= lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottomRight:
					{
						l_dWidth += lX;
						l_dHeight += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottomLeft:
					{
						position.m_x += lX;

						l_dWidth -= lX;
						l_dHeight += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyLeft:
					{
						position.m_x += lX;

						l_dWidth -= lX;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyRight:
					{
						l_dWidth += lX;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyTop:
					{
						position.m_y += lY;

						l_dHeight -= lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyBottom:
					{
						l_dHeight += lY;

						l_hrOk = S_OK;

						break;
					}
					case SvDef::SVExtentLocationPropertyCenter:
					{
						position.m_x += lX;
						position.m_y += lY;

						l_hrOk = S_OK;

						break;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (0.0 < l_dWidth && 0.0 < l_dHeight &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyHeight, l_dHeight) &&
						S_OK == SetExtentProperty(SvDef::SVExtentPropertyWidth, l_dWidth))
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
			l_hrOk = UpdatePolarFromOutputSpace(p_eLocation, lX, lY);
			break;
		}
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			switch (p_eLocation)
			{
				case SvDef::SVExtentLocationPropertyTop:
				case SvDef::SVExtentLocationPropertyBottom:
				case SvDef::SVExtentLocationPropertyCenter:
				{
					SVPoint<double> position;

					if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
					{
						position.m_y += lY;
						l_hrOk = S_OK;
					}

					if (S_OK == l_hrOk)
					{
						if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
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
			switch (p_eLocation)
			{
				case SvDef::SVExtentLocationPropertyLeft:
				case SvDef::SVExtentLocationPropertyCenter:
				case SvDef::SVExtentLocationPropertyRight:
				{
					SVPoint<double> position;

					if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
					{
						position.m_x += lX;
						l_hrOk = S_OK;
					}

					if (S_OK == l_hrOk)
					{
						if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
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

HRESULT SVImageExtentClass::GetRectangle(RECT &p_roRect) const
{
	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	HRESULT result = GetExtentProperty(SvDef::SVExtentPropertyPositionPointX, l_oRect.left);

	if (S_OK == result)
	{
		result = GetExtentProperty(SvDef::SVExtentPropertyPositionPointY, l_oRect.top);
	}

	if (S_OK == result)
	{
		result = GetExtentProperty(SvDef::SVExtentPropertyWidth, l_oRect.right);

		l_oRect.right += l_oRect.left;
	}

	if (S_OK == result)
	{
		result = GetExtentProperty(SvDef::SVExtentPropertyHeight, l_oRect.bottom);

		l_oRect.bottom += l_oRect.top;
	}

	if (S_OK == result)
	{
		p_roRect = l_oRect;
	}

	return result;
}

HRESULT SVImageExtentClass::GetLogicalRectangle(RECT &p_roRect) const
{
	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	HRESULT result = GetExtentProperty(SvDef::SVExtentPropertyWidth, l_oRect.right);

	if (S_OK == result)
	{
		result = GetExtentProperty(SvDef::SVExtentPropertyHeight, l_oRect.bottom);
	}

	if (S_OK == result)
	{
		p_roRect = l_oRect;
	}

	return result;
}

HRESULT SVImageExtentClass::GetOutputRectangle(RECT &p_roRect) const
{
	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	HRESULT result = GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, l_oRect.right);

	if (S_OK == result)
	{
		result = GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, l_oRect.bottom);
	}

	if (S_OK == result)
	{
		p_roRect = l_oRect;
	}

	return result;
}

HRESULT SVImageExtentClass::TranslateToOutputSpace(SVPoint<double> value, SVPoint<double> &rResult) const
{
	HRESULT l_hrOk {S_OK};

	rResult.clear();

	switch (m_eTranslation)
	{
		case SvDef::SVExtentTranslationNone:
		case SvDef::SVExtentTranslationFigureShift:
		{
			rResult = value;

			break;
		}
		case SvDef::SVExtentTranslationShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> displacement;

				GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);

				rResult = value - position - displacement;
			}

			break;
		}// end multiple case:
		case SvDef::SVExtentTranslationProfileShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rResult = value - position;
			}

			break;
		}// end multiple case:
		case SvDef::SVExtentTranslationTransformShift:
		{
			SVPoint<double> position;
			SVPoint<double> displacement;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				rResult = value - position - displacement;
			}

			break;
		}
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationLinear:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				rResult = SVRotatePoint(rotation, value, -dAngle);

				rResult = rResult - rotation + outputRotation;
			}

			break;
		}// end mulitple case:
		case SvDef::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;
			SVPoint<double> displacement;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				rResult = SVRotatePoint(rotation, value, -dAngle);

				rResult = rResult - rotation + outputRotation + displacement;
			}

			break;
		}
		case SvDef::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;
			SVPoint<double> outputRotation;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				double dAngle = SVGetRotationAngle(startPoint, endPoint);

				rResult = SVRotatePoint(startPoint, value, -dAngle);

				rResult = rResult - startPoint + outputRotation;
			}

			break;
		}// end case SvDef::SVExtentTranslationLine:
		case SvDef::SVExtentTranslationFlippedRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				rResult = SVRotateAndFlipPoint(rotation, value, -dAngle);

				rResult = rResult - rotation + outputRotation;
			}

			break;
		}// end case SvDef::SVExtentTranslationFlippedRotate:

		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			SVPoint<double> centerPoint;

			double dStartAngle = 0.0;
			double dOuterRadius = 0.0;
			double dInnerRadius = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, centerPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dOuterRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dInnerRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle);
			}

			if (S_OK == l_hrOk)
			{
				double l_dRadius = SVGetRadius(centerPoint, value);
				double l_dAngle = SVGetFlippedRotationAngle(centerPoint, value);

				l_dAngle -= dStartAngle;
				l_dAngle -= ((long)(l_dAngle) / 360) * 360.0;

				if (dOuterRadius > dInnerRadius)
				{
					rResult.m_x = Degrees2Radians(l_dAngle) * dOuterRadius;
					rResult.m_y = l_dRadius - dInnerRadius;
				}
				else
				{
					rResult.m_x = Degrees2Radians(l_dAngle) * dInnerRadius;
					rResult.m_y = l_dRadius - dOuterRadius;
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationPolarUnwrap:

		case SvDef::SVExtentTranslationCylindricalWarpH:
		{
			double dInputWidth = 0.0;
			double dOutputWidth = 0.0;
			double dAngle = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dInputWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dOutputWidth);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dAngle);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_y = value.m_y;
				double l_dInputRadius = dInputWidth / 2.0;
				double l_dRadius = (dInputWidth / std::sin(dAngle / 360.0 * M_PI)) / 2.0;
				if (value.m_x < l_dInputRadius)
				{
					double l_dXFromCenter = l_dInputRadius - value.m_x;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dXFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_x = dOutputWidth / 2.0 - l_dCircum;
				}
				else
				{
					double l_dXFromCenter = value.m_x - l_dInputRadius;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dXFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_x = dOutputWidth / 2.0 + l_dCircum;
				}
			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpH:

		case SvDef::SVExtentTranslationCylindricalWarpV:
		{
			double dInputHeight = 0.0;
			double dOutputHeight = 0.0;
			double dAngle = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dInputHeight);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dOutputHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dAngle);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_x = value.m_x;
				double l_dInputRadius = dInputHeight / 2.0; // small height
				double l_dRadius = (dInputHeight / std::sin(dAngle / 360.0 * M_PI)) / 2.0;
				if (value.m_y < l_dInputRadius) // Quadrant detection
				{
					double l_dYFromCenter = l_dInputRadius - value.m_y;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dYFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_y = dOutputHeight / 2.0 - l_dCircum;
				}
				else
				{
					double l_dYFromCenter = value.m_y - l_dInputRadius;
					double l_dAngleAtPoint = 90.0 - std::acos(l_dYFromCenter / l_dRadius);
					double l_dCircum = l_dAngleAtPoint / 180.0 * M_PI * l_dRadius;
					rResult.m_y = dOutputHeight / 2.0 + l_dCircum;
				}

			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpV:
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dyOffset = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetY, dyOffset);
			}
			if (S_OK == l_hrOk)
			{
				rResult = value;			// Copy points 1 to 1. all X and Y when center points.
				double l_dHeight = dFullHeight / 2; // One Quadrants Width
				double l_dWidth = dFullWidth / 2;	// One Quadrant
				double l_dYhyp;						// Point on hypotnuse

				if (value.m_y < l_dHeight &&  value.m_x < l_dWidth)
				{
					//******** Upper Left Quadrant ******************
					l_dYhyp = dyOffset - (dyOffset / l_dWidth) * value.m_x;
					rResult.m_y = l_dHeight - (l_dHeight - value.m_y)* l_dHeight / (l_dHeight - l_dYhyp);
				}
				else
					if (value.m_y > l_dHeight && value.m_x < l_dWidth)
					{
						//******** Lower Left Quadrant ******************
						l_dYhyp = dFullHeight - dyOffset + (dyOffset / l_dWidth) * value.m_x;
						rResult.m_y = l_dHeight + (l_dHeight - value.m_y)* l_dHeight / (l_dHeight - l_dYhyp);
					}
					else
						if (value.m_y < l_dHeight && value.m_x > l_dWidth)
						{
							//******** Upper Right Quadrant ******************
							l_dYhyp = dyOffset - (dyOffset / l_dWidth) * (dFullWidth - value.m_x);
							rResult.m_y = l_dHeight - (l_dHeight - value.m_y)* l_dHeight / (l_dHeight - l_dYhyp);
						}
						else
							if (value.m_y > l_dHeight && value.m_x > l_dWidth)
							{
								//******** Lower Right Quadrant ******************
								l_dYhyp = dFullHeight - (dyOffset / l_dWidth) * (value.m_x - l_dWidth);
								rResult.m_y = l_dHeight + (l_dHeight - value.m_y)* l_dHeight / (l_dHeight - l_dYhyp);

							}
			}

			break;
		}
		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dxOffset = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetX, dxOffset);
			}
			if (S_OK == l_hrOk)
			{
				rResult = value;			// Copy points 1 to 1. all X and Y when center points.
				double dHeight = dFullHeight / 2; // One Quadrants Width
				double dWidth = dFullWidth / 2;	// One Quadrant
				double dXhyp;						// Point on hypotenuse

				if (value.m_y < dHeight &&  value.m_x < dWidth)
				{
					//******** Upper Left Quadrant ******************
					dXhyp = dxOffset - (dxOffset / dHeight) * value.m_y;
					rResult.m_x = dWidth - (dWidth - value.m_x)* dWidth / (dWidth - dXhyp);
				}
				else
					if (value.m_y > dHeight && value.m_x < dWidth)
					{
						//******** Lower Left Quadrant ******************
						dXhyp = dxOffset - (dxOffset / dHeight) * (dFullHeight - value.m_y);
						rResult.m_x = dWidth - (dWidth - value.m_x)* dWidth / (dWidth - dXhyp);
					}
					else
						if (value.m_y < dHeight && value.m_x > dWidth)
						{
							//******** Upper Right Quadrant ******************
							dXhyp = dFullWidth - dxOffset + (dxOffset / dHeight) * value.m_y;
							rResult.m_x = dWidth + (dWidth - value.m_x)* dWidth / (dWidth - dXhyp);
						}
						else
							if (value.m_y > dHeight && value.m_x > dWidth)
							{
								//******** Lower Right Quadrant ******************
								dXhyp = dFullWidth - (dxOffset / dHeight) * (value.m_y - dHeight);
								rResult.m_x = dWidth + (dWidth - value.m_x)* dWidth / (dWidth - dXhyp);

							}
			}

			break;
		}

		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationResize:
		{
			SVPoint<double> position;
			double heightScaleFactor = 1.0;
			double widthScaleFactor = 1.0;

			//@WARNING [Jim][8 July 2015]  No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidthScaleFactor, widthScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				rResult.m_x = (value.m_x - position.m_x) * widthScaleFactor;
				rResult.m_y = (value.m_y - position.m_y) * heightScaleFactor;
			}

			break;
		}

		case SvDef::SVExtentTranslationFlipVertical:
		{
			SVPoint<double> position;
			double dHeight = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight);
			}

			if (S_OK == l_hrOk)
			{
				rResult.m_x = value.m_x - position.m_x;
				rResult.m_y = (dHeight - 1) - (value.m_y - position.m_y);
			}

			break;
		}
		case SvDef::SVExtentTranslationFlipHorizontal:
		{
			SVPoint<double> position;
			double dWidth = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);
			}

			if (S_OK == l_hrOk)
			{
				rResult.m_x = (dWidth - 1) - (value.m_x - position.m_x);
				rResult.m_y = value.m_y - position.m_y;
			}

			break;
		}
		default:
		{
			l_hrOk = S_FALSE;

			break;
		}
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVPoint<double> value, SVPoint<double>& rResult) const
{
	HRESULT l_hrOk {S_OK};

	rResult.clear();

	switch (m_eTranslation)
	{
		case SvDef::SVExtentTranslationNone:
		{
			rResult = value;

			l_hrOk = S_OK;

			break;
		}

		case SvDef::SVExtentTranslationFigureShift:
		case SvDef::SVExtentTranslationProfileShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rResult = value + position;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> displacement;

				GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);

				rResult = value + position + displacement;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationTransformShift:
		{
			SVPoint<double> position;
			SVPoint<double> displacement;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				rResult = value + position + displacement;
			}

			break;
		}
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationLinear:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SVRotatePoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += rotation.m_x;
				rResult.m_y += rotation.m_y;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;
			SVPoint<double> displacement;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SVRotatePoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += rotation.m_x;
				rResult.m_y += rotation.m_y;

				rResult.m_x += displacement.m_x;
				rResult.m_y += displacement.m_y;
			}

			break;
		}
		case SvDef::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;
			SVPoint<double> outputRotation;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				double dAngle = SVGetRotationAngle(startPoint, endPoint);

				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SVRotatePoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += startPoint.m_x;
				rResult.m_y += startPoint.m_y;
			}

			break;
		}// end case SvDef::SVExtentTranslationLine:

		case SvDef::SVExtentTranslationFlippedRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SVRotateAndFlipPoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += rotation.m_x;
				rResult.m_y += rotation.m_y;
			}

			break;
		}// end case SvDef::SVExtentTranslationFlippedRotate:

		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			SVPoint<double> centerPoint;

			double dStartAngle = 0.0;
			double dOuterRadius = 0.0;
			double dInnerRadius = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, centerPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dOuterRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dInnerRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle);
			}

			if (S_OK == l_hrOk)
			{
				double l_dRadius;
				double l_dAngle;
				if (dOuterRadius > dInnerRadius)
				{
					l_dRadius = value.m_y + dInnerRadius;
					l_dAngle = Radians2Degrees(value.m_x / dOuterRadius) + dStartAngle;
				}
				else
				{
					l_dRadius = dInnerRadius - value.m_y;
					l_dAngle = Radians2Degrees(value.m_x / dInnerRadius) + dStartAngle;
				}

				rResult = SVRotateAndFlipPoint(centerPoint, l_dRadius, l_dAngle);
			}

			break;
		}// end case SvDef::SVExtentTranslationPolarUnwrap:

		case SvDef::SVExtentTranslationCylindricalWarpH:
		{
			double dInputWidth = 0.0;
			double dWarpAngle = 0.0;
			double dOutputWidth = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dInputWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dOutputWidth);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_y = value.m_y;

				double l_dwidth = dOutputWidth / 2.0; // small width
				double l_dRadius = (dInputWidth / std::sin(dWarpAngle / 360.0 * M_PI)) / 2.0;
				if (value.m_x < l_dwidth)
				{
					double l_dXFromCenter = l_dwidth - value.m_x;
					double l_dAngleAtPoint = (l_dXFromCenter * 180.0) / (M_PI * l_dRadius);
					rResult.m_x = (dInputWidth - 1) / 2.0 - std::cos((90.0 - l_dAngleAtPoint) / 180.0 * M_PI) * l_dRadius;
				}
				else
				{
					double l_dXFromCenter = value.m_x - l_dwidth;
					double l_dAngleAtPoint = (l_dXFromCenter * 180.0) / (M_PI * l_dRadius);
					rResult.m_x = (dInputWidth - 1) / 2.0 + std::cos((90.0 - l_dAngleAtPoint) / 180.0 * M_PI) * l_dRadius;
				}

			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpH:

		case SvDef::SVExtentTranslationCylindricalWarpV:
		{
			double dInputHeight = 0.0;
			double dWarpAngle = 0.0;
			double dOutputHeight = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dInputHeight);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dOutputHeight);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_x = value.m_x;
				double l_dheight = dOutputHeight / 2.0; // small height
				double l_dRadius = (dInputHeight / std::sin(dWarpAngle / 360.0 * M_PI)) / 2;
				if (value.m_y < l_dheight)
				{
					double l_dYFromCenter = l_dheight - value.m_y;
					double l_dAngleAtPoint = (l_dYFromCenter * 180.0) / (M_PI * l_dRadius);
					rResult.m_y = (dInputHeight - 1) / 2.0 - std::cos((90.0 - l_dAngleAtPoint) / 180.0 * M_PI) * l_dRadius;
				}
				else
				{
					double l_dYFromCenter = value.m_y - l_dheight;
					double l_dAngleAtPoint = (l_dYFromCenter * 180.0) / (M_PI * l_dRadius);
					rResult.m_y = (dInputHeight - 1) / 2.0 + std::cos((90.0 - l_dAngleAtPoint) / 180.0 * M_PI) * l_dRadius;
				}
			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpV:
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dyOffset = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetY, dyOffset);
			}
			if (S_OK == l_hrOk)
			{
				rResult = value;			// Copy points 1 to 1. all X and Y when center points.
				double dHeight = dFullHeight / 2; // One Quadrants Width
				double dWidth = dFullWidth / 2;	// One Quadrant
				double dYhyp;	// Point on hypotnuse


				if (value.m_y < dHeight &&  value.m_x < dWidth)
				{
					//******** Upper Left Quadrant ******************
					dYhyp = dyOffset - (dyOffset / dWidth) * value.m_x;
					rResult.m_y = dHeight - ((dHeight - dYhyp)*(dHeight - value.m_y)) / dHeight;
				}
				else
					if (value.m_y > dHeight && value.m_x < dWidth)
					{
						//******** Lower Left Quadrant ******************
						dYhyp = dFullHeight - dyOffset + (dyOffset / dWidth) * value.m_x;
						rResult.m_y = dHeight - ((dHeight - dYhyp)*(value.m_y - dHeight)) / dHeight;
					}
					else
						if (value.m_y < dHeight && value.m_x > dWidth)
						{
							//******** Upper Right Quadrant ******************
							dYhyp = dyOffset - (dyOffset / dWidth) * (dFullWidth - value.m_x);
							rResult.m_y = dHeight - ((dHeight - value.m_y)*(dHeight - dYhyp)) / dHeight;
						}
						else
							if (value.m_y > dHeight && value.m_x > dWidth)
							{
								//******** Lower Right Quadrant ******************
								dYhyp = dFullHeight - (dyOffset / dWidth) * (value.m_x - dWidth);
								rResult.m_y = dHeight - ((value.m_y - dHeight)*(dHeight - dYhyp)) / dHeight;
							}
			}
			break;
		}// end case SvDef::SVExtentTranslationVerticalPerspective:

		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dxOffset = 0.0;
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetX, dxOffset);
			}
			if (S_OK == l_hrOk)
			{
				rResult = value;			// Copy points 1 to 1. all X and Y when center points.
				double dWidth = dFullWidth / 2;	// One Quadrants Width
				double dHeight = dFullHeight / 2;	// One Quadrant
				double dXhyp;	// Point on hypotenuse


				if (value.m_x < dWidth &&  value.m_y < dHeight)
				{
					//******** Upper Left Quadrant ******************
					dXhyp = dxOffset - (dxOffset / dHeight) * value.m_y;
					rResult.m_x = dWidth - ((dWidth - dXhyp)*(dWidth - value.m_x)) / dWidth;
				}
				else
					if (value.m_x < dWidth && value.m_y > dHeight)
					{
						//******** Lower Left Quadrant ******************
						dXhyp = dxOffset - (dxOffset / dHeight) * (dFullHeight - value.m_y);
						rResult.m_x = dWidth - ((dWidth - value.m_x)*(dWidth - dXhyp)) / dWidth;
					}
					else
						if (value.m_x > dWidth && value.m_y < dHeight)
						{
							//******** Upper Right Quadrant ******************
							dXhyp = dFullWidth - dxOffset + (dxOffset / dHeight) *  value.m_y;
							rResult.m_x = dWidth - ((value.m_x - dWidth)*(dWidth - dXhyp)) / dWidth;
						}
						else
							if (value.m_x > dWidth && value.m_y > dHeight)
							{
								//******** Lower Right Quadrant ******************
								dXhyp = dFullWidth - (dxOffset / dHeight) * (value.m_y - dHeight);
								rResult.m_x = dWidth - ((value.m_x - dWidth)*(dWidth - dXhyp)) / dWidth;
							}
			}
			break;
		}// end case SvDef::SVExtentTranslationHorizontalPerspective:

		//new cases for Flip Horizontal & Vertical
		case SvDef::SVExtentTranslationFlipVertical:
		{
			SVPoint<double> position;
			double dFullHeight = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_y = (dFullHeight - 1) - value.m_y;
				rResult.m_x = value.m_x;
			}
			break;
		}
		case SvDef::SVExtentTranslationFlipHorizontal:
		{
			SVPoint<double> position;
			double dFullWidth = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dFullWidth);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_x = (dFullWidth - 1) - value.m_x;
				rResult.m_y = value.m_y;
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

			SVPoint<double> position;

			double heightScaleFactor = 1.0;
			double widthScaleFactor = 1.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidthScaleFactor, widthScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				// Unscale and make relative to the Parent (input) image
				rResult.m_x = (value.m_x / widthScaleFactor) + position.m_x;
				rResult.m_y = (value.m_y / heightScaleFactor) + position.m_y;
			}
			break;
		}
	}// end switch( m_eTranslation )

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVExtentFigureStruct p_svValue, SVExtentFigureStruct &p_rsvResult) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
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
			switch (p_svValue.m_eShape)
			{
				case SvDef::SVExtentShapeArrow:
				{
					l_hrOk = S_OK;

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, -90.0, p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 0.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft, 90.0, p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight, -135.0, p_svValue.m_svTopRight, p_rsvResult.m_svTopRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight, 135.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SvDef::SVExtentShapeRectangle:
				{
					l_hrOk = S_OK;

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 0.0, p_svValue.m_svTopRight, p_rsvResult.m_svTopRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 90.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft, 90.0, p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svTopRight, p_rsvResult.m_svTopRight, 90.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svTopRight, p_rsvResult.m_svTopRight, 90.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft, 0.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft, 0.0, p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SvDef::SVExtentShapeLineHorizontal:
				{
					l_hrOk = S_OK;

					SVPoint<double> rotatePoint;
					SVPoint<double> outputRotationPoint;

					GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotationPoint);

					SVPoint<double> radiusPoint = p_svValue.m_svCenterLeft;

					if (radiusPoint.m_y != outputRotationPoint.m_y)
					{
						radiusPoint.m_y = outputRotationPoint.m_y;
					}

					if (TranslateFromOutputSpace(radiusPoint, rotatePoint) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					double l_dNewAngle = 0.0;

					if (p_svValue.m_svCenterLeft.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -180.0;
					}
					else
					{
						l_dNewAngle = 0.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 0.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svTopLeft = p_rsvResult.m_svCenterLeft;
					p_rsvResult.m_svBottomLeft = p_rsvResult.m_svCenterLeft;

					if (p_svValue.m_svCenterRight.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -180.0;
					}
					else
					{
						l_dNewAngle = 0.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 0.0, p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svTopRight = p_rsvResult.m_svCenterRight;
					p_rsvResult.m_svBottomRight = p_rsvResult.m_svCenterRight;

					if (p_svValue.m_svTopCenter.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -180.0;
					}
					else
					{
						l_dNewAngle = 0.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 0.0, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svBottomCenter = p_rsvResult.m_svTopCenter;

					break;
				}
				case SvDef::SVExtentShapeLineVertical:
				{
					l_hrOk = S_OK;

					SVPoint<double> rotatePoint;
					SVPoint<double> outputRotationPoint;

					GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotationPoint);

					SVPoint<double> radiusPoint = p_svValue.m_svTopCenter;

					if (radiusPoint.m_y != outputRotationPoint.m_y)
					{
						radiusPoint.m_y = outputRotationPoint.m_y;
					}

					if (TranslateFromOutputSpace(radiusPoint, rotatePoint) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					double l_dNewAngle = 0.0;

					if (p_svValue.m_svTopCenter.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -90.0;
					}
					else
					{
						l_dNewAngle = 90.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, l_dNewAngle, p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svTopLeft = p_rsvResult.m_svTopCenter;
					p_rsvResult.m_svTopRight = p_rsvResult.m_svTopCenter;

					if (p_svValue.m_svBottomCenter.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -90.0;
					}
					else
					{
						l_dNewAngle = 90.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 90.0, p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					p_rsvResult.m_svBottomLeft = p_rsvResult.m_svBottomCenter;
					p_rsvResult.m_svBottomRight = p_rsvResult.m_svBottomCenter;

					if (p_svValue.m_svCenterLeft.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -90.0;
					}
					else
					{
						l_dNewAngle = 90.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 90.0, p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft) != S_OK)
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

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svBottomCenter, p_rsvResult.m_svBottomCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svBottomLeft, p_rsvResult.m_svBottomLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svBottomRight, p_rsvResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svCenterLeft, p_rsvResult.m_svCenterLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svCenterRight, p_rsvResult.m_svCenterRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svTopCenter, p_rsvResult.m_svTopCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svTopLeft, p_rsvResult.m_svTopLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(p_svValue.m_svTopRight, p_rsvResult.m_svTopRight))
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

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVExtentLineStruct p_svValue, SVExtentLineStruct &p_rsvResult) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
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
			long l_lCount = static_cast<long> (p_svValue.m_PointVector.size());

			l_hrOk = S_OK;
			p_rsvResult.Initialize();

			if (0 < l_lCount)
			{
				double l_dAngle = 0.0;

				bool l_bIsAngle = S_OK == GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, l_dAngle);

				p_rsvResult.m_bIsAngleValid = p_svValue.m_bIsAngleValid;
				p_rsvResult.m_dAngle = p_svValue.m_dAngle;

				if (!l_bIsAngle || l_dAngle == 0.0 || !p_svValue.m_bIsAngleValid)
				{
					for (long i = 0; S_OK == l_hrOk && i < l_lCount; i++)
					{
						SVPoint<double> point = p_svValue.m_PointVector[i];

						if (S_OK == TranslateFromOutputSpace(point, point))
						{
							p_rsvResult.m_PointVector.push_back(point);
						}
						else
						{
							l_hrOk = S_FALSE;
						}
					}
				}
				else
				{
					SVPoint<double> rotatePoint;
					SVPoint<double> outputRotationPoint;

					GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotationPoint);

					SVPoint<double> l_svRadiusPoint = p_svValue.m_PointVector[0];

					long l_lStart = 0;

					if (p_svValue.m_dAngle == 0.0 && l_svRadiusPoint.m_x != outputRotationPoint.m_x)
					{
						l_svRadiusPoint.m_x = outputRotationPoint.m_x;
					}
					else if (p_svValue.m_dAngle == 90.0 && l_svRadiusPoint.m_y != outputRotationPoint.m_y)
					{
						l_svRadiusPoint.m_y = outputRotationPoint.m_y;
					}
					else
					{
						l_lStart = 1;
					}

					if (S_OK == TranslateFromOutputSpace(l_svRadiusPoint, rotatePoint))
					{
						if (l_lStart == 1)
						{
							p_rsvResult.m_PointVector.push_back(rotatePoint);
						}
					}
					else
					{
						l_hrOk = S_FALSE;
					}

					for (long i = l_lStart; S_OK == l_hrOk && i < l_lCount; i++)
					{
						SVPoint<double> point = p_svValue.m_PointVector[i];

						double l_dNewAngle = p_svValue.m_dAngle;

						if (l_dNewAngle == 0.0)
						{
							if (point.m_x < l_svRadiusPoint.m_x)
							{
								l_dNewAngle -= 180;
							}
						}
						else if (l_dNewAngle == 90.0)
						{
							if (point.m_y < l_svRadiusPoint.m_y)
							{
								l_dNewAngle -= 180;
							}
						}

						if (S_OK == TranslateLineFromOutputSpace(l_svRadiusPoint, rotatePoint, l_dNewAngle, point, point))
						{
							p_rsvResult.m_PointVector.push_back(point);
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

	if (S_OK == l_hrOk)
	{
		p_rsvResult.m_dwColor = p_svValue.m_dwColor;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVExtentMultiLineStruct p_svValue, SVExtentMultiLineStruct &p_rsvResult) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
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

			p_rsvResult.Initialize();

			l_hrOk = TranslateFromOutputSpace(p_svValue.m_StringPoint, p_rsvResult.m_StringPoint);

			for (long i = 0; S_OK == l_hrOk && i < l_lCount; i++)
			{
				SVExtentLineStruct svLine = p_svValue.m_svLineArray[i];

				if (S_OK == TranslateFromOutputSpace(svLine, svLine))
				{
					p_rsvResult.m_svLineArray.push_back(svLine);
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}

			break;
		}
	}

	if (S_OK == l_hrOk)
	{
		p_rsvResult.m_Color = p_svValue.m_Color;

		p_rsvResult.m_StringPoint = p_svValue.m_StringPoint;
		p_rsvResult.m_csString = p_svValue.m_csString;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateLineFromOutputSpace(const SVPoint<double>& rRadiusPoint, const SVPoint<double>& rRotatePoint, double dLineAngle, const SVPoint<double>& rValue, SVPoint<double>& rResult) const
{
	HRESULT l_hrOk {S_OK};

	rResult.clear();

	switch (m_eTranslation)
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
			l_hrOk = TranslateFromOutputSpace(rValue, rResult);

			break;
		}// end case multiple

		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationTransformRotate:
		case SvDef::SVExtentTranslationLinear:
		{
			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);

			if (S_OK == l_hrOk)
			{
				double l_dRadius = SVGetRadius(rRadiusPoint, rValue);

				rResult = SVRotatePoint(rRotatePoint, l_dRadius, dAngle + dLineAngle);
			}

			break;
		}// end case multiple

		case SvDef::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == l_hrOk)
			{
				double l_dAngle = SVGetRotationAngle(startPoint, endPoint);

				if (S_OK == l_hrOk)
				{
					double l_dRadius = SVGetRadius(rRadiusPoint, rValue);

					rResult = SVRotatePoint(rRotatePoint, l_dRadius, l_dAngle + dLineAngle);
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationLine:

		case SvDef::SVExtentTranslationFlippedRotate:
		{
			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);

			if (S_OK == l_hrOk)
			{
				double l_dRadius = SVGetRadius(rRadiusPoint, rValue);

				rResult = SVRotateAndFlipPoint(rRotatePoint, l_dRadius, dAngle - dLineAngle);
			}
		}// end case SvDef::SVExtentTranslationFlippedRotate:

	}// end switch( m_eTranslation )

	return l_hrOk;
}

HRESULT SVImageExtentClass::ValidateAgainstOutputSpace(const SVImageExtentClass &rImageExtent) const
{
	RECT l_oRect;

	HRESULT result = GetOutputRectangle(l_oRect);

	if (S_OK == result)
	{
		if (rImageExtent.hasFigure())
		{
			SVExtentFigureStruct figure = rImageExtent.GetFigure();

			if (rImageExtent.m_eTranslation == SvDef::SVExtentTranslationLine)
			{
				l_oRect.bottom--;
				l_oRect.right--;

				if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svCenterLeft)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svCenterRight)))
				{
					result = S_FALSE;
				}
			}
			else if (rImageExtent.m_eTranslation == SvDef::SVExtentTranslationProfile)
			{
				///When the center is within the Outputrectangle the LinearTool with rotation is valid 
				///figure.msvCenter is not calculated yet
				figure.m_svCenter = (figure.m_svTopLeft  + figure.m_svBottomRight)/2.0;
				if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svCenter)))
				{
					result = S_FALSE;
				}
			}
			else
			{
				l_oRect.bottom++;
				l_oRect.right++;


				if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svCenterLeft)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svCenterRight)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svBottomLeft)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svBottomCenter)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svBottomRight)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svTopLeft)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svTopCenter)))
				{
					result = S_FALSE;
				}
				else if (!::PtInRect(&l_oRect, static_cast<POINT> (figure.m_svTopRight)))
				{
					result = S_FALSE;
				}
			}
		}
		else
		{
			RECT l_oInRect;

			if (S_OK == rImageExtent.GetRectangle(l_oInRect))
			{
				RECT l_oOutRect;

				if (!::UnionRect(&l_oOutRect, &l_oRect, &l_oInRect) ||
					!::EqualRect(&l_oOutRect, &l_oRect))
				{
					result = S_FALSE;
				}
			}
		}
	}
	
	return result;
}

bool SVImageExtentClass::operator==(const SVImageExtentClass& rRhs) const
{
	return m_eTranslation == rRhs.m_eTranslation && m_extentValues == rRhs.m_extentValues && m_properties == rRhs.m_properties;
}

bool SVImageExtentClass::operator!=(const SVImageExtentClass& rRhs) const
{
	return !(*this == rRhs);
}

HRESULT SVImageExtentClass::UpdateSourceOffset(SVExtentOffsetStruct& rOffsetData) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
	{
		case SvDef::SVExtentTranslationNone:
		{
			rOffsetData.m_bIsLinear &= true;

			l_hrOk = S_OK;

			break;
		}

		case SvDef::SVExtentTranslationShift:
		{
			SVPoint<double>  position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> displacement;

				GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);

				SVPoint<double> result = position + displacement - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationProfileShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = position - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationRotate:
		case SvDef::SVExtentTranslationProfile:
		case SvDef::SVExtentTranslationLinear:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = rotation - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint = outputRotation;

				rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle + dAngle), 360.0);

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationTransformShift:
		{
			SVPoint<double> displacement;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = displacement - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvDef::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;
			SVPoint<double> displacement;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = rotation + displacement - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint = outputRotation;

				rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle + dAngle), 360.0);

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}
		case SvDef::SVExtentTranslationFlippedRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = rotation - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint = outputRotation;

				rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle - dAngle), 360.0);

				rOffsetData.m_bIsLinear &= true;
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
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = position - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear = false;
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

HRESULT SVImageExtentClass::UpdatePolarFromOutputSpace(SvDef::SVExtentLocationPropertyEnum eLocation, long p_dX, long p_dY)
{
	HRESULT l_hrOk = S_FALSE;
	SVPoint<double> position;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position)
		&& S_OK == GetExtentProperty(SvDef::SVExtentPropertyStartAngle, l_dStartAngle)
		&& S_OK == GetExtentProperty(SvDef::SVExtentPropertyEndAngle, l_dEndAngle)
		&& S_OK == GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius)
		&& S_OK == GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius))
	{
		switch (eLocation)
		{
			case SvDef::SVExtentLocationPropertyLeft:
			{
				double l_dNewAngle = l_dStartAngle + Radians2Degrees(p_dX) / l_dOuterRadius;

				if (l_dNewAngle > l_dEndAngle)
					l_dNewAngle = l_dEndAngle;

				l_dStartAngle = l_dNewAngle;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyRight:
			{
				double l_dNewAngle = l_dEndAngle + Radians2Degrees(p_dX) / l_dOuterRadius;

				if (l_dNewAngle < l_dStartAngle)
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
				position.m_x += p_dX;
				position.m_y += p_dY;

				l_hrOk = S_OK;

				break;
			}
		}// end switch ( p_eLocation )

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyStartAngle, l_dStartAngle) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyEndAngle, l_dEndAngle) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius))
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

HRESULT SVImageExtentClass::UpdateLine(SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> startPoint;
	SVPoint<double> endPoint;

	if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint) &&
		S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint))
	{
		SVPoint<double> point {rEnd};
		point -= rStart;

		switch (eLocation)
		{
			case SvDef::SVExtentLocationPropertyLeft:
			{
				startPoint += point;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyRight:
			{
				endPoint += point;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyCenter:
			{
				startPoint += point;
				endPoint += point;

				l_hrOk = S_OK;

				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint))
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

HRESULT SVImageExtentClass::UpdatePolar(SvDef::SVExtentLocationPropertyEnum p_eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> position;

	double l_dInnerRadius = 0.0;
	double l_dOuterRadius = 0.0;
	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;

	if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
		S_OK == GetExtentProperty(SvDef::SVExtentPropertyStartAngle, l_dStartAngle) &&
		S_OK == GetExtentProperty(SvDef::SVExtentPropertyEndAngle, l_dEndAngle) &&
		S_OK == GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius) &&
		S_OK == GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius))
	{
		double l_dNewAngle = SVGetFlippedRotationAngle(position, rEnd);
		double l_dNewRadius = SVGetRadius(position, rEnd);

		switch (p_eLocation)
		{
			case SvDef::SVExtentLocationPropertyTopLeft:
			{
				long l_lMult = (long)(l_dStartAngle) / 360;

				if (l_dStartAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dStartAngle))
				{
					if (l_dStartAngle < l_dNewAngle)
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
				long l_lMult = (long)(l_dEndAngle) / 360;

				if (l_dEndAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dEndAngle))
				{
					if (l_dEndAngle < l_dNewAngle)
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

				if (l_dEndAngle - l_dStartAngle > 720)
				{
					l_dStartAngle = l_dEndAngle - 720;
				}


				l_dInnerRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyBottomRight:
			{
				long l_lMult = (long)(l_dEndAngle) / 360;

				if (l_dEndAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dEndAngle))
				{
					if (l_dEndAngle < l_dNewAngle)
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

				if (l_dEndAngle - l_dStartAngle > 720)
				{
					l_dStartAngle = l_dEndAngle - 720;
				}


				l_dOuterRadius = l_dNewRadius;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyBottomLeft:
			{
				long l_lMult = (long)(l_dStartAngle) / 360;

				if (l_dStartAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dStartAngle))
				{
					if (l_dStartAngle < l_dNewAngle)
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
				long l_lMult = (long)(l_dStartAngle) / 360;

				if (l_dStartAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dStartAngle))
				{
					if (l_dStartAngle < l_dNewAngle)
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
				long l_lMult = (long)(l_dEndAngle) / 360;

				if (l_dEndAngle < 0)
				{
					l_lMult -= 1;
				}

				l_dNewAngle += (double)(l_lMult) * 360.0;

				if (180.0 < ::fabs(l_dNewAngle - l_dEndAngle))
				{
					if (l_dEndAngle < l_dNewAngle)
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

				if (l_dEndAngle - l_dStartAngle > 720)
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
				position += rEnd - rStart;

				l_hrOk = S_OK;

				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyStartAngle, l_dStartAngle) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyEndAngle, l_dEndAngle) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyInnerRadius, l_dInnerRadius) &&
				S_OK == SetExtentProperty(SvDef::SVExtentPropertyOuterRadius, l_dOuterRadius))
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

void SVImageExtentClass::ClearOutputData()
{
	m_figure.Initialize();

	DisableExtentProperty(SvDef::SVExtentPropertyPositionsOutput);
	DisableExtentProperty(SvDef::SVExtentPropertyDimentionsOutput);
	if (m_eTranslation == SvDef::SVExtentTranslationLine)
	{
		DisableExtentProperty(SvDef::SVExtentPropertyRotationAngle);
		DisableExtentProperty(SvDef::SVExtentPropertyWidth);
		DisableExtentProperty(SvDef::SVExtentPropertyHeight);
	}
	else
	{
		DisableExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine);
	}
}

HRESULT SVImageExtentClass::BuildOutputDimensions()
{
	HRESULT result {S_OK};
	ClearOutputData();

	switch (m_eTranslation)
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
			double dValue = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);
			result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue);

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end multiple case:

		case SvDef::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;

			double l_dValue = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			result = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == result)
			{
				double dWidth = SVGetRadius(startPoint, endPoint);

				result = SetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth, false);

				if (S_OK == result)
				{
					result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dWidth, false);
				}
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyHeight, 1.0, false);

				if (S_OK == result)
				{
					result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, 1.0, false);
				}
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
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

			double dValue = 0.0;
			double inputHeight = 0.0;
			double inputWidth = 0.0;
			double heightScaleFactor = 1.0;
			double widthScaleFactor = 1.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			result = GetExtentProperty(SvDef::SVExtentPropertyWidth, inputWidth);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyWidthScaleFactor, widthScaleFactor);
			}

			if (S_OK == result)
			{
				dValue = inputWidth * widthScaleFactor;
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, inputHeight);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			}

			if (S_OK == result)
			{
				dValue = inputHeight * heightScaleFactor;

				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationDoubleHeight:

		case SvDef::SVExtentTranslationFlippedRotate:
		case SvDef::SVExtentTranslationRotate:
		{
			double dValue = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationFlippedRotate: case SvDef::SVExtentTranslationRotate:

		case SvDef::SVExtentTranslationLinear:
		{
			double dHeight = 0.0;
			double dWidth = 0.0;
			double dAngle = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			result = GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dAngle);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dWidth, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dHeight, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationLinear:

		case SvDef::SVExtentTranslationProfile:
		{
			double dValue = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue);

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				SVPoint<double> point;

				point.m_y += dValue / 2;

				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, point, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationProfile:

		case SvDef::SVExtentTranslationPolarUnwrap:
		{
			double dInnerRadius = 0.0;
			double dOuterRadius = 0.0;
			double dStartAngle = 0.0;
			double dEndAngle = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyRectangle);

			if (S_OK != GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle))
			{
				result = S_FALSE;
			}

			if (S_OK != GetExtentProperty(SvDef::SVExtentPropertyEndAngle, dEndAngle))
			{
				result = S_FALSE;
			}

			if (S_OK != GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dInnerRadius))
			{
				result = S_FALSE;
			}

			if (S_OK != GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dOuterRadius))
			{
				result = S_FALSE;
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			if (S_OK == result)
			{
				double dHeight = fabs(dOuterRadius - dInnerRadius);
				if (dHeight < 1.0)
				{
					dHeight = 1.0;
				}
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dHeight, false);

				if (S_OK == result)
				{
					double l_dMaxRadius = dOuterRadius > dInnerRadius ? dOuterRadius : dInnerRadius;
					double l_dWidth = Degrees2Radians(dEndAngle - dStartAngle) * l_dMaxRadius;
					if (l_dWidth < 1.0)
					{
						l_dWidth = 1.0;
					}
					result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, l_dWidth, false);
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationPolarUnwrap:

		case SvDef::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> position;

			double dHeight = 0.0;
			double dWidth = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);
			}

			if (S_OK == result)
			{
				double dMax = std::sqrt((dWidth * dWidth) + (dHeight * dHeight));

				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dMax, false);

				if (S_OK == result)
				{
					result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dMax, false);
				}

				if (S_OK == result)
				{
					position.m_x += (dMax - dWidth) / 2;
					position.m_y += (dMax - dHeight) / 2;

					result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, position, false);
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationTransformRotate:

		case SvDef::SVExtentTranslationCylindricalWarpH:
		{
			double dValue = 0.0;
			double dWarpAngle = 0.0;

			result = S_OK;

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue);
			}
			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == result)
			{
				double l_dRadius = (dValue / std::sin(dWarpAngle / 360.0 * M_PI)) / 2.0;
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, l_dRadius * M_PI * (dWarpAngle / 180.0), false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dValue, false);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpH:

		case SvDef::SVExtentTranslationCylindricalWarpV:
		{
			double dValue = 0.0;
			double dWarpAngle = 0.0;

			result = S_OK;

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == result)
			{
				double dRadius = (dValue / std::sin(dWarpAngle / 360.0 * M_PI)) / 2.0;
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dRadius * M_PI * (dWarpAngle / 180.0), false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dValue, false);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpV:

		case SvDef::SVExtentTranslationVerticalPerspective:
		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			double dValue = 0.0;

			DisableExtentProperty(SvDef::SVExtentPropertyPie);

			result = GetExtentProperty(SvDef::SVExtentPropertyWidth, dValue);

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvDef::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationVerticalPerspective:

		default:
		{
			result = S_FALSE;

			break;
		}
	}

	return result;
}

HRESULT SVImageExtentClass::BuildFigure()
{
	HRESULT l_hrOk = m_figure.Initialize();

	if (S_OK == l_hrOk)
	{
		switch (m_eTranslation)
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
				double dWidth = 0.0;
				double dHeight = 0.0;

				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight);
				}

				if (S_OK == l_hrOk)
				{
					m_figure.m_eShape = SvDef::SVExtentShapeRectangle;

					m_figure.m_svTopCenter.m_x = static_cast<long>(dWidth / 2);

					m_figure.m_svTopRight.m_x = static_cast<long> (dWidth);

					m_figure.m_svCenterLeft.m_y = static_cast<long> (dHeight / 2);

					m_figure.m_svCenterRight.m_y = static_cast<long> (dHeight / 2);
					m_figure.m_svCenterRight.m_x = static_cast<long> (dWidth);

					m_figure.m_svBottomLeft.m_y = static_cast<long> (dHeight);

					m_figure.m_svBottomCenter.m_y = static_cast<long> (dHeight);
					m_figure.m_svBottomCenter.m_x = static_cast<long> (dWidth / 2);

					m_figure.m_svBottomRight.m_y = static_cast<long> (dHeight);
					m_figure.m_svBottomRight.m_x = static_cast<long> (dWidth);

					l_hrOk = TranslateFromOutputSpace(m_figure, m_figure);
				}

				break;
			}
			case SvDef::SVExtentTranslationDoubleHeight:
			case SvDef::SVExtentTranslationResize:
			case SvDef::SVExtentTranslationFlipHorizontal:
			case SvDef::SVExtentTranslationFlipVertical:
			case SvDef::SVExtentTranslationFigureShift:
			{
				double dWidth = 0.0;
				double dHeight = 0.0;

				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight);
				}

				SVPoint<double> startPoint;
				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, startPoint);
				}

				if (S_OK == l_hrOk)
				{
					m_figure.m_eShape = SvDef::SVExtentShapeRectangle;

					m_figure.m_svTopLeft = startPoint;

					//set top center
					m_figure.m_svTopCenter.m_x = startPoint.m_x + (dWidth / 2.0);
					m_figure.m_svTopCenter.m_y = startPoint.m_y;

					//set top right
					m_figure.m_svTopRight.m_x = startPoint.m_x + dWidth;
					m_figure.m_svTopRight.m_y = startPoint.m_y;

					//set center left
					m_figure.m_svCenterLeft.m_x = startPoint.m_x;
					m_figure.m_svCenterLeft.m_y = startPoint.m_y + (dHeight / 2.0);

					//set Center Right
					m_figure.m_svCenterRight.m_x = startPoint.m_x + dWidth;
					m_figure.m_svCenterRight.m_y = startPoint.m_y + (dHeight / 2.0);

					//set Bottom Left
					m_figure.m_svBottomLeft.m_x = startPoint.m_x;
					m_figure.m_svBottomLeft.m_y = startPoint.m_y + dHeight;

					//Set Bottom Center
					m_figure.m_svBottomCenter.m_x = startPoint.m_x + (dWidth / 2.0);
					m_figure.m_svBottomCenter.m_y = startPoint.m_y + (dHeight / 2.0);

					//Set Bottom Right
					m_figure.m_svBottomRight.m_x = startPoint.m_x + dWidth;
					m_figure.m_svBottomRight.m_y = startPoint.m_y + dHeight;
				}

				break;
			}

			case SvDef::SVExtentTranslationLinear:
			{
				double dWidth = 0.0;
				double dHeight = 0.0;

				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight);
				}

				if (S_OK == l_hrOk)
				{
					m_figure.m_eShape = SvDef::SVExtentShapeArrow;

					m_figure.m_svTopLeft.m_y = dHeight / 2 - 10;

					m_figure.m_svBottomLeft.m_y = dHeight / 2 + 10;

					m_figure.m_svCenterLeft.m_y = dHeight / 2;

					m_figure.m_svTopCenter.m_y = dHeight / 2;
					m_figure.m_svTopCenter.m_x = static_cast<long> (dWidth / 2);

					m_figure.m_svBottomCenter.m_y = dHeight / 2;
					m_figure.m_svBottomCenter.m_x = static_cast<long> (dWidth / 2);

					m_figure.m_svTopRight.m_y = dHeight / 2 - 10;
					m_figure.m_svTopRight.m_x = static_cast<long> (dWidth - 10);

					m_figure.m_svCenterRight.m_y = dHeight / 2;
					m_figure.m_svCenterRight.m_x = static_cast<long> (dWidth);

					m_figure.m_svBottomRight.m_y = dHeight / 2 + 10;
					m_figure.m_svBottomRight.m_x = static_cast<long> (dWidth - 10);

					l_hrOk = TranslateFromOutputSpace(m_figure, m_figure);
				}

				break;
			}
			case SvDef::SVExtentTranslationLine:
			{
				double dWidth = 0.0;

				l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					m_figure.m_eShape = SvDef::SVExtentShapeArrow;

					m_figure.m_svTopLeft.m_y = -10;

					m_figure.m_svTopCenter.m_x = static_cast<long> (dWidth / 2);

					m_figure.m_svTopRight.m_y = -10;
					m_figure.m_svTopRight.m_x = static_cast<long> (dWidth - 10);

					m_figure.m_svCenterRight.m_x = static_cast<long> (dWidth);

					m_figure.m_svBottomLeft.m_y = 10;

					m_figure.m_svBottomCenter.m_x = static_cast<long> (dWidth / 2);

					m_figure.m_svBottomRight.m_y = 10;
					m_figure.m_svBottomRight.m_x = static_cast<long> (dWidth - 10);

					l_hrOk = TranslateFromOutputSpace(m_figure, m_figure);
				}

				break;
			}
			case SvDef::SVExtentTranslationPolarUnwrap:
			{
				SVPoint<double> centerPoint;

				double dStartAngle = 0.0;
				double dEndAngle = 0.0;
				double dInnerRadius = 0.0;
				double dOuterRadius = 0.0;

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, centerPoint);
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyStartAngle, dStartAngle);
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyEndAngle, dEndAngle);
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyInnerRadius, dInnerRadius);
					if (dInnerRadius < 1)
					{
						dInnerRadius = 1;
					}
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyOuterRadius, dOuterRadius);
					if (dOuterRadius < 1)
					{
						dOuterRadius = 1;
					}
				}

				if (S_OK == l_hrOk)
				{
					if (dEndAngle - dStartAngle < 360.0)
					{
						m_figure.m_eShape = SvDef::SVExtentShapePie;
					}
					else if (dEndAngle - dStartAngle < 720.0)
					{
						m_figure.m_eShape = SvDef::SVExtentShapeCircle;
					}
					else
					{
						m_figure.m_eShape = SvDef::SVExtentShapeDoubleCircle;
					}

					m_figure.m_svTopLeft = centerPoint;
					m_figure.m_svTopCenter = centerPoint;
					m_figure.m_svTopRight = centerPoint;

					m_figure.m_svBottomRight = SVRotateAndFlipPoint(centerPoint, dOuterRadius, dStartAngle);
					m_figure.m_svBottomLeft = SVRotateAndFlipPoint(centerPoint, dOuterRadius, dEndAngle);
					m_figure.m_svCenterRight = SVRotateAndFlipPoint(centerPoint, dInnerRadius, dStartAngle);
					m_figure.m_svCenterLeft = SVRotateAndFlipPoint(centerPoint, dInnerRadius, dEndAngle);
				}

				break;
			}
			case SvDef::SVExtentTranslationVerticalPerspective:
			{
				double dWidth;
				double dHeight;
				double dYPosition;
				if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth) &&
					S_OK == GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight) &&
					S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetY, dYPosition))
				{
					m_figure.m_eShape = SvDef::SVExtentShapeHexagon;

					m_figure.m_svTopLeft = SVPoint<double> {0.0, dYPosition};
					m_figure.m_svTopRight = SVPoint<double> {dWidth - 1.0, dYPosition};

					m_figure.m_svBottomLeft = SVPoint<double> {0.0, dHeight - dYPosition - 1.0};
					m_figure.m_svBottomRight = SVPoint<double> {dWidth - 1.0, dHeight - dYPosition - 1.0};

					m_figure.m_svBottomCenter = SVPoint<double> {dWidth / 2.0, dHeight - 1.0};
					m_figure.m_svTopCenter = SVPoint<double> {dWidth / 2.0, 1.0};

					m_figure.m_svCenterRight = SVPoint<double> {dWidth - 1.0, dHeight / 2.0};
					m_figure.m_svCenterLeft = SVPoint<double> {0.0, dHeight / 2.0};
				}
				break;
			}
			case SvDef::SVExtentTranslationHorizontalPerspective:
			{
				double dWidth;
				double dHeight;
				double dXPosition;
				if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyWidth, dWidth) &&
					S_OK == GetExtentProperty(SvDef::SVExtentPropertyHeight, dHeight) &&
					S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffsetX, dXPosition))
				{
					m_figure.m_eShape = SvDef::SVExtentShapeHexagon;

					m_figure.m_svTopLeft = SVPoint<double> {dXPosition,  0.0};
					m_figure.m_svTopRight = SVPoint<double> {dWidth - dXPosition - 1.0, 0.0};

					m_figure.m_svBottomLeft = SVPoint<double> {dXPosition, dHeight - 1.0};
					m_figure.m_svBottomRight = SVPoint<double> {dWidth - dXPosition - 1.0, dHeight - 1.0};

					m_figure.m_svBottomCenter = SVPoint<double> {dWidth / 2, dHeight - 1.0};
					m_figure.m_svTopCenter = SVPoint<double> {dWidth / 2, 1.0};

					m_figure.m_svCenterRight = SVPoint<double> {dWidth - 1.0, dHeight / 2};
					m_figure.m_svCenterLeft = SVPoint<double> {0.0,        dHeight / 2};
				}
				break;
			}
			default:
			{
				l_hrOk = S_FALSE;

				break;
			}
		}

		if (S_OK != l_hrOk)
		{
			m_figure.Initialize();
		}
	}

	return l_hrOk;
}

HRESULT  SVImageExtentClass::UpdateVerticalPerspective(SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> position;

	double l_dY = rStart.m_y - rEnd.m_y;

	if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
	{
		switch (eLocation)
		{
			case SvDef::SVExtentLocationPropertyTop:
			case SvDef::SVExtentLocationPropertyTopRight:
			case SvDef::SVExtentLocationPropertyTopLeft:
			{
				position.m_y -= l_dY;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyBottom:
			case SvDef::SVExtentLocationPropertyBottomRight:
			case SvDef::SVExtentLocationPropertyBottomLeft:
			{
				position.m_y += l_dY;
				l_hrOk = S_OK;
				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
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

HRESULT  SVImageExtentClass::UpdateHorizontalPerspective(SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd)
{
	HRESULT l_hrOk = S_FALSE;

	SVPoint<double> position;

	double l_dX = rStart.m_x - rEnd.m_x;

	if (S_OK == GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
	{
		switch (eLocation)
		{
			case SvDef::SVExtentLocationPropertyBottomLeft:
			case SvDef::SVExtentLocationPropertyTopLeft:
			case SvDef::SVExtentLocationPropertyLeft:
			{
				position.m_x -= l_dX;
				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentLocationPropertyRight:
			case SvDef::SVExtentLocationPropertyBottomRight:
			case SvDef::SVExtentLocationPropertyTopRight:
			{
				position.m_x += l_dX;
				l_hrOk = S_OK;
				break;
			}
		}

		if (S_OK == l_hrOk)
		{
			if (S_OK == SetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position))
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


HRESULT SVImageExtentClass::GetTitlePoint(POINT &rTitlePoint) const
{
	HRESULT l_hrOk = S_OK;
	SVPoint<double> point {rTitlePoint};

	l_hrOk = GetTitlePoint(point);

	if (S_OK == l_hrOk)
	{
		rTitlePoint = static_cast<POINT> (point);
	}
	return l_hrOk;
}

HRESULT SVImageExtentClass::GetTitlePoint(SVPoint<double>& rTitlePoint) const
{
	HRESULT l_hrOk = S_OK;

	SVPoint<double> position;

	switch (m_eTranslation)
	{
		// Place the Title above the top of the ROI rectangle
		case SvDef::SVExtentTranslationResize:
		{
			//@WARNING [Jim][8 July 2015] No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = (position.m_x + 5);
				rTitlePoint.m_y = (position.m_y - 10);
			}
			break;
		}
		// These cases place the Title below the top of the ROI rectangle
		case SvDef::SVExtentTranslationDoubleHeight:
		case SvDef::SVExtentTranslationFlipHorizontal:
		case SvDef::SVExtentTranslationFlipVertical:
		case SvDef::SVExtentTranslationFigureShift:
		{
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = position.m_x + 5;
				rTitlePoint.m_y = position.m_y + 5;
			}
			break;
		}

		case SvDef::SVExtentTranslationHorizontalPerspective:
		{
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = position.m_x + 5;
				rTitlePoint.m_y = position.m_y + 5;
			}

			break;
		}
		case SvDef::SVExtentTranslationVerticalPerspective:
		{
			l_hrOk = GetExtentProperty(SvDef::SVExtentPropertyTranslationOffset, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = position.m_x + 5;
				rTitlePoint.m_y = position.m_y + 5;
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
			rTitlePoint.m_x = 0.0;
			rTitlePoint.m_y = -10.0;

			l_hrOk = TranslateFromOutputSpace(rTitlePoint, rTitlePoint);

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

HRESULT SVImageExtentClass::setProperty(SvDef::SVExtentPropertyEnum eProperty, double dValue)
{
	HRESULT result {E_INVALIDARG};

	switch (eProperty)
	{
		case SvDef::SVExtentPropertyPositionPointX:
		case SvDef::SVExtentPropertyPositionPointY:
		case SvDef::SVExtentPropertyPositionPointEndOfLineX:
		case SvDef::SVExtentPropertyPositionPointEndOfLineY:
		case SvDef::SVExtentPropertyTranslationOffsetX:
		case SvDef::SVExtentPropertyTranslationOffsetY:
		case SvDef::SVExtentPropertyOutputPositionPointX:
		case SvDef::SVExtentPropertyOutputPositionPointY:
		case SvDef::SVExtentPropertyOutputWidth:
		case SvDef::SVExtentPropertyOutputHeight:
		{
			//Remove mantissa for the selected values
			::modf(dValue, &dValue);
			break;
		}
		default:
		{
			break;
		}
	}

	SVExtentPropertyValueMap::iterator iter {m_extentValues.find(eProperty)};
	if (m_extentValues.end() != iter)
	{
		iter->second = dValue;
		m_properties |= eProperty;
		result = S_OK;
	}
	else
	{
		assert(false);
	}
	return result;
}

bool SVImageExtentClass::isEnabled(SvDef::SVExtentPropertyEnum eProperty) const
{
	return ((m_properties & eProperty) == eProperty) && (eProperty != SvDef::SVExtentPropertyNone);
}