//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentLineStruct
//* .File Name       : $Workfile:   SVExtentLineStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentLineStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVExtentLineStruct::SVExtentLineStruct()
{
	Initialize();
}

SVExtentLineStruct::SVExtentLineStruct( const SVExtentLineStruct &p_rsvValue )
{
	*this = p_rsvValue;
}



SVExtentLineStruct::~SVExtentLineStruct()
{
	Initialize();
}

const SVExtentLineStruct &SVExtentLineStruct::operator=( const SVExtentLineStruct &rValue )
{
	if ( this != &rValue )
	{
		Initialize();

		m_PointVector = rValue.m_PointVector;

		m_dwColor = rValue.m_dwColor;
		
		m_bIsAngleValid = rValue.m_bIsAngleValid;
		m_dAngle = rValue.m_dAngle;
	}
	return *this;
}


void SVExtentLineStruct::Initialize()
{
	m_PointVector.clear();

	m_dwColor = 0;

	m_bIsAngleValid = false;
	m_dAngle = 0.0;
}

