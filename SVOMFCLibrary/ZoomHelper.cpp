//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
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
#include "ZoomHelper.h"
#include <algorithm>
#pragma endregion Includes

#undef max
#undef min

#pragma region Constructor
ZoomHelper::ZoomHelper()
: m_Default_Min(0.0625)
, m_Zoom_Min(0.0625)
, m_Default_Max(16.0)
, m_Zoom_Max(16.0)
, m_ZoomStepLarge(1.0)
, m_ZoomStepSmall(0.1)
, m_Zoom(1.0)
, m_ZoomFit(1.0)
, m_bIsOne(false)
, m_bIsFit(false)
{
}
#pragma endregion Constructor

#pragma region Public Methods
double ZoomHelper::GetZoom() const
{
	double retval = 1.0;

	if(m_Zoom > 0)
	{
		retval = m_Zoom;
	}
	return retval;
}

void ZoomHelper::SetZoom(double zoom)
{
	double originalZoom = m_Zoom;
	m_bIsOne = false;
	m_bIsFit = false;

	m_Zoom = std::max( std::min( zoom, m_Zoom_Max ), m_Zoom_Min );

	if(originalZoom != m_Zoom)
	{
		ZoomChanged();
	}
}

void ZoomHelper::ExtendMinMax(double zoom)
{
	m_Zoom_Max = std::max(zoom, m_Zoom_Max);
	m_Zoom_Min = std::min(m_Zoom_Min, zoom);
}

double ZoomHelper::SetToFit()
{
	double originalZoom = m_Zoom;
	m_Zoom = m_ZoomFit;
	m_bIsOne = false;
	m_bIsFit = true;

	if(originalZoom != m_Zoom)
	{
		ZoomChanged();
	}

	return m_Zoom;
}

void ZoomHelper::SetToOne()
{
	double originalZoom = m_Zoom;
	m_Zoom = 1.0;
	m_bIsOne = true;
	m_bIsFit = false;

	if(originalZoom != m_Zoom)
	{
		ZoomChanged();
	}
}

double ZoomHelper::ZoomPlus()
{
	double originalZoom = m_Zoom;
	m_bIsOne = false;
	m_bIsFit = false;

	m_Zoom += GetZoomStep();

	m_Zoom = std::min(m_Zoom_Max, m_Zoom);
	if(originalZoom != m_Zoom)
	{
		ZoomChanged();
	}


	return m_Zoom;
}

double ZoomHelper::ZoomMinus()
{
	double originalZoom = m_Zoom;
	m_bIsOne = false;
	m_bIsFit = false;

	m_Zoom -= GetZoomStep();

	m_Zoom = std::max(m_Zoom, m_Zoom_Min);
	if(originalZoom != m_Zoom)
	{
		ZoomChanged();
	}


	return m_Zoom;
}

bool ZoomHelper::CalculateZoomFit(CSize ImageSize, CSize ViewSize)
{
	if( ImageSize.cx < 1 ||
		ImageSize.cy < 1 ||
		ViewSize.cx < 1 ||
		ViewSize.cy < 1 )
	{
		ASSERT(FALSE);
		m_ZoomFit =1.0;
		return false;
	}

	double Width = static_cast<double>(ViewSize.cx);
	double Height = static_cast<double>( ViewSize.cy );
	double ImageWidth = static_cast<double>(ImageSize.cx);
	double ImageHeight = static_cast<double>(ImageSize.cy);

	if( Width / ImageWidth > Height / ImageHeight )
	{
		m_ZoomFit = Height / ImageHeight;
	}
	else
	{
		m_ZoomFit = Width / ImageWidth;
	}

	m_Zoom_Max = std::max(m_ZoomFit, m_Default_Max);
	m_Zoom_Min = std::min(m_ZoomFit, m_Default_Min);

	return true;
}

bool ZoomHelper::IsFit() const
{
	return m_bIsFit;
}

bool ZoomHelper::IsOne() const
{
	return m_bIsOne;
}

void ZoomHelper::CloneSettings(const ZoomHelper& Source)
{
	double originalZoom = m_Zoom;
	m_Zoom = Source.m_Zoom;
	m_bIsOne = Source.m_bIsOne;
	if(originalZoom != m_Zoom)
	{
		ZoomChanged();
	}
}

void ZoomHelper::Clear()
{
	m_Zoom = 1.0;
	m_Zoom_Min = m_Default_Min;
	m_Zoom_Max = m_Default_Max;
	m_ZoomFit = 1.0;
	m_bIsOne = false;
	m_bIsFit = false;

	ZoomChanged();
}

double ZoomHelper::GetZoomMin() const
{
	return m_Zoom_Min;
}
double ZoomHelper::GetZoomMax() const
{
	return m_Zoom_Max;
}

double ZoomHelper::GetZoomStep() const
{
	double step = m_ZoomStepSmall;
	if( m_Zoom >= 2.0 * m_ZoomStepLarge )
	{
		step = m_ZoomStepLarge;
	}

	return step;
}

void ZoomHelper::ZoomChanged()
{
	return; // Base class does nothing.  For overriding by derived classes
}
#pragma endregion Public Methods

