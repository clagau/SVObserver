//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageExtentClass
//* .File Name       : $Workfile:   SVImageExtentClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   24 Sep 2013 15:47:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <cmath>
//Moved to precompiled header: #include <map>
#include "SVImageExtentClass.h"
#include "SVExtentOffsetStruct.h"
#include "Definitions/SVExtentEnums.h"
#include "SVFileSystemLibrary\SVFileNameClass.h"
#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVUtilityLibrary/GeoHelper.h"
#pragma endregion Includes


//These are the property extents which have values to be saved in the map
constexpr SvPb::SVExtentPropertyEnum cStoredExtentValues[]
{
	SvPb::SVExtentPropertyWidth,
	SvPb::SVExtentPropertyHeight,
	SvPb::SVExtentPropertyStartAngle,
	SvPb::SVExtentPropertyEndAngle,
	SvPb::SVExtentPropertyInnerRadius,
	SvPb::SVExtentPropertyOuterRadius,
	SvPb::SVExtentPropertyWidthFactorContent,
	SvPb::SVExtentPropertyWidthFactorFormat,
	SvPb::SVExtentPropertyHeightFactorContent,
	SvPb::SVExtentPropertyHeightFactorFormat,
	SvPb::SVExtentPropertyOutputWidth,
	SvPb::SVExtentPropertyOutputHeight,
	SvPb::SVExtentPropertyRotationAngle,
	SvPb::SVExtentPropertyPositionPointX,
	SvPb::SVExtentPropertyPositionPointY,
	SvPb::SVExtentPropertyPositionPointEndOfLineX,
	SvPb::SVExtentPropertyPositionPointEndOfLineY,
	SvPb::SVExtentPropertyTranslationOffsetX,
	SvPb::SVExtentPropertyTranslationOffsetY,
	SvPb::SVExtentPropertyOutputPositionPointX,
	SvPb::SVExtentPropertyOutputPositionPointY,
};

static const SVExtentPropertyStringMap cExtentPropertyNames =
{
	{SvPb::SVExtentPropertyPositionPointX, std::string("X Position")},
	{SvPb::SVExtentPropertyPositionPointY, std::string("Y Position")},
	{SvPb::SVExtentPropertyTranslationOffsetX, std::string("X Offset")},
	{SvPb::SVExtentPropertyTranslationOffsetY, std::string("Y Offset")},
	{SvPb::SVExtentPropertyTranslationOffsetY, std::string("Y Offset")},
	{SvPb::SVExtentPropertyTranslationOffsetY, std::string("Y Offset")},
	{SvPb::SVExtentPropertyRotationAngle, std::string("Rotation Angle")},
	{SvPb::SVExtentPropertyOutputPositionPointX, std::string("Output X Position")},
	{SvPb::SVExtentPropertyOutputPositionPointY, std::string("Output Y Position")},
	{SvPb::SVExtentPropertyWidth, std::string("Width")},
	{SvPb::SVExtentPropertyHeight, std::string("Height")},
	{SvPb::SVExtentPropertyStartAngle, std::string("Start Angle")},
	{SvPb::SVExtentPropertyEndAngle, std::string("End Angle")},
	{SvPb::SVExtentPropertyInnerRadius, std::string("Inner Radius")},
	{SvPb::SVExtentPropertyOuterRadius, std::string("Outer Radius")},
	{SvPb::SVExtentPropertyOutputWidth, std::string("Width")},
	{SvPb::SVExtentPropertyOutputHeight, std::string("Height")},
};

///  useful for debugging
static const SVExtentPropertyStringMap cExtentPropertyShortNames =
{
	{SvPb::SVExtentPropertyPositionPointX, std::string("X")},
	{SvPb::SVExtentPropertyPositionPointY, std::string("Y")},
	{SvPb::SVExtentPropertyPositionPointEndOfLineX, std::string("eolX")},
	{SvPb::SVExtentPropertyPositionPointEndOfLineY, std::string("eolY")},
	{SvPb::SVExtentPropertyTranslationOffsetX, std::string("dX")},
	{SvPb::SVExtentPropertyTranslationOffsetY, std::string("dY")},
	{SvPb::SVExtentPropertyRotationAngle, std::string("rot")},
	{SvPb::SVExtentPropertyOutputPositionPointX, std::string("xOut")},
	{SvPb::SVExtentPropertyOutputPositionPointY, std::string("yOut")},
	{SvPb::SVExtentPropertyWidth, std::string("w")},
	{SvPb::SVExtentPropertyHeight, std::string("h")},
	{SvPb::SVExtentPropertyStartAngle, std::string("start")},
	{SvPb::SVExtentPropertyEndAngle, std::string("end")},
	{SvPb::SVExtentPropertyInnerRadius, std::string("inner")},
	{SvPb::SVExtentPropertyOuterRadius, std::string("outer")},
	{SvPb::SVExtentPropertyOutputWidth, std::string("wOut")},
	{SvPb::SVExtentPropertyOutputHeight, std::string("hOut")},
	{SvPb::SVExtentPropertyWidthFactorContent, std::string("w*")},
	{SvPb::SVExtentPropertyHeightFactorContent, std::string("h*")},
	{SvPb::SVExtentPropertyWidthFactorFormat, std::string("wS*")},
	{SvPb::SVExtentPropertyHeightFactorFormat, std::string("hS*")},
};

///contains properties of a subset of cExtentPropertyShortNames. Useful if only some extent properties are of interest and to be logged
static const SVExtentPropertyStringMap cExtentPropertyShortNamesAbridged =
{
	{SvPb::SVExtentPropertyPositionPointX, std::string("X")},
	{SvPb::SVExtentPropertyPositionPointY, std::string("Y")},
	{SvPb::SVExtentPropertyWidth, std::string("w")},
	{SvPb::SVExtentPropertyHeight, std::string("h")},
	{SvPb::SVExtentPropertyOutputWidth, std::string("wOut")},
	{SvPb::SVExtentPropertyOutputHeight, std::string("hOut")},
	{SvPb::SVExtentPropertyWidthFactorContent, std::string("wC")},
	{SvPb::SVExtentPropertyHeightFactorContent, std::string("hC*")},
	{SvPb::SVExtentPropertyWidthFactorFormat, std::string("wF*")},
	{SvPb::SVExtentPropertyHeightFactorFormat, std::string("hF*")},
};

