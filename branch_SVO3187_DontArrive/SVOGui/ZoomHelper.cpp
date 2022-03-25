//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ZoomHelper
//* .File Name       : $Workfile:   ZoomHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   13 Aug 2014 17:19:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "ZoomHelper.h"
#pragma endregion Includes

constexpr double cDefault_Min = 0.0625;
constexpr double cDefault_Max = 16.0;
constexpr double cZoomStepLarge = 1.0;
constexpr double cZoomStepSmall = 0.1;

int ZoomHelper::m_InstanceCount{0};
double ZoomHelper::m_ScaleFactor[ZoomHelper::m_cScaleCount] = {0};
HFONT ZoomHelper::m_hFont[ZoomHelper::m_cScaleCount] = {nullptr};

#pragma region Constructor
ZoomHelper::ZoomHelper() :
	m_Zoom_Min(cDefault_Min)
	, m_Zoom_Max(cDefault_Max)
{
	m_ScaleIndex = m_cScaleCount / 2;
	if (m_InstanceCount == 0)
	{
		Init();
	}
	m_InstanceCount++;
}

ZoomHelper::~ZoomHelper()
{
	m_InstanceCount--;
	Exit();
}

void ZoomHelper::Init()
{
	if (m_InstanceCount != 0)
	{
		return;
	}
	for (int i = m_cScaleCount - 1; 0 <= i; i--)
	{
		// map the first 14 fonts (0 - 14) to the default since they're too small
		// to read anyway.
		if (i < static_cast<int>(m_cScaleCount / 2))
		{
			m_ScaleFactor[i] = 1.0 / (double)((m_cScaleCount / 2 + 1) - i);

			m_hFont[i] = m_hFont[m_cScaleCount / 2];
		}
		else
		{
			m_ScaleFactor[i] = (double)(i - (m_cScaleCount / 2 - 1));

			m_hFont[i] = CreateFont(static_cast<int>(m_ScaleFactor[i] * 8), // logical height of font
									static_cast<long>(m_ScaleFactor[i] * 6), // logical average character width
									0, // angle of escapement
									0, // base-line orientation angle
									0, // font weight
									FALSE, // italic attribute flag
									FALSE, // underline attribute flag
									FALSE, // strikeout attribute flag
									DEFAULT_CHARSET, // character set identifier
									OUT_DEFAULT_PRECIS, // output precision
									CLIP_DEFAULT_PRECIS, // clipping precision
									DEFAULT_QUALITY, // output quality
									DEFAULT_PITCH | FF_DONTCARE, // pitch and family
									nullptr); // pointer to typeface name string
		}
	}
}

void ZoomHelper::Exit()
{
	if (0 != m_InstanceCount)
	{
		return;
	}

	for (unsigned int i = 0; i < m_cScaleCount; i++)
	{
		if (m_cScaleCount / 2 <= i)
		{
			DeleteObject(m_hFont[i]);
		}

		m_hFont[i] = nullptr;
	}
}
#pragma endregion Constructor

#pragma region Public Methods
double ZoomHelper::GetZoom() const
{
	double Result{1.0};

	if (m_Zoom > 0)
	{
		Result = m_Zoom;
	}
	return Result;
}

bool ZoomHelper::SetZoomType(ZoomEnum ZoomType, unsigned int ZoomIndex /*=0*/)
{
	constexpr unsigned int cZoomSmallestIndex = 0;
	constexpr unsigned int cZoomSmallIndex = 12;
	constexpr unsigned int cZoomNormalIndex = 15;
	constexpr unsigned int cZoomLargeIndex = 22;
	constexpr unsigned int cZoomLargestIndex = 30;

	bool Result{false};
	double originalZoom = m_Zoom;

	//First clear the flags then set accordingly
	m_bIsOne = false;
	m_bIsFit = false;

	switch (ZoomType)
	{
		case ZoomEnum::ZoomSmallest:
			m_ScaleIndex = cZoomSmallestIndex;
			m_Zoom = m_ScaleFactor[m_ScaleIndex];
			break;

		case ZoomEnum::ZoomSmall:
			m_ScaleIndex = cZoomSmallIndex;
			m_Zoom = m_ScaleFactor[m_ScaleIndex];
			break;

		case ZoomEnum::ZoomNormal:
			m_ScaleIndex = cZoomNormalIndex;
			m_Zoom = m_ScaleFactor[m_ScaleIndex];
			m_bIsOne = true;
			break;

		case ZoomEnum::ZoomLarge:
			m_ScaleIndex = cZoomLargeIndex;
			m_Zoom = m_ScaleFactor[m_ScaleIndex];
			break;

		case ZoomEnum::ZoomLargest:
			m_ScaleIndex = cZoomLargestIndex;
			m_Zoom = m_ScaleFactor[m_ScaleIndex];
			break;

		case ZoomEnum::ZoomPlus:
			m_Zoom += GetZoomStep();
			m_Zoom = std::min(m_Zoom_Max, m_Zoom);
			break;

		case ZoomEnum::ZoomMinus:
			m_Zoom -= GetZoomStep();
			m_Zoom = std::max(m_Zoom, m_Zoom_Min);
			break;

		case ZoomEnum::ZoomFitAll:
		case ZoomEnum::ZoomFitWidth:
		case ZoomEnum::ZoomFitHeight:
			//Before this is called CalculateZoomFit must be called which sets m_ZoomFit
			m_Zoom = m_ZoomFit;
			m_bIsFit = true;
			break;

		case ZoomEnum::ZoomValue:
			m_ScaleIndex = std::min(cZoomLargestIndex, ZoomIndex);
			m_Zoom = m_ScaleFactor[m_ScaleIndex];
			break;
		
		default:
			break;
	}
	Result = (m_Zoom != originalZoom) ? true : false;
	if(Result)
	{
		ZoomChanged();
	}
	return Result;
}

