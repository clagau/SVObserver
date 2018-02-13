//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentDimensionsClass
//* .File Name       : $Workfile:   SVExtentDimensionsClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentDimensionsClass.h"

SVExtentDimensionsClass::SVExtentDimensionsClass()
{
	Initialize();
}

SVExtentDimensionsClass::SVExtentDimensionsClass( const SVExtentDimensionsClass &p_rsvValue )
{
	Initialize();

	*this = p_rsvValue;
}

SVExtentDimensionsClass::~SVExtentDimensionsClass()
{
	Initialize();
}

const SVExtentDimensionsClass &SVExtentDimensionsClass::operator=( const SVExtentDimensionsClass &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyHeight ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyHeight, p_rsvValue.m_dHeight );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyWidth ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyWidth, p_rsvValue.m_dWidth );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyStartAngle ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyStartAngle, p_rsvValue.m_dStartAngle );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyEndAngle ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyEndAngle, p_rsvValue.m_dEndAngle );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyInnerRadius ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyInnerRadius, p_rsvValue.m_dInnerRadius );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyOuterRadius ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyOuterRadius, p_rsvValue.m_dOuterRadius );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyHeightScaleFactor, p_rsvValue.m_dHeightScaleFactor );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyOutputHeight ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyOutputHeight, p_rsvValue.m_lOutputHeight );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyWidthScaleFactor, p_rsvValue.m_dWidthScaleFactor );
		}

		if ( ( p_rsvValue.m_ulProperties & SvDef::SVExtentPropertyOutputWidth ) != 0 )
		{
			SetExtentProperty( SvDef::SVExtentPropertyOutputWidth, p_rsvValue.m_lOutputWidth );
		}
	}
	return *this;
}

HRESULT SVExtentDimensionsClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_ulProperties = 0;

	m_dWidth = 0.0;
	m_dHeight = 0.0;
	m_dStartAngle = 0.0;
	m_dEndAngle = 0.0;
	m_dInnerRadius = 0.0;
	m_dOuterRadius = 0.0;

	m_lOutputWidth = 0;
	m_lOutputHeight = 0;

	return l_hrOk;
}

