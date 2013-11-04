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

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldRotationAngle ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOldRotationAngle, p_rsvValue.m_dOldRotationAngle );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldPositionPoint ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOldPositionPoint, p_rsvValue.m_svOldPositionPoint );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldPositionPointX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldPositionPointX, p_rsvValue.m_svOldPositionPoint.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldPositionPointY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldPositionPointY, p_rsvValue.m_svOldPositionPoint.m_dPositionY );
			}
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldAbsPositionPoint ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOldAbsPositionPoint, p_rsvValue.m_svOldAbsPositionPoint );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldAbsPositionPointX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldAbsPositionPointX, p_rsvValue.m_svOldAbsPositionPoint.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldAbsPositionPointY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldAbsPositionPointY, p_rsvValue.m_svOldAbsPositionPoint.m_dPositionY );
			}
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldRotationPoint ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOldRotationPoint, p_rsvValue.m_svOldRotationPoint );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldRotationPointX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldRotationPointX, p_rsvValue.m_svOldRotationPoint.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldRotationPointY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldRotationPointY, p_rsvValue.m_svOldRotationPoint.m_dPositionY );
			}
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldTranslationOffset ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOldTranslationOffset, p_rsvValue.m_svOldTranslationOffset );
		}
		else
		{
			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldTranslationOffsetX ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldTranslationOffsetX, p_rsvValue.m_svOldTranslationOffset.m_dPositionX );
			}

			if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOldTranslationOffsetY ) != 0 )
			{
				SetExtentProperty( SVExtentPropertyOldTranslationOffsetY, p_rsvValue.m_svOldTranslationOffset.m_dPositionY );
			}
		}
	}// end if ( this != p_rsvValue )
	return *this;
}

HRESULT SVExtentPositionClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_ulProperties = 0;

	if ( m_svPositionPoint.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svPositionPointEndOfLine.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svTranslationOffset.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	m_dRotationAngle = 0.0;
	m_dOldRotationAngle = 0.0;

	if ( m_svOutputPositionPoint.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svOldPositionPoint.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svOldAbsPositionPoint.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svOldRotationPoint.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svOldTranslationOffset.Initialize() != S_OK )
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

		if ( ( p_eProperty & SVExtentPropertyOldRotationAngle ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldRotationAngle;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldPositionPointX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldPositionPointY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldPositionPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldAbsPositionPointX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldAbsPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldAbsPositionPointY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldAbsPositionPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldRotationPointX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldRotationPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldRotationPointY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldRotationPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldTranslationOffsetX ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldTranslationOffsetX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldTranslationOffsetY ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyOldTranslationOffsetY;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVExtentPositionClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	double l_dValue = 0.0;

	l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( l_hrOk == S_OK )
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
			case SVExtentPropertyOldRotationAngle:
			{
				p_rdValue = m_dOldRotationAngle;

				break;
			}
			case SVExtentPropertyOldPositionPointX:
			{
				p_rdValue = m_svOldPositionPoint.m_dPositionX;

				break;
			}
			case SVExtentPropertyOldPositionPointY:
			{
				p_rdValue = m_svOldPositionPoint.m_dPositionY;

				break;
			}
			case SVExtentPropertyOldAbsPositionPointX:
			{
				p_rdValue = m_svOldAbsPositionPoint.m_dPositionX;

				break;
			}
			case SVExtentPropertyOldAbsPositionPointY:
			{
				p_rdValue = m_svOldAbsPositionPoint.m_dPositionY;

				break;
			}
			case SVExtentPropertyOldRotationPointX:
			{
				p_rdValue = m_svOldRotationPoint.m_dPositionX;

				break;
			}
			case SVExtentPropertyOldRotationPointY:
			{
				p_rdValue = m_svOldRotationPoint.m_dPositionY;

				break;
			}
			case SVExtentPropertyOldTranslationOffsetX:
			{
				p_rdValue = m_svOldTranslationOffset.m_dPositionX;

				break;
			}
			case SVExtentPropertyOldTranslationOffsetY:
			{
				p_rdValue = m_svOldTranslationOffset.m_dPositionY;

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
	HRESULT l_hrOk = S_FALSE;

	SVExtentPointStruct l_svPoint;

	l_hrOk = GetExtentProperty( p_eProperty, l_svPoint );

	if ( l_hrOk == S_OK )
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
			case SVExtentPropertyOldPositionPoint:
			{
				p_rsvValue = m_svOldPositionPoint;

				break;
			}
			case SVExtentPropertyOldAbsPositionPoint:
			{
				p_rsvValue = m_svOldAbsPositionPoint;

				break;
			}
			case SVExtentPropertyOldRotationPoint:
			{
				p_rsvValue = m_svOldRotationPoint;

				break;
			}
			case SVExtentPropertyOldTranslationOffset:
			{
				p_rsvValue = m_svOldTranslationOffset;

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

		if ( ( p_eProperty & SVExtentPropertyOldRotationAngle ) != 0 )
		{
			m_dOldRotationAngle = p_dValue;

			m_ulProperties |= SVExtentPropertyOldRotationAngle;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldPositionPointX ) != 0 )
		{
			m_svOldPositionPoint.m_dPositionX = p_dValue;

			m_ulProperties |= SVExtentPropertyOldPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldPositionPointY ) != 0 )
		{
			m_svOldPositionPoint.m_dPositionY = p_dValue;

			m_ulProperties |= SVExtentPropertyOldPositionPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldAbsPositionPointX ) != 0 )
		{
			m_svOldAbsPositionPoint.m_dPositionX = p_dValue;

			m_ulProperties |= SVExtentPropertyOldAbsPositionPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldAbsPositionPointY ) != 0 )
		{
			m_svOldAbsPositionPoint.m_dPositionY = p_dValue;

			m_ulProperties |= SVExtentPropertyOldAbsPositionPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldRotationPointX ) != 0 )
		{
			m_svOldRotationPoint.m_dPositionX = p_dValue;

			m_ulProperties |= SVExtentPropertyOldRotationPointX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldRotationPointY ) != 0 )
		{
			m_svOldRotationPoint.m_dPositionY = p_dValue;

			m_ulProperties |= SVExtentPropertyOldRotationPointY;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldTranslationOffsetX ) != 0 )
		{
			m_svOldTranslationOffset.m_dPositionX = p_dValue;

			m_ulProperties |= SVExtentPropertyOldTranslationOffsetX;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldTranslationOffsetY ) != 0 )
		{
			m_svOldTranslationOffset.m_dPositionY = p_dValue;

			m_ulProperties |= SVExtentPropertyOldTranslationOffsetY;

			l_hrOk = S_OK;
		}
	}

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

		if ( ( p_eProperty & SVExtentPropertyOldPositionPoint ) != 0 )
		{
			m_svOldPositionPoint = p_svValue;

			m_ulProperties |= SVExtentPropertyOldPositionPoint;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldAbsPositionPoint ) != 0 )
		{
			m_svOldAbsPositionPoint = p_svValue;

			m_ulProperties |= SVExtentPropertyOldAbsPositionPoint;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldTranslationOffset ) != 0 )
		{
			m_svOldTranslationOffset = p_svValue;

			m_ulProperties |= SVExtentPropertyOldTranslationOffset;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyOldRotationPoint ) != 0 )
		{
			m_svOldRotationPoint = p_svValue;

			m_ulProperties |= SVExtentPropertyOldRotationPoint;

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentPositionClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:40:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Mar 2011 13:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:02:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:03:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