static const SVExtentPropertyPointMap cExtentPropertyPointMap =
{
	{SvPb::SVExtentPropertyPositionPoint, {SvPb::SVExtentPropertyPositionPointX, SvPb::SVExtentPropertyPositionPointY}},
	{SvPb::SVExtentPropertyPositionPointEndOfLine, {SvPb::SVExtentPropertyPositionPointEndOfLineX, SvPb::SVExtentPropertyPositionPointEndOfLineY}},
	{SvPb::SVExtentPropertyTranslationOffset, {SvPb::SVExtentPropertyTranslationOffsetX, SvPb::SVExtentPropertyTranslationOffsetY}},
	{SvPb::SVExtentPropertyOutputPositionPoint, {SvPb::SVExtentPropertyOutputPositionPointX, SvPb::SVExtentPropertyOutputPositionPointY}},
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVImageExtentClass::SVImageExtentClass()
{
	Initialize();
}

SVImageExtentClass &SVImageExtentClass::operator=(const SVImageExtentClass& rRhs)
{
	if (this != &rRhs)
	{
		m_eTranslation = rRhs.m_eTranslation;
		m_extentValues = rRhs.m_extentValues;
		m_figure = rRhs.m_figure;
		m_isUpdated = rRhs.m_isUpdated;
		m_useTransferMatrix = rRhs.m_useTransferMatrix;
		m_TransferAngle = rRhs.m_TransferAngle;
		m_properties = rRhs.m_properties;
		m_TransferMatrix = rRhs.m_TransferMatrix;
	}
	return *this;
}


void SVImageExtentClass::Initialize()
{
	m_eTranslation = SvPb::SVExtentTranslationUnknown;
	m_properties = 0UL;

	for (auto extentProperty : cStoredExtentValues)
	{
		m_extentValues[extentProperty] = 0.0;
	}
	m_isUpdated = false;
	m_useTransferMatrix = false;
}

SvPb::SVExtentTranslationEnum SVImageExtentClass::GetTranslation() const
{
	
	return m_eTranslation;
}

void SVImageExtentClass::SetTranslation(SvPb::SVExtentTranslationEnum eTranslation)
{
	m_eTranslation = eTranslation;

	ClearOutputData();
}

void SVImageExtentClass::DisableExtentProperty(SvPb::SVExtentPropertyEnum eProperty)
{
	m_properties &= static_cast<unsigned long> (~eProperty);
}

HRESULT SVImageExtentClass::GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, long &rValue) const
{
	double dValue {0.0};

	HRESULT result = GetExtentProperty(eProperty, dValue);

	if (S_OK == result)
	{
		rValue = static_cast<long> (dValue);
	}

	return result;
}

HRESULT SVImageExtentClass::GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double &rValue) const
{
	SVExtentPropertyValueMap::const_iterator iter {m_extentValues.find(eProperty)};
	if (m_extentValues.end() != iter)
	{
		rValue = iter->second;
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT SVImageExtentClass::GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, POINT &rValue) const
{
	SVPoint<double> point;

	HRESULT result = GetExtentProperty(eProperty, point);

	if (S_OK == result)
	{
		rValue = static_cast<POINT> (point);
	}

	return result;
}

HRESULT SVImageExtentClass::GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, SVPoint<double>& rValue) const
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

HRESULT SVImageExtentClass::SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue, bool clearOutputData /*= true*/)
{
	HRESULT result {S_OK};

	// special case to handle polar unwrap
	if (SvPb::SVExtentTranslationPolarUnwrap == m_eTranslation)
	{
		if (SvPb::SVExtentPropertyRotationAngle == (eProperty & SvPb::SVExtentPropertyRotationAngle))
		{
			double dEndAngle = 0.0;
			double dStartAngle = 0.0;
			result = GetExtentProperty(SvPb::SVExtentPropertyEndAngle, dEndAngle);
			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle);
			}
			if (S_OK == result)
			{
				result = setProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle + (dValue - dEndAngle));
			}
			if (S_OK == result)
			{
				result = setProperty(SvPb::SVExtentPropertyEndAngle, dValue);
			}
		}
		else if (SvPb::SVExtentPropertyEndAngle == (eProperty & SvPb::SVExtentPropertyEndAngle))
		{
			result = setProperty(SvPb::SVExtentPropertyRotationAngle, dValue);
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

HRESULT SVImageExtentClass::SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue, bool clearOutputData /*= true*/)
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

HRESULT SVImageExtentClass::UpdateDataRecalculateOutput()
{
	HRESULT l_hrOk = BuildOutputDimensions();

	if (S_OK == l_hrOk)
	{
		l_hrOk = BuildFigure();
	}
	m_isUpdated = (l_hrOk == S_OK);
	return l_hrOk;
}

bool SVImageExtentClass::hasFigure() const
{
	return (SvDef::SVExtentShapeUnknown != m_figure.m_eShape);
}


HRESULT SVImageExtentClass::GetRectangle(RECT &p_roRect) const
{
	RECT l_oRect;

	l_oRect.top = 0;
	l_oRect.left = 0;
	l_oRect.right = 0;
	l_oRect.bottom = 0;

	HRESULT result = GetExtentProperty(SvPb::SVExtentPropertyPositionPointX, l_oRect.left);

	if (S_OK == result)
	{
		result = GetExtentProperty(SvPb::SVExtentPropertyPositionPointY, l_oRect.top);
	}

	if (S_OK == result)
	{
		result = GetExtentProperty(SvPb::SVExtentPropertyWidth, l_oRect.right);

		l_oRect.right += l_oRect.left;
	}

	if (S_OK == result)
	{
		result = GetExtentProperty(SvPb::SVExtentPropertyHeight, l_oRect.bottom);

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

	HRESULT result = GetExtentProperty(SvPb::SVExtentPropertyWidth, l_oRect.right);

	if (S_OK == result)
	{
		result = GetExtentProperty(SvPb::SVExtentPropertyHeight, l_oRect.bottom);
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

	HRESULT result = GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, l_oRect.right);

	if (S_OK == result)
	{
		result = GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, l_oRect.bottom);
	}

	if (S_OK == result)
	{
		p_roRect = l_oRect;
	}

	return result;
}



HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVPoint<double> value, SVPoint<double>& rResult) const
{
	HRESULT l_hrOk {S_OK};

	rResult.clear();
	if (m_useTransferMatrix)
	{
		rResult.m_x = value.m_x * m_TransferMatrix[0][0] + value.m_y * m_TransferMatrix[0][1] + m_TransferMatrix[0][2];
		rResult.m_y = value.m_x * m_TransferMatrix[1][0] + value.m_y * m_TransferMatrix[1][1] + m_TransferMatrix[1][2];
		return S_OK;
	}

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		{
			rResult = value;

			l_hrOk = S_OK;

			break;
		}

		case SvPb::SVExtentTranslationFigureShift:
		case SvPb::SVExtentTranslationProfileShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rResult = value + position;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> displacement;

				GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);

				rResult = value + position + displacement;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationTransformShift:
		{
			SVPoint<double> position;
			SVPoint<double> displacement;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				rResult = value + position + displacement;
			}

			break;
		}
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationLinear:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SvUl::SVRotatePoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += rotation.m_x;
				rResult.m_y += rotation.m_y;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;
			SVPoint<double> displacement;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SvUl::SVRotatePoint(outputRotation, value, dAngle);
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
		case SvPb::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;
			SVPoint<double> outputRotation;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				double dAngle = SvUl::SVGetRotationAngle(startPoint, endPoint);

				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SvUl::SVRotatePoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += startPoint.m_x;
				rResult.m_y += startPoint.m_y;
			}

			break;
		}// end case SVExtentTranslationLine:

		case SvPb::SVExtentTranslationFlippedRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				if (dAngle == 0.0)
				{
					rResult = value;
				}
				else
				{
					rResult = SvUl::SVRotateAndFlipPoint(outputRotation, value, dAngle);
				}

				rResult.m_x -= outputRotation.m_x;
				rResult.m_y -= outputRotation.m_y;

				rResult.m_x += rotation.m_x;
				rResult.m_y += rotation.m_y;
			}

			break;
		}// end case SVExtentTranslationFlippedRotate:

		case SvPb::SVExtentTranslationPolarUnwrap:
		{
			SVPoint<double> centerPoint;

			double dStartAngle = 0.0;
			double dOuterRadius = 0.0;
			double dInnerRadius = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, centerPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, dOuterRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, dInnerRadius);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle);
			}

			if (S_OK == l_hrOk)
			{
				double l_dRadius;
				double l_dAngle;
				if (dOuterRadius > dInnerRadius)
				{
					l_dRadius = value.m_y + dInnerRadius;
					l_dAngle = SvUl::Radians2Degrees(value.m_x / dOuterRadius) + dStartAngle;
				}
				else
				{
					l_dRadius = dInnerRadius - value.m_y;
					l_dAngle = SvUl::Radians2Degrees(value.m_x / dInnerRadius) + dStartAngle;
				}

				rResult = SvUl::SVRotateAndFlipPoint(centerPoint, l_dRadius, l_dAngle);
			}

			break;
		}// end case SVExtentTranslationPolarUnwrap:

		case SvPb::SVExtentTranslationCylindricalWarpH:
		{
			double dInputWidth = 0.0;
			double dWarpAngle = 0.0;
			double dOutputWidth = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dInputWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dOutputWidth);
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
		}// end case SVExtentTranslationCylindricalWarpH:

		case SvPb::SVExtentTranslationCylindricalWarpV:
		{
			double dInputHeight = 0.0;
			double dWarpAngle = 0.0;
			double dOutputHeight = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dInputHeight);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dOutputHeight);
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
		}// end case SVExtentTranslationCylindricalWarpV:
		case SvPb::SVExtentTranslationVerticalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dyOffset = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetY, dyOffset);
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
		}// end case SVExtentTranslationVerticalPerspective:

		case SvPb::SVExtentTranslationHorizontalPerspective:
		{
			double dFullHeight = 0.0;
			double dFullWidth = 0.0;
			double dxOffset = 0.0;
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dFullWidth);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetX, dxOffset);
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
		}// end case SVExtentTranslationHorizontalPerspective:

		//new cases for Flip Horizontal & Vertical
		case SvPb::SVExtentTranslationFlipVertical:
		{
			SVPoint<double> position;
			double dFullHeight = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dFullHeight);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_y = (dFullHeight - 1) - value.m_y;
				rResult.m_x = value.m_x;
			}
			break;
		}
		case SvPb::SVExtentTranslationFlipHorizontal:
		{
			SVPoint<double> position;
			double dFullWidth = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dFullWidth);
			}
			if (S_OK == l_hrOk)
			{
				rResult.m_x = (dFullWidth - 1) - value.m_x;
				rResult.m_y = value.m_y;
			}
			break;
		}
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		{
			//@WARNING [Jim][8 July 2015] No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.

			SVPoint<double> position;

			double heightScaleFactor = 1.0;
			double widthScaleFactor = 1.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);
			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeightFactorContent, heightScaleFactor);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidthFactorContent, widthScaleFactor);
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

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVExtentFigureStruct value, SVExtentFigureStruct& rResult) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		{
			rResult = value;

			l_hrOk = S_OK;

			break;
		}
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationFlippedRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationTransformShift:
		case SvPb::SVExtentTranslationTransformRotate:
		case SvPb::SVExtentTranslationPolarUnwrap:
		case SvPb::SVExtentTranslationLine:
		case SvPb::SVExtentTranslationLinear:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationVerticalPerspective:
		case SvPb::SVExtentTranslationHorizontalPerspective:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFigureShift:
		{
			switch (value.m_eShape)
			{
				case SvDef::SVExtentShapeArrow:
				{
					l_hrOk = S_OK;

					if (S_OK != TranslateFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft, -90.0, value.m_svTopLeft, rResult.m_svTopLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft, 0.0, value.m_svTopCenter, rResult.m_svTopCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft, 0.0, value.m_svCenterRight, rResult.m_svCenterRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft, 0.0, value.m_svBottomCenter, rResult.m_svBottomCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft, 90.0, value.m_svBottomLeft, rResult.m_svBottomLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterRight, rResult.m_svCenterRight, -135.0, value.m_svTopRight, rResult.m_svTopRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svCenterRight, rResult.m_svCenterRight, 135.0, value.m_svBottomRight, rResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SvDef::SVExtentShapeRectangle:
				{
					l_hrOk = S_OK;

					if (S_OK != TranslateFromOutputSpace(value.m_svTopLeft, rResult.m_svTopLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svTopLeft, rResult.m_svTopLeft, 0.0, value.m_svTopCenter, rResult.m_svTopCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svTopLeft, rResult.m_svTopLeft, 0.0, value.m_svTopRight, rResult.m_svTopRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svTopLeft, rResult.m_svTopLeft, 90.0, value.m_svCenterLeft, rResult.m_svCenterLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svTopLeft, rResult.m_svTopLeft, 90.0, value.m_svBottomLeft, rResult.m_svBottomLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svTopRight, rResult.m_svTopRight, 90.0, value.m_svCenterRight, rResult.m_svCenterRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svTopRight, rResult.m_svTopRight, 90.0, value.m_svBottomRight, rResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svBottomLeft, rResult.m_svBottomLeft, 0.0, value.m_svBottomCenter, rResult.m_svBottomCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateLineFromOutputSpace(value.m_svBottomLeft, rResult.m_svBottomLeft, 0.0, value.m_svBottomRight, rResult.m_svBottomRight))
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

					GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotationPoint);

					SVPoint<double> radiusPoint = value.m_svCenterLeft;

					radiusPoint.m_y = outputRotationPoint.m_y;

					if (TranslateFromOutputSpace(radiusPoint, rotatePoint) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 0.0, value.m_svCenterLeft, rResult.m_svCenterLeft) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					rResult.m_svTopLeft = rResult.m_svCenterLeft;
					rResult.m_svBottomLeft = rResult.m_svCenterLeft;

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 0.0, value.m_svCenterRight, rResult.m_svCenterRight) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					rResult.m_svTopRight = rResult.m_svCenterRight;
					rResult.m_svBottomRight = rResult.m_svCenterRight;

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 0.0, value.m_svTopCenter, rResult.m_svTopCenter) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					rResult.m_svBottomCenter = rResult.m_svTopCenter;

					break;
				}
				case SvDef::SVExtentShapeLineVertical:
				{
					l_hrOk = S_OK;

					SVPoint<double> rotatePoint;
					SVPoint<double> outputRotationPoint;

					GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotationPoint);

					SVPoint<double> radiusPoint = value.m_svTopCenter;
					radiusPoint.m_y = outputRotationPoint.m_y;

					if (TranslateFromOutputSpace(radiusPoint, rotatePoint) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					double l_dNewAngle = 90.0;

					if (value.m_svTopCenter.m_y < radiusPoint.m_y)
					{
						l_dNewAngle = -90.0;
					}

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, l_dNewAngle, value.m_svTopCenter, rResult.m_svTopCenter) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					rResult.m_svTopLeft = rResult.m_svTopCenter;
					rResult.m_svTopRight = rResult.m_svTopCenter;

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 90.0, value.m_svBottomCenter, rResult.m_svBottomCenter) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					rResult.m_svBottomLeft = rResult.m_svBottomCenter;
					rResult.m_svBottomRight = rResult.m_svBottomCenter;

					if (TranslateLineFromOutputSpace(radiusPoint, rotatePoint, 90.0, value.m_svCenterLeft, rResult.m_svCenterLeft) != S_OK)
					{
						l_hrOk = S_FALSE;
					}

					rResult.m_svCenterRight = rResult.m_svCenterLeft;

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

					if (S_OK != TranslateFromOutputSpace(value.m_svBottomCenter, rResult.m_svBottomCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svBottomLeft, rResult.m_svBottomLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svBottomRight, rResult.m_svBottomRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svCenterLeft, rResult.m_svCenterLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svCenterRight, rResult.m_svCenterRight))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svTopCenter, rResult.m_svTopCenter))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svTopLeft, rResult.m_svTopLeft))
					{
						l_hrOk = S_FALSE;
					}

					if (S_OK != TranslateFromOutputSpace(value.m_svTopRight, rResult.m_svTopRight))
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

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVExtentLineStruct value, SVExtentLineStruct& rResult) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		{
			rResult = value;

			l_hrOk = S_OK;

			break;
		}
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationFlippedRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationTransformShift:
		case SvPb::SVExtentTranslationTransformRotate:
		case SvPb::SVExtentTranslationPolarUnwrap:
		case SvPb::SVExtentTranslationLine:
		case SvPb::SVExtentTranslationLinear:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFigureShift:
		{
			long l_lCount = static_cast<long> (value.m_PointVector.size());

			l_hrOk = S_OK;
			rResult.Initialize();

			if (0 < l_lCount)
			{
				double l_dAngle = 0.0;

				bool l_bIsAngle = S_OK == GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, l_dAngle);

				rResult.m_bIsAngleValid = value.m_bIsAngleValid;
				rResult.m_dAngle = value.m_dAngle;

				if (!l_bIsAngle || l_dAngle == 0.0 || !value.m_bIsAngleValid)
				{
					for (long i = 0; S_OK == l_hrOk && i < l_lCount; i++)
					{
						SVPoint<double> point = value.m_PointVector[i];

						if (S_OK == TranslateFromOutputSpace(point, point))
						{
							rResult.m_PointVector.push_back(point);
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

					GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotationPoint);

					SVPoint<double> l_svRadiusPoint = value.m_PointVector[0];

					long l_lStart = 0;

					if (value.m_dAngle == 0.0 && l_svRadiusPoint.m_x != outputRotationPoint.m_x)
					{
						l_svRadiusPoint.m_x = outputRotationPoint.m_x;
					}
					else if (value.m_dAngle == 90.0 && l_svRadiusPoint.m_y != outputRotationPoint.m_y)
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
							rResult.m_PointVector.push_back(rotatePoint);
						}
					}
					else
					{
						l_hrOk = S_FALSE;
					}

					for (long i = l_lStart; S_OK == l_hrOk && i < l_lCount; i++)
					{
						SVPoint<double> point = value.m_PointVector[i];

						double l_dNewAngle = value.m_dAngle;

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
							rResult.m_PointVector.push_back(point);
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
		rResult.m_dwColor = value.m_dwColor;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateFromOutputSpace(SVExtentMultiLineStruct value, SVExtentMultiLineStruct& rResult) const
{
	HRESULT l_hrOk = S_FALSE;

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		{
			rResult = value;

			l_hrOk = S_OK;

			break;
		}
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationFlippedRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationTransformShift:
		case SvPb::SVExtentTranslationTransformRotate:
		case SvPb::SVExtentTranslationPolarUnwrap:
		case SvPb::SVExtentTranslationLine:
		case SvPb::SVExtentTranslationLinear:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFigureShift:
		{
			long l_lCount = static_cast<long> (value.m_svLineArray.size());

			rResult.Initialize();

			l_hrOk = TranslateFromOutputSpace(value.m_StringPoint, rResult.m_StringPoint);

			for (long i = 0; S_OK == l_hrOk && i < l_lCount; i++)
			{
				SVExtentLineStruct svLine = value.m_svLineArray[i];

				if (S_OK == TranslateFromOutputSpace(svLine, svLine))
				{
					rResult.m_svLineArray.push_back(svLine);
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
		rResult.m_Color = value.m_Color;

		rResult.m_StringPoint = value.m_StringPoint;
		rResult.m_csString = value.m_csString;
	}

	return l_hrOk;
}

HRESULT SVImageExtentClass::TranslateLineFromOutputSpace(const SVPoint<double>& rRadiusPoint, const SVPoint<double>& rRotatePoint, double dLineAngle, SVPoint<double> value, SVPoint<double>& rResult) const
{
	HRESULT l_hrOk {S_OK};

	rResult.clear();

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationTransformShift:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationCylindricalWarpH:
		case SvPb::SVExtentTranslationCylindricalWarpV:
		case SvPb::SVExtentTranslationVerticalPerspective:
		case SvPb::SVExtentTranslationHorizontalPerspective:
		case SvPb::SVExtentTranslationPolarUnwrap:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFigureShift:
		{
			l_hrOk = TranslateFromOutputSpace(value, rResult);

			break;
		}// end case multiple

		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationTransformRotate:
		case SvPb::SVExtentTranslationLinear:
		{
			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);

			if (S_OK == l_hrOk)
			{
				double l_dRadius = SvUl::SVGetRadius(rRadiusPoint, value);

				rResult = SvUl::SVRotatePoint(rRotatePoint, l_dRadius, dAngle + dLineAngle);
			}

			break;
		}// end case multiple

		case SvPb::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == l_hrOk)
			{
				double l_dAngle = SvUl::SVGetRotationAngle(startPoint, endPoint);

				if (S_OK == l_hrOk)
				{
					double l_dRadius = SvUl::SVGetRadius(rRadiusPoint, value);

					rResult = SvUl::SVRotatePoint(rRotatePoint, l_dRadius, l_dAngle + dLineAngle);
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationLine:

		case SvPb::SVExtentTranslationFlippedRotate:
		{
			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);

			if (S_OK == l_hrOk)
			{
				double l_dRadius = SvUl::SVGetRadius(rRadiusPoint, value);

				rResult = SvUl::SVRotateAndFlipPoint(rRotatePoint, l_dRadius, dAngle - dLineAngle);
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

			if (rImageExtent.m_eTranslation == SvPb::SVExtentTranslationLine)
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
			else if (rImageExtent.m_eTranslation == SvPb::SVExtentTranslationProfile)
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
	
	if (m_useTransferMatrix)
	{
		rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle + m_TransferAngle), 360.0);
		return S_OK;
	}


	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationNone:
		{
			rOffsetData.m_bIsLinear &= true;

			l_hrOk = S_OK;

			break;
		}

		case SvPb::SVExtentTranslationShift:
		{
			SVPoint<double>  position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> displacement;

				GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);

				SVPoint<double> result = position + displacement - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationProfileShift:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = position - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationProfile:
		case SvPb::SVExtentTranslationLinear:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = rotation - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint = outputRotation;

				rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle + dAngle), 360.0);

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationTransformShift:
		{
			SVPoint<double> displacement;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = displacement - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint.clear();

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case multiple
		case SvPb::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;
			SVPoint<double> displacement;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, displacement);
			}

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = rotation + displacement - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint = outputRotation;

				rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle + dAngle), 360.0);

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}
		case SvPb::SVExtentTranslationFlippedRotate:
		{
			SVPoint<double> rotation;
			SVPoint<double> outputRotation;

			double dAngle = 0.0;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, rotation);

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);
			}

			if (S_OK == l_hrOk)
			{
				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, outputRotation);
			}

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = rotation - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

				rOffsetData.m_svRotationPoint = outputRotation;

				rOffsetData.m_dRotationAngle = ::fmod((rOffsetData.m_dRotationAngle - dAngle), 360.0);

				rOffsetData.m_bIsLinear &= true;
			}

			break;
		}// end case SVExtentTranslationFlippedRotate:
		// Warp Type Tools
		case SvPb::SVExtentTranslationVerticalPerspective:
		case SvPb::SVExtentTranslationHorizontalPerspective:
		case SvPb::SVExtentTranslationCylindricalWarpH:
		case SvPb::SVExtentTranslationCylindricalWarpV:
		case SvPb::SVExtentTranslationFigureShift:
		case SvPb::SVExtentTranslationPolarUnwrap:
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFlipHorizontal:
		{
			SVPoint<double> position;

			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				SVPoint<double> result = position - rOffsetData.m_svRotationPoint + rOffsetData.m_svOffset;

				rOffsetData.m_svOffset = SvUl::SVRotatePoint(rOffsetData.m_svOffset, result, rOffsetData.m_dRotationAngle);

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

		if (fileformatID != SVFileUnknown && 0 == _access(pFileName, 0))
		{

			std::string strFile(pFileName);

			Code = SVMatroxBufferInterface::GetImageSize(strFile, Width, Height);
		}

	}

	if (Code == S_OK)
	{
		SetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyWidth, Width);
		SetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyHeight, Height);
		SetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointX, 0);
		SetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointY, 0);
		SetTranslation(SvPb::SVExtentTranslationNone);
		UpdateDataRecalculateOutput();
		hrOk = S_OK;
	}
	return hrOk;
}

