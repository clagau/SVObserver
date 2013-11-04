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

HRESULT SVImagePropertiesClass::DisableImageProperty( SVImagePropertyEnum p_eProperty )
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & ~SVImagePropertyAll ) == 0 )
	{
		if ( ( p_eProperty & SVImagePropertyFormat ) != 0 )
		{
			m_ulProperties &= ~SVImagePropertyFormat;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVImagePropertyPixelDepth ) != 0 )
		{
			m_ulProperties &= ~SVImagePropertyPixelDepth;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVImagePropertyBandNumber ) != 0 )
		{
			m_ulProperties &= ~SVImagePropertyBandNumber;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVImagePropertyBandLink ) != 0 )
		{
			m_ulProperties &= ~SVImagePropertyBandLink;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVImagePropertiesClass::GetImageProperty( SVImagePropertyEnum p_eProperty, int &p_riValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	long l_lValue = 0;

	l_hrOk = GetImageProperty( p_eProperty, l_lValue );

	if ( l_hrOk == S_OK )
	{
		p_riValue = (int)l_lValue;
	}

	return l_hrOk;
}

HRESULT SVImagePropertiesClass::GetImageProperty( SVImagePropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_OK;

	switch( p_eProperty )
	{
		case SVImagePropertyFormat:
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
		case SVImagePropertyPixelDepth:
		{
			p_rlValue = m_iPixelDepth;

			break;
		}
		case SVImagePropertyBandNumber:
		{
			p_rlValue = m_iBandNumber;

			break;
		}
		case SVImagePropertyBandLink:
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

HRESULT SVImagePropertiesClass::SetImageProperty( SVImagePropertyEnum p_eProperty, long p_lValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( ( p_eProperty & ~SVImagePropertyAll ) == 0 )
	{
		if ( ( p_eProperty & SVImagePropertyFormat ) != 0 )
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

					m_ulProperties |= SVImagePropertyFormat;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					break;
				}
			}
		}

		if ( ( p_eProperty & SVImagePropertyPixelDepth ) != 0 )
		{
			m_iPixelDepth = (int)p_lValue;

			m_ulProperties |= SVImagePropertyPixelDepth;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVImagePropertyBandNumber ) != 0 )
		{
			m_iBandNumber = (int)p_lValue;

			m_ulProperties |= SVImagePropertyBandNumber;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVImagePropertyBandLink ) != 0 )
		{
			m_iBandLink = (int)p_lValue;

			m_ulProperties |= SVImagePropertyBandLink;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}


bool SVImagePropertiesClass::operator==( const SVImagePropertiesClass &p_rsvValue ) const
{
	bool l_bOk = m_ulProperties == p_rsvValue.m_ulProperties;

	if ( ( m_ulProperties & SVImagePropertyFormat ) != 0 )
	{
		l_bOk = l_bOk && m_iFormat == p_rsvValue.m_iFormat;
	}

	if ( ( m_ulProperties & SVImagePropertyPixelDepth ) != 0 )
	{
		l_bOk = l_bOk && m_iPixelDepth == p_rsvValue.m_iPixelDepth;
	}

	if ( ( m_ulProperties & SVImagePropertyBandNumber ) != 0 )
	{
		l_bOk = l_bOk && m_iBandNumber == p_rsvValue.m_iBandNumber;
	}

	if ( ( m_ulProperties & SVImagePropertyBandLink ) != 0 )
	{
		l_bOk = l_bOk && m_iBandLink == p_rsvValue.m_iBandLink;
	}

	return l_bOk;
}

bool SVImagePropertiesClass::operator!=( const SVImagePropertiesClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImagePropertiesClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:53:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Mar 2011 13:13:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Feb 2011 11:51:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jul 2007 08:56:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added include for HBitmapUtilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:02:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:04:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
