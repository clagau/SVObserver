//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentSizeStruct
//* .File Name       : $Workfile:   SVExtentSizeStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentSizeStruct.h"

SVExtentSizeStruct::SVExtentSizeStruct()
{
	Initialize();
}

SVExtentSizeStruct::SVExtentSizeStruct( const SVExtentSizeStruct& p_rsvValue )
{
	*this = p_rsvValue;
}

SVExtentSizeStruct::SVExtentSizeStruct( const SIZE& p_roValue )
{
	*this = p_roValue;
}

SVExtentSizeStruct::SVExtentSizeStruct( double l_dCX, double l_dCY )
{
	m_dCX = l_dCX;
	m_dCY = l_dCY;
}

SVExtentSizeStruct::~SVExtentSizeStruct()
{
	Initialize();
}

const SVExtentSizeStruct& SVExtentSizeStruct::operator = ( const SVExtentSizeStruct& p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_dCX = p_rsvValue.m_dCX;
		m_dCY = p_rsvValue.m_dCY;
	}	
	return *this;
}

const SVExtentSizeStruct& SVExtentSizeStruct::operator = ( const SIZE& p_roValue )
{
	Initialize();

	m_dCX = p_roValue.cx;
	m_dCY = p_roValue.cy;

	return *this;
}

HRESULT SVExtentSizeStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_dCX = 0.0;
	m_dCY = 0.0;

	return l_hrOk;
}

bool SVExtentSizeStruct::operator == ( const SVExtentSizeStruct& p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = m_dCX == p_rsvValue.m_dCX;
	l_bOk = l_bOk && m_dCY == p_rsvValue.m_dCY;

	return l_bOk;
}

bool SVExtentSizeStruct::operator != ( const SVExtentSizeStruct &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = ! ( *this == p_rsvValue );

	return l_bOk;
}

SVExtentSizeStruct::operator SIZE() const
{
	SIZE l_Size;

	l_Size.cx = (long) m_dCX;
	l_Size.cy = (long) m_dCY;

	return l_Size;
}