HRESULT SVExtentDimensionsClass::DisableExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty )
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & ~SvDef::SVExtentPropertyDimensionsAll ) == 0 )
	{
		if ( ( p_eProperty & ~SvDef::SVExtentPropertyPoint ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & ~SvDef::SVExtentPropertyLine ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & ~SvDef::SVExtentPropertyRectangle ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & SvDef::SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			m_ulProperties &= ~SvDef::SVExtentPropertyHeightScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			m_ulProperties &= ~SvDef::SVExtentPropertyWidthScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & ~SvDef::SVExtentPropertyPie ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyStartAngle ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyStartAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyEndAngle ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyEndAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyInnerRadius ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyInnerRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOuterRadius ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOuterRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVExtentDimensionsClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	double l_dValue = 0.0;

	HRESULT l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( S_OK == l_hrOk )
	{
		p_rlValue = (long)l_dValue;
	}

	return l_hrOk;
}

HRESULT SVExtentDimensionsClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_FALSE;
	
	if ( ( p_eProperty & m_ulProperties ) != 0 )
	{
		switch( p_eProperty )
		{
			case SvDef::SVExtentPropertyWidth:
			{
				p_rdValue = m_dWidth;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SvDef::SVExtentPropertyHeight:
			{
				p_rdValue = m_dHeight;
				
				l_hrOk = S_OK;
				
				break;
			}

			case SvDef::SVExtentPropertyWidthScaleFactor:
			{
				p_rdValue = m_dWidthScaleFactor;

				l_hrOk = S_OK;

				break;
			}
			case SvDef::SVExtentPropertyHeightScaleFactor:
			{
				p_rdValue = m_dHeightScaleFactor;

				l_hrOk = S_OK;

				break;
			}
			
			case SvDef::SVExtentPropertyStartAngle:
			{
				p_rdValue = m_dStartAngle;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SvDef::SVExtentPropertyEndAngle:
			{
				p_rdValue = m_dEndAngle;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SvDef::SVExtentPropertyInnerRadius:
			{
				p_rdValue = m_dInnerRadius;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SvDef::SVExtentPropertyOuterRadius:
			{
				p_rdValue = m_dOuterRadius;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SvDef::SVExtentPropertyOutputWidth:
			{
				p_rdValue = m_lOutputWidth;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SvDef::SVExtentPropertyOutputHeight:
			{
				p_rdValue = m_lOutputHeight;
				
				l_hrOk = S_OK;
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
		
	return l_hrOk;
}

HRESULT SVExtentDimensionsClass::SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( ( p_eProperty & ~SvDef::SVExtentPropertyDimensionsAll ) == 0 )
	{
		if ( ( p_eProperty & ~SvDef::SVExtentPropertyLine ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyWidth ) != 0 )
			{
				m_dWidth = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyHeight ) != 0 )
			{
				m_dHeight = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_lOutputWidth = (long)p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_lOutputHeight = (long)p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		} //if ( ( p_eProperty & ~SvDef::SVExtentPropertyLine ) == 0 )

		if ( ( p_eProperty & ~SvDef::SVExtentPropertyRectangle ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyWidth ) != 0 )
			{
				m_dWidth = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyHeight ) != 0 )
			{
				m_dHeight = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_lOutputWidth = (long)p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_lOutputHeight = (long)p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & SvDef::SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			m_dWidthScaleFactor = p_dValue;

			m_ulProperties |= SvDef::SVExtentPropertyWidthScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			m_dHeightScaleFactor = p_dValue;

			m_ulProperties |= SvDef::SVExtentPropertyHeightScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & ~SvDef::SVExtentPropertyPie ) == 0 )
		{
			if ( ( p_eProperty & SvDef::SVExtentPropertyStartAngle ) != 0 )
			{
				m_dStartAngle = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyStartAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyEndAngle ) != 0 )
			{
				m_dEndAngle = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyEndAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyInnerRadius ) != 0 )
			{
				m_dInnerRadius = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyInnerRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOuterRadius ) != 0 )
			{
				m_dOuterRadius = p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOuterRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputWidth ) != 0 )
			{
				m_lOutputWidth = (long)p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SvDef::SVExtentPropertyOutputHeight ) != 0 )
			{
				m_lOutputHeight = (long)p_dValue;

				m_ulProperties |= SvDef::SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

bool SVExtentDimensionsClass::operator==( const SVExtentDimensionsClass &p_rsvValue ) const
{
	bool l_bOk = m_ulProperties == p_rsvValue.m_ulProperties;

	if ( l_bOk )
	{
		if ( ( m_ulProperties & SvDef::SVExtentPropertyHeight ) != 0 )
		{
			l_bOk = l_bOk && m_dHeight == p_rsvValue.m_dHeight;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyWidth ) != 0 )
		{
			l_bOk = l_bOk && m_dWidth == p_rsvValue.m_dWidth;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyStartAngle ) != 0 )
		{
			l_bOk = l_bOk && m_dStartAngle == p_rsvValue.m_dStartAngle;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyEndAngle ) != 0 )
		{
			l_bOk = l_bOk && m_dEndAngle == p_rsvValue.m_dEndAngle;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyInnerRadius ) != 0 )
		{
			l_bOk = l_bOk && m_dInnerRadius == p_rsvValue.m_dInnerRadius;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyOuterRadius ) != 0 )
		{
			l_bOk = l_bOk && m_dOuterRadius == p_rsvValue.m_dOuterRadius;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			l_bOk = l_bOk && m_dHeightScaleFactor == p_rsvValue.m_dHeightScaleFactor;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyOutputHeight ) != 0 )
		{
			l_bOk = l_bOk && m_lOutputHeight == p_rsvValue.m_lOutputHeight;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			l_bOk = l_bOk && m_dWidthScaleFactor == p_rsvValue.m_dWidthScaleFactor;
		}

		if ( ( m_ulProperties & SvDef::SVExtentPropertyOutputWidth ) != 0 )
		{
			l_bOk = l_bOk && m_lOutputWidth == p_rsvValue.m_lOutputWidth;
		}
	}

	return l_bOk;
}

bool SVExtentDimensionsClass::operator!=( const SVExtentDimensionsClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

bool SVExtentDimensionsClass::IsEnabled( SvDef::SVExtentPropertyEnum p_eProperty ) const
{
	return ((m_ulProperties & p_eProperty) == (unsigned long) p_eProperty) && p_eProperty != SvDef::SVExtentPropertyNone;
}

