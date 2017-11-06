//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePropertiesClass
//* .File Name       : $Workfile:   SVImagePropertiesClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVImagePropertiesClass.h"
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"

SVImagePropertiesClass::SVImagePropertiesClass()
{
	Initialize();
}

SVImagePropertiesClass::SVImagePropertiesClass( const SVImagePropertiesClass &p_rsvValue )
{
	Initialize();

	*this = p_rsvValue;
}

SVImagePropertiesClass::~SVImagePropertiesClass()
{
	Initialize();
}

const SVImagePropertiesClass &SVImagePropertiesClass::operator=( const SVImagePropertiesClass &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_ulProperties = p_rsvValue.m_ulProperties;

		m_iFormat = p_rsvValue.m_iFormat;
		m_iPixelDepth = p_rsvValue.m_iPixelDepth;
		m_iBandNumber = p_rsvValue.m_iBandNumber;
		m_iBandLink = p_rsvValue.m_iBandLink;
	}
	return *this;
}

HRESULT SVImagePropertiesClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_ulProperties = 0;

	m_iFormat = SVImageFormatUnknown;
	m_iPixelDepth = 0;
	m_iBandNumber = 0;
	m_iBandLink = 0;

	return l_hrOk;
}

HRESULT SVImagePropertiesClass::DisableImageProperty( SvDef::SVImagePropertyEnum p_eProperty )
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & ~SvDef::SVImagePropertyEnum::SVImagePropertyAll ) == 0 )
	{
		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyFormat ) != 0 )
		{
			m_ulProperties &= ~SvDef::SVImagePropertyEnum::SVImagePropertyFormat;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth ) != 0 )
		{
			m_ulProperties &= ~SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber ) != 0 )
		{
			m_ulProperties &= ~SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyBandLink ) != 0 )
		{
			m_ulProperties &= ~SvDef::SVImagePropertyEnum::SVImagePropertyBandLink;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVImagePropertiesClass::GetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, int &p_riValue ) const
{
	long l_lValue = 0;

	HRESULT l_hrOk = GetImageProperty( p_eProperty, l_lValue );

	if ( S_OK == l_hrOk )
	{
		p_riValue = (int)l_lValue;
	}

	return l_hrOk;
}

HRESULT SVImagePropertiesClass::GetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_OK;

	switch( p_eProperty )
	{
		case SvDef::SVImagePropertyEnum::SVImagePropertyFormat:
		{
			switch( p_rlValue )
			{
				case SVImageFormatUnknown:
				case SVImageFormatMono8:
				case SVImageFormatMono16:
				case SVImageFormatMono32:
				case SVImageFormatRGB5551:
				case SVImageFormatRGB565:
				case SVImageFormatRGB888:
				case SVImageFormatRGB8888:
				case SVImageFormatRGB101010:
				case SVImageFormatRGB161616:
				{
					p_rlValue = m_iFormat;

					break;
				}
				default:
				{
					l_hrOk = S_FALSE;

					break;
				}
			}

			break;
		}
		case SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth:
		{
			p_rlValue = m_iPixelDepth;

			break;
		}
		case SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber:
		{
			p_rlValue = m_iBandNumber;

			break;
		}
		case SvDef::SVImagePropertyEnum::SVImagePropertyBandLink:
		{
			p_rlValue = m_iBandLink;

			break;
		}
		default :
		{
			l_hrOk = S_FALSE;

			break;
		}
	}

	return l_hrOk;
}

HRESULT SVImagePropertiesClass::SetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, long p_lValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( ( p_eProperty & ~SvDef::SVImagePropertyEnum::SVImagePropertyAll ) == 0 )
	{
		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyFormat ) != 0 )
		{
			switch( p_lValue )
			{
				case SVImageFormatUnknown:
				case SVImageFormatMono8:
				case SVImageFormatMono16:
				case SVImageFormatMono32:
				case SVImageFormatRGB5551:
				case SVImageFormatRGB565:
				case SVImageFormatRGB888:
				case SVImageFormatRGB8888:
				case SVImageFormatRGB101010:
				case SVImageFormatRGB161616:
				{
					m_iFormat = (int)p_lValue;

					m_ulProperties |= SvDef::SVImagePropertyEnum::SVImagePropertyFormat;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					break;
				}
			}
		}

		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth ) != 0 )
		{
			m_iPixelDepth = (int)p_lValue;

			m_ulProperties |= SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber ) != 0 )
		{
			m_iBandNumber = (int)p_lValue;

			m_ulProperties |= SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SvDef::SVImagePropertyEnum::SVImagePropertyBandLink ) != 0 )
		{
			m_iBandLink = (int)p_lValue;

			m_ulProperties |= SvDef::SVImagePropertyEnum::SVImagePropertyBandLink;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}


bool SVImagePropertiesClass::operator==( const SVImagePropertiesClass &p_rsvValue ) const
{
	bool l_bOk = m_ulProperties == p_rsvValue.m_ulProperties;

	if ( ( m_ulProperties & SvDef::SVImagePropertyEnum::SVImagePropertyFormat ) != 0 )
	{
		l_bOk = l_bOk && m_iFormat == p_rsvValue.m_iFormat;
	}

	if ( ( m_ulProperties & SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth ) != 0 )
	{
		l_bOk = l_bOk && m_iPixelDepth == p_rsvValue.m_iPixelDepth;
	}

	if ( ( m_ulProperties & SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber ) != 0 )
	{
		l_bOk = l_bOk && m_iBandNumber == p_rsvValue.m_iBandNumber;
	}

	if ( ( m_ulProperties & SvDef::SVImagePropertyEnum::SVImagePropertyBandLink ) != 0 )
	{
		l_bOk = l_bOk && m_iBandLink == p_rsvValue.m_iBandLink;
	}

	return l_bOk;
}

bool SVImagePropertiesClass::operator!=( const SVImagePropertiesClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

