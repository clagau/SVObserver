//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPositionClass
//* .File Name       : $Workfile:   SVExtentPositionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentPositionClass.h"

SVExtentPositionClass::SVExtentPositionClass()
{
	Initialize();
}

SVExtentPositionClass::SVExtentPositionClass( const SVExtentPositionClass &p_rsvValue )
{
	Initialize();

	*this = p_rsvValue;
}

SVExtentPositionClass::~SVExtentPositionClass()
{
	Initialize();
}

const SVExtentPositionClass &SVExtentPositionClass::operator=( const SVExtentPositionClass &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyPositionPoint ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyPositionPoint, p_rsvValue.m_svPositionPoint );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyPositionPointX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyPositionPointX, p_rsvValue.m_svPositionPoint.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyPositionPointY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyPositionPointY, p_rsvValue.m_svPositionPoint.m_dPositionY );
			}
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyPositionPointEndOfLine ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyPositionPointEndOfLine, p_rsvValue.m_svPositionPointEndOfLine );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyPositionPointEndOfLineX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyPositionPointEndOfLineX, p_rsvValue.m_svPositionPointEndOfLine.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyPositionPointEndOfLineY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyPositionPointEndOfLineY, p_rsvValue.m_svPositionPointEndOfLine.m_dPositionY );
			}
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyTranslationOffset ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyTranslationOffset, p_rsvValue.m_svTranslationOffset );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyTranslationOffsetX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyTranslationOffsetX, p_rsvValue.m_svTranslationOffset.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyTranslationOffsetY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyTranslationOffsetY, p_rsvValue.m_svTranslationOffset.m_dPositionY );
			}
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyRotationAngle ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyRotationAngle, p_rsvValue.m_dRotationAngle );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOutputPositionPoint ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOutputPositionPoint, p_rsvValue.m_svOutputPositionPoint );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOutputPositionPointX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOutputPositionPointX, p_rsvValue.m_svOutputPositionPoint.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOutputPositionPointY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOutputPositionPointY, p_rsvValue.m_svOutputPositionPoint.m_dPositionY );
			}
		}
	}// end if ( this != p_rsvValue )
	return *this;
}

HRESULT SVExtentPositionClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_ulProperties = 0;

	if ( S_OK != m_svPositionPoint.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svPositionPointEndOfLine.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svTranslationOffset.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	m_dRotationAngle = 0.0;

	if ( S_OK != m_svOutputPositionPoint.Initialize() )
	{
		l_hrOk = S_FALSE;
	}
	return l_hrOk;
}