void ZoomHelper::SetZoom(double zoom)
{
	double originalZoom = m_Zoom;
	m_bIsOne = false;
	m_bIsFit = false;

	m_Zoom = std::max(std::min(zoom, m_Zoom_Max), m_Zoom_Min);

	if (originalZoom != m_Zoom)
	{
		ZoomChanged();
	}
}

void ZoomHelper::ExtendMinMax(double zoom)
{
	m_Zoom_Max = std::max(zoom, m_Zoom_Max);
	m_Zoom_Min = std::min(m_Zoom_Min, zoom);
}

bool ZoomHelper::CalculateZoomFit(ZoomEnum ZoomType, CSize ImageSize, CSize ViewSize)
{
	if (ImageSize.cx < 1 || ImageSize.cy < 1 || ViewSize.cx < 1 || ViewSize.cy < 1)
	{
		assert(false);
		m_ZoomFit = 1.0;
		return false;
	}
	double ScrollWidth = static_cast<double> (::GetSystemMetrics(SM_CXVSCROLL) + 1);
	double ScrollHeight = static_cast<double> (::GetSystemMetrics(SM_CYHSCROLL) + 1);

	double Width = static_cast<double> (ViewSize.cx);
	double Height = static_cast<double> (ViewSize.cy);
	double ImageWidth = static_cast<double> (ImageSize.cx);
	double ImageHeight = static_cast<double> (ImageSize.cy);

	switch (ZoomType)
	{
		case ZoomFitWidth:
			//If width is bigger then we need to consider the scroll bar 
			if (Width / ImageWidth > Height / ImageHeight)
			{
				Width -= ScrollWidth;
			}
			m_ZoomFit = Width / ImageWidth;
			break;

		case ZoomFitHeight:
			//If height is bigger then we need to consider the scroll bar 
			if (Width / ImageWidth < Height / ImageHeight)
			{
				Height -= ScrollHeight;
			}
			m_ZoomFit = Height / ImageHeight;
			break;

		case ZoomFitAll:
			if (Width / ImageWidth > Height / ImageHeight)
			{
				m_ZoomFit = Height / ImageHeight;
			}
			else
			{
				m_ZoomFit = Width / ImageWidth;
			}
			break;

		default:
			break;
	}

	m_Zoom_Max = std::max(m_ZoomFit, cDefault_Max);
	m_Zoom_Min = std::min(m_ZoomFit, cDefault_Min);

	return true;
}

void ZoomHelper::CloneSettings(const ZoomHelper& Source)
{
	double originalZoom = m_Zoom;
	m_Zoom = Source.m_Zoom;
	m_bIsOne = Source.m_bIsOne;
	if (originalZoom != m_Zoom)
	{
		ZoomChanged();
	}
}

void ZoomHelper::Clear()
{
	m_Zoom = 1.0;
	m_Zoom_Min = cDefault_Min;
	m_Zoom_Max = cDefault_Max;
	m_ZoomFit = 1.0;
	m_bIsOne = false;
	m_bIsFit = false;

	ZoomChanged();
}

double ZoomHelper::GetZoomStep() const
{
	double step = cZoomStepSmall;
	if (m_Zoom >= 2.0 * cZoomStepLarge)
	{
		step = cZoomStepLarge;
	}

	return step;
}
#pragma endregion Public Methods

#pragma region Private Methods
unsigned ZoomHelper::FindScaleIndex()
{
	m_ScaleIndex = m_cScaleCount - 1;
	for (unsigned int i = 0; i < m_cScaleCount; i++)
	{
		if (m_Zoom <= m_ScaleFactor[i])
		{
			m_ScaleIndex = i;
			break;
		}
	}

	return m_ScaleIndex;
}

void ZoomHelper::ZoomChanged()
{
	FindScaleIndex();
}
#pragma endregion Private Methods

