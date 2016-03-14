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

const SVExtentLineStruct &SVExtentLineStruct::operator=( const SVExtentLineStruct &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_svPointArray.Copy(p_rsvValue.m_svPointArray);

		m_dwColor = p_rsvValue.m_dwColor;
		
		m_bIsAngleValid = p_rsvValue.m_bIsAngleValid;
		m_dAngle = p_rsvValue.m_dAngle;
	}
	return *this;
}


HRESULT SVExtentLineStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_svPointArray.RemoveAll();

	m_dwColor = 0;

	m_bIsAngleValid = false;
	m_dAngle = 0.0;

	return l_hrOk;
}