HRESULT SVExtentPositionClass::DisableExtentProperty( SVExtentPropertyEnum p_eProperty )
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & ~SVExtentPropertyPositionsAll ) == 0 )
	{
		if ( ( p_eProperty & SVExtentPropertyPositionPointX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyPositionPointY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyPositionPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyTranslationOffsetX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyTranslationOffsetX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyTranslationOffsetY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyTranslationOffsetY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyRotationAngle ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyRotationAngle;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOutputPositionPointX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOutputPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOutputPositionPointY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOutputPositionPointY;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVExtentPositionClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	double l_dValue = 0.0;

	HRESULT l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( S_OK == l_hrOk )
	{
		p_rlValue = (long)l_dValue;
	}

	return l_hrOk;
}

HRESULT SVExtentPositionClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & m_ulProperties ) != 0 )
	{
		switch ( p_eProperty )
		{
			case SVExtentPropertyPositionPointX:
			{
				p_rdValue = m_svPositionPoint.m_dPositionX;

				break;
			}
			case SVExtentPropertyPositionPointY:
			{
				p_rdValue = m_svPositionPoint.m_dPositionY;

				break;
			}
			case SVExtentPropertyPositionPointEndOfLineX:
			{
				p_rdValue = m_svPositionPointEndOfLine.m_dPositionX;

				break;
			}
			case SVExtentPropertyPositionPointEndOfLineY:
			{
				p_rdValue = m_svPositionPointEndOfLine.m_dPositionY;

				break;
			}
			case SVExtentPropertyTranslationOffsetX:
			{
				p_rdValue = m_svTranslationOffset.m_dPositionX;

				break;
			}
			case SVExtentPropertyTranslationOffsetY:
			{
				p_rdValue = m_svTranslationOffset.m_dPositionY;

				break;
			}
			case SVExtentPropertyRotationAngle:
			{
				p_rdValue = m_dRotationAngle;

				break;
			}
			case SVExtentPropertyOutputPositionPointX:
			{
				p_rdValue = m_svOutputPositionPoint.m_dPositionX;

				break;
			}
			case SVExtentPropertyOutputPositionPointY:
			{
				p_rdValue = m_svOutputPositionPoint.m_dPositionY;

				break;
			}
			default:
			{
				l_hrOk = S_FALSE;

				break;
			}
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVExtentPositionClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const
{
	SVExtentPointStruct l_svPoint;

	HRESULT l_hrOk = GetExtentProperty( p_eProperty, l_svPoint );

	if ( S_OK == l_hrOk )
	{
		p_roValue = l_svPoint;
	}

	return l_hrOk;
}

HRESULT SVExtentPositionClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & m_ulProperties ) != 0 )
	{
		switch ( p_eProperty )
		{
			case SVExtentPropertyPositionPoint:
			{
				p_rsvValue = m_svPositionPoint;

				break;
			}
			case SVExtentPropertyPositionPointEndOfLine:
			{
				p_rsvValue = m_svPositionPointEndOfLine;

				break;
			}
			case SVExtentPropertyTranslationOffset:
			{
				p_rsvValue = m_svTranslationOffset;

				break;
			}
			case SVExtentPropertyOutputPositionPoint:
			{
				p_rsvValue = m_svOutputPositionPoint;

				break;
			}
			default:
			{
				l_hrOk = S_FALSE;

				break;
			}
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVExtentPositionClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( ( p_eProperty & ~SVExtentPropertyPositionsAll ) == 0 )
	{
		if ( ( p_eProperty & SVExtentPropertyPositionPointX ) != 0 )
		{
			m_svPositionPoint.m_dPositionX = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyPositionPointY ) != 0 )
		{
			m_svPositionPoint.m_dPositionY = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyPositionPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyPositionPointEndOfLineX ) != 0 )
		{
			m_svPositionPointEndOfLine.m_dPositionX = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyPositionPointEndOfLineX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyPositionPointEndOfLineY ) != 0 )
		{
			m_svPositionPointEndOfLine.m_dPositionY = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyPositionPointEndOfLineY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyTranslationOffsetX ) != 0 )
		{
			m_svTranslationOffset.m_dPositionX = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyTranslationOffsetX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyTranslationOffsetY ) != 0 )
		{
			m_svTranslationOffset.m_dPositionY = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyTranslationOffsetY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyRotationAngle ) != 0 )
		{
			m_dRotationAngle = p_dValue;

			m_ulProperties |= SVExtentPropertyRotationAngle;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOutputPositionPointX ) != 0 )
		{
			m_svOutputPositionPoint.m_dPositionX = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyOutputPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOutputPositionPointY ) != 0 )
		{
			m_svOutputPositionPoint.m_dPositionY = (long)p_dValue;

			m_ulProperties |= SVExtentPropertyOutputPositionPointY;

			l_hrOk = S_OK;
		}
	} // if ( ( p_eProperty & ~SVExtentPropertyPositionsAll ) == 0 )


	return l_hrOk;
}

