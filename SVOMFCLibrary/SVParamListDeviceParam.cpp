//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParamListDeviceParam
//* .File Name       : $Workfile:   SVParamListDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:10:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVParamListDeviceParam.h"
#pragma endregion Includes

SVParamListDeviceParam::SVParamListDeviceParam()
{
}

SVParamListDeviceParam::SVParamListDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVParamListDeviceParam::SVParamListDeviceParam(const SVParamListDeviceParam& rhs) 
: SVDeviceParam(rhs)
, mapParameters(rhs.mapParameters)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVParamListDeviceParam::~SVParamListDeviceParam()
{
}

SVClonable* SVParamListDeviceParam::CloneImpl() const
{
	return new SVParamListDeviceParam(*this);
}