void SVImageExtentClass::getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const
{
	for (auto valuePair : m_extentValues)
	{
		auto* value = rExtentProperties.Add();
		if (nullptr != value)
		{
			value->set_type(valuePair.first);
			value->set_value(valuePair.second);
			auto iter = cExtentPropertyNames.find(valuePair.first);
			if (cExtentPropertyNames.end() != iter)
			{
				value->set_name(iter->second);
			}
			value->set_filteredoutflag(!isEnabled(valuePair.first));
		}
	}
}




void SVImageExtentClass::ClearOutputData()
{
	m_isUpdated = false;
	m_figure.Initialize();

	DisableExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint);
	DisableExtentProperty(SvPb::SVExtentPropertyDimentionsOutput);
	if (m_eTranslation == SvPb::SVExtentTranslationLine)
	{
		DisableExtentProperty(SvPb::SVExtentPropertyRotationAngle);
		DisableExtentProperty(SvPb::SVExtentPropertyWidth);
		DisableExtentProperty(SvPb::SVExtentPropertyHeight);
	}
	else
	{
		DisableExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine);
	}
}

HRESULT SVImageExtentClass::BuildOutputDimensions()
{
	HRESULT result {S_OK};
	ClearOutputData();

	switch (m_eTranslation)
	{
		case SvPb::SVExtentTranslationUnknown:
		{
			break;
		}

		case SvPb::SVExtentTranslationNone:
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationTransformShift:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFigureShift:
		{
			double dValue = 0.0;

			DisableExtentProperty(SvPb::SVExtentPropertyPie);
			result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dValue);

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end multiple case:

		case SvPb::SVExtentTranslationLine:
		{
			SVPoint<double> startPoint;
			SVPoint<double> endPoint;

			DisableExtentProperty(SvPb::SVExtentPropertyPie);
			result = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyPositionPointEndOfLine, endPoint);
			}

			if (S_OK == result)
			{
				double dWidth = SvUl::SVGetRadius(startPoint, endPoint);

				result = SetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth, false);

				if (S_OK == result)
				{
					result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dWidth, false);
				}
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyHeight, 1.0, false);

				if (S_OK == result)
				{
					result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, 1.0, false);
				}
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationLine:

		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationResize:
		{
			//@WARNING [Jim][8 July 2015] No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.

			double inputHeight {0.0};
			double inputWidth {0.0};
			double heightScaleFactor {1.0};
			double widthScaleFactor {1.0};

			DisableExtentProperty(SvPb::SVExtentPropertyPie);

			result = GetExtentProperty(SvPb::SVExtentPropertyWidth, inputWidth);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyWidthFactorFormat, widthScaleFactor);
			}

			if (S_OK == result)
			{
				double dValue = inputWidth * widthScaleFactor;
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, inputHeight);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeightFactorFormat, heightScaleFactor);
			}

			if (S_OK == result)
			{
				double dValue = inputHeight * heightScaleFactor;
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationDoubleHeight:

		case SvPb::SVExtentTranslationFlippedRotate:
		case SvPb::SVExtentTranslationRotate:
		{
			double dValue {0.0};

			DisableExtentProperty(SvPb::SVExtentPropertyPie);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationFlippedRotate: case SvDef::SVExtentTranslationRotate:

		case SvPb::SVExtentTranslationLinear:
		{
			double dHeight = 0.0;
			double dWidth = 0.0;
			double dAngle = 0.0;

			DisableExtentProperty(SvPb::SVExtentPropertyPie);

			result = GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dAngle);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dWidth, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dHeight, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationLinear:

		case SvPb::SVExtentTranslationProfile:
		{
			double dValue = 0.0;

			DisableExtentProperty(SvPb::SVExtentPropertyPie);

			result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dValue);

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				SVPoint<double> point;

				point.m_y += dValue / 2;

				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, point, false);
			}

			break;
		}// end case SvDef::SVExtentTranslationProfile:

		case SvPb::SVExtentTranslationPolarUnwrap:
		{
			double dInnerRadius = 0.0;
			double dOuterRadius = 0.0;
			double dStartAngle = 0.0;
			double dEndAngle = 0.0;

			DisableExtentProperty(SvPb::SVExtentPropertyRectangle);

			if (S_OK != GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle))
			{
				result = S_FALSE;
			}

			if (S_OK != GetExtentProperty(SvPb::SVExtentPropertyEndAngle, dEndAngle))
			{
				result = S_FALSE;
			}

			if (S_OK != GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, dInnerRadius))
			{
				result = S_FALSE;
			}

			if (S_OK != GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, dOuterRadius))
			{
				result = S_FALSE;
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}

			if (S_OK == result)
			{
				double dHeight = fabs(dOuterRadius - dInnerRadius);
				if (dHeight < 1.0)
				{
					dHeight = 1.0;
				}
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dHeight, false);

				if (S_OK == result)
				{
					double l_dMaxRadius = dOuterRadius > dInnerRadius ? dOuterRadius : dInnerRadius;
					double l_dWidth = SvUl::Degrees2Radians(dEndAngle - dStartAngle) * l_dMaxRadius;
					if (l_dWidth < 1.0)
					{
						l_dWidth = 1.0;
					}
					result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, l_dWidth, false);
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationPolarUnwrap:

		case SvPb::SVExtentTranslationTransformRotate:
		{
			SVPoint<double> position;

			double dHeight = 0.0;
			double dWidth = 0.0;

			DisableExtentProperty(SvPb::SVExtentPropertyPie);

			result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight);

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);
			}

			if (S_OK == result)
			{
				double dMax = std::sqrt((dWidth * dWidth) + (dHeight * dHeight));

				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dMax, false);

				if (S_OK == result)
				{
					result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dMax, false);
				}

				if (S_OK == result)
				{
					position.m_x += (dMax - dWidth) / 2;
					position.m_y += (dMax - dHeight) / 2;

					result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, position, false);
				}
			}

			break;
		}// end case SvDef::SVExtentTranslationTransformRotate:

		case SvPb::SVExtentTranslationCylindricalWarpH:
		{
			double dValue = 0.0;
			double dWarpAngle = 0.0;

			result = S_OK;

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dValue);
			}
			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == result)
			{
				double l_dRadius = (dValue / std::sin(dWarpAngle / 360.0 * M_PI)) / 2.0;
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, l_dRadius * M_PI * (dWarpAngle / 180.0), false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dValue);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dValue, false);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpH:

		case SvPb::SVExtentTranslationCylindricalWarpV:
		{
			double dValue = 0.0;
			double dWarpAngle = 0.0;

			result = S_OK;

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dWarpAngle);
			}
			if (S_OK == result)
			{
				double dRadius = (dValue / std::sin(dWarpAngle / 360.0 * M_PI)) / 2.0;
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dRadius * M_PI * (dWarpAngle / 180.0), false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dValue);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dValue, false);
			}
			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
			}
			break;
		}// end case SvDef::SVExtentTranslationCylindricalWarpV:

		case SvPb::SVExtentTranslationVerticalPerspective:
		case SvPb::SVExtentTranslationHorizontalPerspective:
		{
			double dValue = 0.0;

			DisableExtentProperty(SvPb::SVExtentPropertyPie);

			result = GetExtentProperty(SvPb::SVExtentPropertyWidth, dValue);

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, dValue, false);
			}

			if (S_OK == result)
			{
				result = GetExtentProperty(SvPb::SVExtentPropertyHeight, dValue);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, dValue, false);
			}

			if (S_OK == result)
			{
				result = SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>{0.0, 0.0}, false);
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
			case SvPb::SVExtentTranslationUnknown:
			case SvPb::SVExtentTranslationNone:
			case SvPb::SVExtentTranslationTransformShift:
			case SvPb::SVExtentTranslationTransformRotate:
			case SvPb::SVExtentTranslationCylindricalWarpH:
			case SvPb::SVExtentTranslationCylindricalWarpV:
			{
				// No figure
				break;
			}
			case SvPb::SVExtentTranslationShift:
			case SvPb::SVExtentTranslationRotate:
			case SvPb::SVExtentTranslationFlippedRotate:
			case SvPb::SVExtentTranslationProfile:
			case SvPb::SVExtentTranslationProfileShift:
			{
				double dWidth = 0.0;
				double dHeight = 0.0;

				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight);
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
			case SvPb::SVExtentTranslationDoubleHeight:
			case SvPb::SVExtentTranslationResize:
			case SvPb::SVExtentTranslationFlipHorizontal:
			case SvPb::SVExtentTranslationFlipVertical:
			case SvPb::SVExtentTranslationFigureShift:
			{
				double dWidth = 0.0;
				double dHeight = 0.0;

				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight);
				}

				SVPoint<double> startPoint;
				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, startPoint);
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

			case SvPb::SVExtentTranslationLinear:
			{
				double dWidth = 0.0;
				double dHeight = 0.0;

				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight);
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
			case SvPb::SVExtentTranslationLine:
			{
				double dWidth = 0.0;

				l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth);

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
			case SvPb::SVExtentTranslationPolarUnwrap:
			{
				SVPoint<double> centerPoint;

				double dStartAngle = 0.0;
				double dEndAngle = 0.0;
				double dInnerRadius = 0.0;
				double dOuterRadius = 0.0;

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, centerPoint);
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyStartAngle, dStartAngle);
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyEndAngle, dEndAngle);
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyInnerRadius, dInnerRadius);
					if (dInnerRadius < 1)
					{
						dInnerRadius = 1;
					}
				}

				if (S_OK == l_hrOk)
				{
					l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyOuterRadius, dOuterRadius);
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

					m_figure.m_svBottomRight = SvUl::SVRotateAndFlipPoint(centerPoint, dOuterRadius, dStartAngle);
					m_figure.m_svBottomLeft = SvUl::SVRotateAndFlipPoint(centerPoint, dOuterRadius, dEndAngle);
					m_figure.m_svCenterRight = SvUl::SVRotateAndFlipPoint(centerPoint, dInnerRadius, dStartAngle);
					m_figure.m_svCenterLeft = SvUl::SVRotateAndFlipPoint(centerPoint, dInnerRadius, dEndAngle);
				}

				break;
			}
			case SvPb::SVExtentTranslationVerticalPerspective:
			{
				double dWidth;
				double dHeight;
				double dYPosition;
				if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth) &&
					S_OK == GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight) &&
					S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetY, dYPosition))
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
			case SvPb::SVExtentTranslationHorizontalPerspective:
			{
				double dWidth;
				double dHeight;
				double dXPosition;
				if (S_OK == GetExtentProperty(SvPb::SVExtentPropertyWidth, dWidth) &&
					S_OK == GetExtentProperty(SvPb::SVExtentPropertyHeight, dHeight) &&
					S_OK == GetExtentProperty(SvPb::SVExtentPropertyTranslationOffsetX, dXPosition))
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
		case SvPb::SVExtentTranslationResize:
		{
			//@WARNING [Jim][8 July 2015] No identification of error cases.  
			// GetExtentProperty () only returns S_FALSE, which is not 
			// very helpful. Currently none of these cases give much help
			// with error identification.
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = (position.m_x + 5);
				rTitlePoint.m_y = (position.m_y - 10);
			}
			break;
		}
		// These cases place the Title below the top of the ROI rectangle
		case SvPb::SVExtentTranslationDoubleHeight:
		case SvPb::SVExtentTranslationFlipHorizontal:
		case SvPb::SVExtentTranslationFlipVertical:
		case SvPb::SVExtentTranslationFigureShift:
		{
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = position.m_x + 5;
				rTitlePoint.m_y = position.m_y + 5;
			}
			break;
		}

		case SvPb::SVExtentTranslationHorizontalPerspective:
		{
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = position.m_x + 5;
				rTitlePoint.m_y = position.m_y + 5;
			}

			break;
		}
		case SvPb::SVExtentTranslationVerticalPerspective:
		{
			l_hrOk = GetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, position);

			if (S_OK == l_hrOk)
			{
				rTitlePoint.m_x = position.m_x + 5;
				rTitlePoint.m_y = position.m_y + 5;
			}

			break;
		}
		// These cases place the Title above the top of the ROI rectangle
		case SvPb::SVExtentTranslationShift:
		case SvPb::SVExtentTranslationRotate:
		case SvPb::SVExtentTranslationFlippedRotate:
		case SvPb::SVExtentTranslationProfileShift:
		case SvPb::SVExtentTranslationTransformShift:
		case SvPb::SVExtentTranslationTransformRotate:
		case SvPb::SVExtentTranslationPolarUnwrap:
		case SvPb::SVExtentTranslationLine:
		case SvPb::SVExtentTranslationLinear:
		case SvPb::SVExtentTranslationProfile:
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