HRESULT SVExtentPositionClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( ( p_eProperty & ~SVExtentPropertyPositionsAll ) == 0 )
	{
		if ( ( p_eProperty & SVExtentPropertyPositionPoint ) != 0 )
		{
			m_svPositionPoint.m_dPositionX = (long)(p_svValue.m_dPositionX);
			m_svPositionPoint.m_dPositionY = (long)(p_svValue.m_dPositionY);

			m_ulProperties |= SVExtentPropertyPositionPoint;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyPositionPointEndOfLine ) != 0 )
		{
			m_svPositionPointEndOfLine.m_dPositionX = (long)(p_svValue.m_dPositionX);
			m_svPositionPointEndOfLine.m_dPositionY = (long)(p_svValue.m_dPositionY);

			m_ulProperties |= SVExtentPropertyPositionPointEndOfLine;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyTranslationOffset ) != 0 )
		{
			m_svTranslationOffset.m_dPositionX = (long)(p_svValue.m_dPositionX);
			m_svTranslationOffset.m_dPositionY = (long)(p_svValue.m_dPositionY);

			m_ulProperties |= SVExtentPropertyTranslationOffset;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOutputPositionPoint ) != 0 )
		{
			m_svOutputPositionPoint.m_dPositionX = (long)(p_svValue.m_dPositionX);
			m_svOutputPositionPoint.m_dPositionY = (long)(p_svValue.m_dPositionY);

			m_ulProperties |= SVExtentPropertyOutputPositionPoint;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

bool SVExtentPositionClass::operator==( const SVExtentPositionClass &p_rsvValue ) const
{
	bool l_bOk = m_ulProperties == p_rsvValue.m_ulProperties;

	if ( ( m_ulProperties & SVExtentPropertyPositionPoint ) != 0 )
	{
		l_bOk = l_bOk && m_svPositionPoint == p_rsvValue.m_svPositionPoint;
	}
	else
	{
		if ( ( m_ulProperties & SVExtentPropertyPositionPointX ) != 0 )
		{
			l_bOk = l_bOk && m_svPositionPoint.m_dPositionX == p_rsvValue.m_svPositionPoint.m_dPositionX;
		}

		if ( ( m_ulProperties & SVExtentPropertyPositionPointY ) != 0 )
		{
			l_bOk = l_bOk && m_svPositionPoint.m_dPositionY == p_rsvValue.m_svPositionPoint.m_dPositionY;
		}
	}

	if ( ( m_ulProperties & SVExtentPropertyPositionPointEndOfLine ) != 0 )
	{
		l_bOk = l_bOk && m_svPositionPointEndOfLine == p_rsvValue.m_svPositionPointEndOfLine;
	}
	else
	{
		if ( ( m_ulProperties & SVExtentPropertyPositionPointEndOfLineX ) != 0 )
		{
			l_bOk = l_bOk && m_svPositionPointEndOfLine.m_dPositionX == p_rsvValue.m_svPositionPointEndOfLine.m_dPositionX;
		}

		if ( ( m_ulProperties & SVExtentPropertyPositionPointEndOfLineY ) != 0 )
		{
			l_bOk = l_bOk && m_svPositionPointEndOfLine.m_dPositionY == p_rsvValue.m_svPositionPointEndOfLine.m_dPositionY;
		}
	}

	if ( ( m_ulProperties & SVExtentPropertyTranslationOffset ) != 0 )
	{
		l_bOk = l_bOk && m_svTranslationOffset == p_rsvValue.m_svTranslationOffset;
	}
	else
	{
		if ( ( m_ulProperties & SVExtentPropertyTranslationOffsetX ) != 0 )
		{
			l_bOk = l_bOk && m_svTranslationOffset.m_dPositionX == p_rsvValue.m_svTranslationOffset.m_dPositionX;
		}

		if ( ( m_ulProperties & SVExtentPropertyTranslationOffsetY ) != 0 )
		{
			l_bOk = l_bOk && m_svTranslationOffset.m_dPositionY == p_rsvValue.m_svTranslationOffset.m_dPositionY;
		}
	}

	if ( ( m_ulProperties & SVExtentPropertyRotationAngle ) != 0 )
	{
		l_bOk = l_bOk && m_dRotationAngle == p_rsvValue.m_dRotationAngle;
	}

	if ( ( m_ulProperties & SVExtentPropertyOutputPositionPoint ) != 0 )
	{
		l_bOk = l_bOk && m_svOutputPositionPoint == p_rsvValue.m_svOutputPositionPoint;
	}
	else
	{
		if ( ( m_ulProperties & SVExtentPropertyOutputPositionPointX ) != 0 )
		{
			l_bOk = l_bOk && m_svOutputPositionPoint.m_dPositionX == p_rsvValue.m_svOutputPositionPoint.m_dPositionX;
		}

		if ( ( m_ulProperties & SVExtentPropertyOutputPositionPointY ) != 0 )
		{
			l_bOk = l_bOk && m_svOutputPositionPoint.m_dPositionY == p_rsvValue.m_svOutputPositionPoint.m_dPositionY;
		}
	}

	return l_bOk;
}

bool SVExtentPositionClass::operator!=( const SVExtentPositionClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

bool SVExtentPositionClass::IsEnabled( SVExtentPropertyEnum p_eProperty ) const
{
	return ((m_ulProperties & p_eProperty) == (unsigned long) p_eProperty) && p_eProperty != SVExtentPropertyNone;
}

