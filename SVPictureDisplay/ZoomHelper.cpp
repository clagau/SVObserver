//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : ZoomHelper
//* .File Name       : $Workfile:   ZoomHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:29:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ZoomHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
ZoomHelper::ZoomHelper()
{
	Clear();
}
#pragma endregion Constructor

#pragma region Public Methods
double ZoomHelper::GetZoom() const
{
	if(m_Zoom > 0)
	{
		return m_Zoom;
	}
	else
	{
		return 1.0;
	}
}

void ZoomHelper::SetZoom( double zoom, bool changeMax )
{
	m_bIsOne = false;
	m_bIsFit = false;
	if(changeMax)
	{
		m_Zoom = zoom;
		m_Zoom_Max = max( zoom, m_Zoom_Max );
		m_Zoom_Min = min( m_Zoom_Min, zoom );
	}
	else
	{
		m_Zoom = zoom;
		m_Zoom = min( zoom, m_Zoom_Max );
		m_Zoom = max( zoom, m_Zoom_Min );
	}
}

double ZoomHelper::SetToFit()
{
	m_Zoom = m_ZoomFit;
	m_bIsOne = false;
	m_bIsFit = true;
	return m_Zoom;
}

double ZoomHelper::SetToOne()
{
	m_Zoom = 1.0;
	m_bIsOne = true;
	m_bIsFit = false;
	return m_Zoom;
}

double ZoomHelper::ZoomPlus()
{
	m_bIsOne = false;
	m_bIsFit = false;

	//Calculate the change in zoomfactor dependent on the Zoomfactor
	if( m_Zoom >= 2.0 * m_ZoomStepLarge )
	{
		m_Zoom += m_ZoomStepLarge;
	}
	else
	{
		m_Zoom += m_ZoomStepSmall;
	}

	m_Zoom = min(m_Zoom_Max, m_Zoom);
	return m_Zoom;
}

double ZoomHelper::ZoomMinus()
{
	m_bIsOne = false;
	m_bIsFit = false;

	if( m_Zoom >= ( 2.0 * m_ZoomStepLarge ) )
	{
		m_Zoom -= m_ZoomStepLarge;
	}
	else
	{
		m_Zoom -= m_ZoomStepSmall;
	}

	m_Zoom = max( m_Zoom, m_Zoom_Min );
	return m_Zoom;
}

double ZoomHelper::CalculateZoomFit(CSize ImageSize, CSize ViewSize)
{
	if( ImageSize.cx < 1 ||
		ImageSize.cy < 1 ||
		ViewSize.cx < 1 ||
		ViewSize.cy < 1 )
	{
		ASSERT(FALSE);
		m_ZoomFit = 1.0;
		return m_ZoomFit;
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
	m_Zoom_Max = max( m_ZoomFit ,m_Default_Max );
	m_Zoom_Min = min( m_ZoomFit, m_Default_Min );
	return m_ZoomFit;
}

bool ZoomHelper::IsFit() const
{
	return m_bIsFit;
}

bool ZoomHelper::IsOne() const
{
	return m_bIsOne;
}

void ZoomHelper::CloneSettings(const ZoomHelper &Source)
{
	m_Zoom = Source.m_Zoom;
	m_Zoom_Min= Source.m_Zoom_Min;
	m_Zoom_Max= Source.m_Zoom_Max;
	m_ZoomFit= Source.m_ZoomFit;
	m_bIsOne= Source.m_bIsOne;
	m_bIsFit= Source.m_bIsFit;
}

void ZoomHelper::Clear()
{
	m_Zoom = 1.0;
	m_Zoom_Min = 0.1;
	m_Zoom_Max = 10;
	m_ZoomFit = 1.0;
	m_bIsOne = false;
	m_bIsFit = false;
	m_Default_Min = 0.1;
	m_Default_Max = 10.0;
	m_ZoomStepLarge = 1.0;
	m_ZoomStepSmall = 0.1;
}

double ZoomHelper::GetZoomMin() const
{
	return m_Zoom_Min;
}

double ZoomHelper::GetZoomMax() const
{
	return m_Zoom_Max;
}

double ZoomHelper::GetZoomStepSmall() const
{
	return m_ZoomStepSmall;
}


double ZoomHelper::GetZoomStepLarge() const
{
	return m_ZoomStepLarge;
}

#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\ZoomHelper.cpp_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:29:00   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */