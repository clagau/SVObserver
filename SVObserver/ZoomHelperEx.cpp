//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ZoomHelperEx
//* .File Name       : $Workfile:   ZoomHelperEx.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 13:23:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ZoomHelperEx.h"
#include "SVOMFCLibrary\ZoomHelper.h"
#include <algorithm>
#pragma endregion Includes

int ZoomHelperEx::s_InstanceCount = 0;
double ZoomHelperEx::m_scaleFactor[ ZoomHelperEx::s_scaleCount ] = {0};
HGDIOBJ ZoomHelperEx::m_hFont[ ZoomHelperEx::s_scaleCount ] = {NULL};

#pragma region Constructor
ZoomHelperEx::ZoomHelperEx()
{
	m_scaleIndex = s_scaleCount / 2;
	if(s_InstanceCount == 0)
	{
		Init();
	}
	s_InstanceCount++;
}

ZoomHelperEx::~ZoomHelperEx()
{
	s_InstanceCount--;
	if(s_InstanceCount == 0)
	{
		Exit();
	}
}

void ZoomHelperEx::Exit()
{
	if(s_InstanceCount != 0)
	{
		return;
	}

	for( unsigned int i = 0; i < s_scaleCount; i++ )
	{
		if( s_scaleCount / 2 <= i )
		{
			DeleteObject( m_hFont[ i ] );
		}

		m_hFont[ i ] = NULL;
	}
}


void ZoomHelperEx::Init()
{
	if(s_InstanceCount != 0)
	{
		return;
	}
	for (int i = s_scaleCount - 1; 0 <= i; i--)
	{
		// map the first 14 fonts (0 - 14) to the default since they're too small
		// to read anyway.
		if( i < static_cast< int >( s_scaleCount / 2 ) )
		{
			m_scaleFactor[ i ] = 1.0 / ( double )( ( s_scaleCount / 2 + 1 ) - i );

			m_hFont[ i ] = m_hFont[ s_scaleCount / 2 ];
		}
		else
		{
			m_scaleFactor[ i ] = ( double )( i - ( s_scaleCount / 2 - 1 ) );

			m_hFont[ i ] = CreateFont( static_cast< int >( m_scaleFactor[ i ] * 8 ), // logical height of font
				static_cast< long >( m_scaleFactor[ i ] * 6 ), // logical average character width
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
				NULL ); // pointer to typeface name string
		}
	}
}
#pragma endregion Constructor

#pragma region Public Methods
HGDIOBJ ZoomHelperEx::GetFont() const
{
	return m_hFont[m_scaleIndex];
}

unsigned ZoomHelperEx::GetScaleIndex() const
{
	return m_scaleIndex;
}

unsigned ZoomHelperEx::GetScaleCount() const
{
	return s_scaleCount;
}

unsigned ZoomHelperEx::FindScaleIndex()
{
	unsigned i;
	m_scaleIndex = s_scaleCount -1;
	for( i = 0; i < s_scaleCount; i++ )
	{
		if( m_Zoom <= m_scaleFactor[i] ) 
		{ 
			m_scaleIndex = i;
			break; 
		}
	}

	return m_scaleIndex;
}

void ZoomHelperEx::ZoomChanged()
{
	FindScaleIndex();
}

bool ZoomHelperEx::ChangeScaleIndex(EZoomMode e, unsigned int value)
{
	bool ret = false;
	double PrevZoom = m_Zoom;

	switch(e)
	{
	case EZOOM_SMALLEST:
		{
			m_scaleIndex = 0;
			break;
		}

	case EZOOM_SMALL:
		{
			m_scaleIndex = s_scaleCount / 2 - 3;
			break;
		}

	case EZOOM_NORMAL:
		{
			m_scaleIndex = s_scaleCount / 2;
			break;
		}

	case EZOOM_LARGE:
		{
			m_scaleIndex = s_scaleCount / 2 + 7;
			break;
		}

	case EZOOM_LARGEST:
		{
			m_scaleIndex = s_scaleCount - 1;
			break;
		}

	case EZOOM_IN:
		{
			m_scaleIndex = std::min( s_scaleCount - 1, m_scaleIndex + 1 );
			break;
		}

	case EZOOM_OUT:
		{
			if( m_scaleIndex > 0 )
			{
				m_scaleIndex = m_scaleIndex -1;
			}
			else
			{
				m_scaleIndex = 0;
			}

			break;
		}

	case EZOOM_VALUE:
		{
			m_scaleIndex = std::min( s_scaleCount - 1, value );
			break;
		}
	}

	m_Zoom = m_scaleFactor[m_scaleIndex];

	if(m_Zoom != PrevZoom)
	{
		ret = true;
	}

	return ret;
}
#pragma endregion Public Methods

