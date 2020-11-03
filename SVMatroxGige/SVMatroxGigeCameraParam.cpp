//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParam
//* .File Name       : $Workfile:   SVMatroxGigeCameraParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:42  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeCameraParam.h"
#pragma endregion Includes

SVMatroxGigeCameraParam::SVMatroxGigeCameraParam(int p_ParameterID, _variant_t& p_rValue)
: m_ParameterID(p_ParameterID), m_value(p_rValue)
{
}
SVMatroxGigeCameraParam::SVMatroxGigeCameraParam(const SVMatroxGigeCameraParam& rParam)
: m_ParameterID(rParam.m_ParameterID), m_value(rParam.m_value)
{ 
}

SVMatroxGigeCameraParam::~SVMatroxGigeCameraParam()
{
}

SVMatroxGigeCameraParam& SVMatroxGigeCameraParam::operator=(const SVMatroxGigeCameraParam& rParam)
{
	Copy(rParam);
	return *this;
}

void SVMatroxGigeCameraParam::Copy(const SVMatroxGigeCameraParam& rParam)
{
	m_ParameterID = rParam.m_ParameterID;
	m_value = rParam.m_value;
}