HRESULT SVImageExtentClass::setProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue)
{
	HRESULT result {E_INVALIDARG};

	switch (eProperty)
	{
		case SvPb::SVExtentPropertyPositionPointX:
		case SvPb::SVExtentPropertyPositionPointY:
		case SvPb::SVExtentPropertyPositionPointEndOfLineX:
		case SvPb::SVExtentPropertyPositionPointEndOfLineY:
		case SvPb::SVExtentPropertyTranslationOffsetX:
		case SvPb::SVExtentPropertyTranslationOffsetY:
		case SvPb::SVExtentPropertyOutputPositionPointX:
		case SvPb::SVExtentPropertyOutputPositionPointY:
		case SvPb::SVExtentPropertyOutputWidth:
		case SvPb::SVExtentPropertyOutputHeight:
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

bool SVImageExtentClass::isEnabled(SvPb::SVExtentPropertyEnum eProperty) const
{
	return ((m_properties & eProperty) == eProperty) && (eProperty != SvPb::SVExtentPropertyNone);
}


//Arvid: useful for debugging extents 
bool SVImageExtentClass::OutputDebugInformationOnExtent(const char* pDescription, const SVImageExtentClass* pReference, long deltaThreshold) const
{
	std::stringstream info;

	bool isBigDelta = false;

	info.flags(std::ios::fixed);
	info.precision(2);

	info << pDescription << ": ";
	if (nullptr != pReference)
	{
		info << "(delta) ";
	}

	auto& mapOfNames = cExtentPropertyShortNamesAbridged; //this defines which properties are to be logged here

	for (auto& propertyAndValue : m_extentValues)
	{
		auto enumAndName = mapOfNames.find(propertyAndValue.first);

		if (enumAndName != mapOfNames.end())
		{
			if (nullptr != pReference)
			{
				OutputDebugInformationOnExtent(pDescription); //If we want delta information we usually also need information on the reference

				auto referencePropertyAndValue = pReference->m_extentValues.find(propertyAndValue.first);
				if (referencePropertyAndValue != pReference->m_extentValues.end())
				{

					if (referencePropertyAndValue->second != propertyAndValue.second)
					{
//						info << enumAndName->second << "=" << referencePropertyAndValue->second << "->" << propertyAndValue.second;
						auto del = propertyAndValue.second - referencePropertyAndValue->second;
						info << enumAndName->second << ": " << del;
						if(deltaThreshold>0)
						{
							isBigDelta = (abs(del) > deltaThreshold);
						}
					}
				}
				else
				{
					info << enumAndName->second << "=" << "???->" << propertyAndValue.second;
				}
			}
			else
			{
				info << enumAndName->second << "=" << propertyAndValue.second;
			}
		}
		info << " ";
	}

	info << "\n";

	if (isBigDelta)
	{
		info << "\n\t\tBig Delta!\n\n";
	}


	OutputDebugString(info.str().c_str());

	return isBigDelta;
}
void SVImageExtentClass::setTransfermatrix(const std::vector<double>& rMatrix)
{
	if (rMatrix.size() < 7)
	{
		m_useTransferMatrix = false;
		return;
	}
	//{{1.0, 0.0, 0.0}, {0.0, 1.0,0.0}};
	m_TransferMatrix[0][0] = rMatrix[0];
	m_TransferMatrix[0][1] = rMatrix[1];
	m_TransferMatrix[0][2] = rMatrix[2];
	m_TransferMatrix[1][0] = rMatrix[3];
	m_TransferMatrix[1][1] = rMatrix[4];
	m_TransferMatrix[1][2] = rMatrix[5];
	m_TransferAngle = rMatrix[6];
	m_useTransferMatrix = true;
	return;

}

