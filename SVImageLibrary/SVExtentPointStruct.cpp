//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPointStruct
//* .File Name       : $Workfile:   SVExtentPointStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentPointStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVExtentPointStruct::SVExtentPointStruct()
{
	Initialize();
}

SVExtentPointStruct::SVExtentPointStruct( const SVExtentPointStruct &p_rsvValue )
{
	*this = p_rsvValue;
}

SVExtentPointStruct::SVExtentPointStruct( const POINT &p_roValue )
{
	*this = p_roValue;
}

SVExtentPointStruct::SVExtentPointStruct( double l_dValueX, double l_dValueY )
{
	Initialize();

	m_dPositionX = l_dValueX;
	m_dPositionY = l_dValueY;
}

SVExtentPointStruct::~SVExtentPointStruct()
{
	Initialize();
}

const SVExtentPointStruct &SVExtentPointStruct::operator=( const SVExtentPointStruct &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_dPositionX = p_rsvValue.m_dPositionX;
		m_dPositionY = p_rsvValue.m_dPositionY;
	}	
	return *this;
}

const SVExtentPointStruct &SVExtentPointStruct::operator=( const POINT &p_roValue )
{
	Initialize();

	m_dPositionX = p_roValue.x;
	m_dPositionY = p_roValue.y;
	
	return *this;
}

HRESULT SVExtentPointStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_dPositionX = 0.0;
	m_dPositionY = 0.0;

	return l_hrOk;
}

bool SVExtentPointStruct::operator==( const SVExtentPointStruct &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = m_dPositionX == p_rsvValue.m_dPositionX;
	l_bOk = l_bOk && m_dPositionY == p_rsvValue.m_dPositionY;

	return l_bOk;
}

bool SVExtentPointStruct::operator!=( const SVExtentPointStruct &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = ! ( *this == p_rsvValue );

	return l_bOk;
}

SVExtentPointStruct::operator POINT() const
{
	POINT l_oPoint;

	l_oPoint.x = (long)m_dPositionX;
	l_oPoint.y = (long)m_dPositionY;

	return l_oPoint;
}

const SVExtentPointStruct& SVExtentPointStruct::operator -= (const SVExtentPointStruct& p_rPoint)
{
	m_dPositionX -= p_rPoint.m_dPositionX;
	m_dPositionY -= p_rPoint.m_dPositionY;
	return *this;
}

const SVExtentPointStruct& SVExtentPointStruct::operator += (const SVExtentPointStruct& p_rPoint)
{
	m_dPositionX += p_rPoint.m_dPositionX;
	m_dPositionY += p_rPoint.m_dPositionY;
	return *this;
}

SVExtentPointStruct operator - (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2)
{
	SVExtentPointStruct l_pt;
	l_pt.m_dPositionX = p_rPoint1.m_dPositionX - p_rPoint2.m_dPositionX;
	l_pt.m_dPositionY = p_rPoint1.m_dPositionY - p_rPoint2.m_dPositionY;
	return l_pt;
}

SVExtentPointStruct operator + (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2)
{
	SVExtentPointStruct l_pt;
	l_pt.m_dPositionX = p_rPoint1.m_dPositionX + p_rPoint2.m_dPositionX;
	l_pt.m_dPositionY = p_rPoint1.m_dPositionY + p_rPoint2.m_dPositionY;
	return l_pt;
}

SVExtentPointStruct operator / (const SVExtentPointStruct& p_rPoint1, double p_Constant)
{
	SVExtentPointStruct l_pt;
	l_pt.m_dPositionX = p_rPoint1.m_dPositionX / p_Constant;
	l_pt.m_dPositionY = p_rPoint1.m_dPositionY / p_Constant;
	return l_pt;
}

