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

		m_svPointArray = rValue.m_svPointArray;

		m_dwColor = rValue.m_dwColor;
		
		m_bIsAngleValid = rValue.m_bIsAngleValid;
		m_dAngle = rValue.m_dAngle;
	}
	return *this;
}


HRESULT SVExtentLineStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_svPointArray.clear();

	m_dwColor = 0;

	m_bIsAngleValid = false;
	m_dAngle = 0.0;

	return l_hrOk;
}

