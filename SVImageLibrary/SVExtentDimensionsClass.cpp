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
		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyHeight ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyHeight, p_rsvValue.m_dHeight );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyWidth ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyWidth, p_rsvValue.m_dWidth );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyStartAngle ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyStartAngle, p_rsvValue.m_dStartAngle );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyEndAngle ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyEndAngle, p_rsvValue.m_dEndAngle );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyInnerRadius ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyInnerRadius, p_rsvValue.m_dInnerRadius );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOuterRadius ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOuterRadius, p_rsvValue.m_dOuterRadius );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyHeightScaleFactor, p_rsvValue.m_dHeightScaleFactor );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOutputHeight ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOutputHeight, p_rsvValue.m_lOutputHeight );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyWidthScaleFactor, p_rsvValue.m_dWidthScaleFactor );
		}

		if ( ( p_rsvValue.m_ulProperties & SVExtentPropertyOutputWidth ) != 0 )
		{
			SetExtentProperty( SVExtentPropertyOutputWidth, p_rsvValue.m_lOutputWidth );
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

HRESULT SVExtentDimensionsClass::DisableExtentProperty( SVExtentPropertyEnum p_eProperty )
{
	HRESULT l_hrOk = S_OK;

	if ( ( p_eProperty & ~SVExtentPropertyDimensionsAll ) == 0 )
	{
		if ( ( p_eProperty & ~SVExtentPropertyPoint ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & ~SVExtentPropertyLine ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & ~SVExtentPropertyRectangle ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyHeightScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			m_ulProperties &= ~SVExtentPropertyWidthScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & ~SVExtentPropertyPie ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyStartAngle ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyStartAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyEndAngle ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyEndAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyInnerRadius ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyInnerRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOuterRadius ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOuterRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_ulProperties &= ~SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVExtentDimensionsClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
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

HRESULT SVExtentDimensionsClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_FALSE;
	
	if ( ( p_eProperty & m_ulProperties ) != 0 )
	{
		switch( p_eProperty )
		{
			case SVExtentPropertyWidth:
			{
				p_rdValue = m_dWidth;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SVExtentPropertyHeight:
			{
				p_rdValue = m_dHeight;
				
				l_hrOk = S_OK;
				
				break;
			}

			case SVExtentPropertyWidthScaleFactor:
			{
				p_rdValue = m_dWidthScaleFactor;

				l_hrOk = S_OK;

				break;
			}
			case SVExtentPropertyHeightScaleFactor:
			{
				p_rdValue = m_dHeightScaleFactor;

				l_hrOk = S_OK;

				break;
			}
			
			case SVExtentPropertyStartAngle:
			{
				p_rdValue = m_dStartAngle;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SVExtentPropertyEndAngle:
			{
				p_rdValue = m_dEndAngle;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SVExtentPropertyInnerRadius:
			{
				p_rdValue = m_dInnerRadius;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SVExtentPropertyOuterRadius:
			{
				p_rdValue = m_dOuterRadius;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SVExtentPropertyOutputWidth:
			{
				p_rdValue = m_lOutputWidth;
				
				l_hrOk = S_OK;
				
				break;
			}
			case SVExtentPropertyOutputHeight:
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

HRESULT SVExtentDimensionsClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( ( p_eProperty & ~SVExtentPropertyDimensionsAll ) == 0 )
	{
		if ( ( p_eProperty & ~SVExtentPropertyLine ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyWidth ) != 0 )
			{
				m_dWidth = p_dValue;

				m_ulProperties |= SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyHeight ) != 0 )
			{
				m_dHeight = p_dValue;

				m_ulProperties |= SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_lOutputWidth = (long)p_dValue;

				m_ulProperties |= SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_lOutputHeight = (long)p_dValue;

				m_ulProperties |= SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		} //if ( ( p_eProperty & ~SVExtentPropertyLine ) == 0 )

		if ( ( p_eProperty & ~SVExtentPropertyRectangle ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyWidth ) != 0 )
			{
				m_dWidth = p_dValue;

				m_ulProperties |= SVExtentPropertyWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyHeight ) != 0 )
			{
				m_dHeight = p_dValue;

				m_ulProperties |= SVExtentPropertyHeight;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_lOutputWidth = (long)p_dValue;

				m_ulProperties |= SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_lOutputHeight = (long)p_dValue;

				m_ulProperties |= SVExtentPropertyOutputHeight;

				l_hrOk = S_OK;
			}
		}

		if ( ( p_eProperty & SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			m_dWidthScaleFactor = p_dValue;

			m_ulProperties |= SVExtentPropertyWidthScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			m_dHeightScaleFactor = p_dValue;

			m_ulProperties |= SVExtentPropertyHeightScaleFactor;

			l_hrOk = S_OK;
		}

		if ( ( p_eProperty & ~SVExtentPropertyPie ) == 0 )
		{
			if ( ( p_eProperty & SVExtentPropertyStartAngle ) != 0 )
			{
				m_dStartAngle = p_dValue;

				m_ulProperties |= SVExtentPropertyStartAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyEndAngle ) != 0 )
			{
				m_dEndAngle = p_dValue;

				m_ulProperties |= SVExtentPropertyEndAngle;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyInnerRadius ) != 0 )
			{
				m_dInnerRadius = p_dValue;

				m_ulProperties |= SVExtentPropertyInnerRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOuterRadius ) != 0 )
			{
				m_dOuterRadius = p_dValue;

				m_ulProperties |= SVExtentPropertyOuterRadius;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputWidth ) != 0 )
			{
				m_lOutputWidth = (long)p_dValue;

				m_ulProperties |= SVExtentPropertyOutputWidth;

				l_hrOk = S_OK;
			}

			if ( ( p_eProperty & SVExtentPropertyOutputHeight ) != 0 )
			{
				m_lOutputHeight = (long)p_dValue;

				m_ulProperties |= SVExtentPropertyOutputHeight;

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
		if ( ( m_ulProperties & SVExtentPropertyHeight ) != 0 )
		{
			l_bOk = l_bOk && m_dHeight == p_rsvValue.m_dHeight;
		}

		if ( ( m_ulProperties & SVExtentPropertyWidth ) != 0 )
		{
			l_bOk = l_bOk && m_dWidth == p_rsvValue.m_dWidth;
		}

		if ( ( m_ulProperties & SVExtentPropertyStartAngle ) != 0 )
		{
			l_bOk = l_bOk && m_dStartAngle == p_rsvValue.m_dStartAngle;
		}

		if ( ( m_ulProperties & SVExtentPropertyEndAngle ) != 0 )
		{
			l_bOk = l_bOk && m_dEndAngle == p_rsvValue.m_dEndAngle;
		}

		if ( ( m_ulProperties & SVExtentPropertyInnerRadius ) != 0 )
		{
			l_bOk = l_bOk && m_dInnerRadius == p_rsvValue.m_dInnerRadius;
		}

		if ( ( m_ulProperties & SVExtentPropertyOuterRadius ) != 0 )
		{
			l_bOk = l_bOk && m_dOuterRadius == p_rsvValue.m_dOuterRadius;
		}

		if ( ( m_ulProperties & SVExtentPropertyHeightScaleFactor ) != 0 )
		{
			l_bOk = l_bOk && m_dHeightScaleFactor == p_rsvValue.m_dHeightScaleFactor;
		}

		if ( ( m_ulProperties & SVExtentPropertyOutputHeight ) != 0 )
		{
			l_bOk = l_bOk && m_lOutputHeight == p_rsvValue.m_lOutputHeight;
		}

		if ( ( m_ulProperties & SVExtentPropertyWidthScaleFactor ) != 0 )
		{
			l_bOk = l_bOk && m_dWidthScaleFactor == p_rsvValue.m_dWidthScaleFactor;
		}

		if ( ( m_ulProperties & SVExtentPropertyOutputWidth ) != 0 )
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

bool SVExtentDimensionsClass::IsEnabled( SVExtentPropertyEnum p_eProperty ) const
{
	return ((m_ulProperties & p_eProperty) == (unsigned long) p_eProperty) && p_eProperty != SVExtentPropertyNone;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentDimensionsClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Mar 2011 13:13:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:02:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:01:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
