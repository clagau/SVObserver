//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeDeviceParameterStruct
//* .File Name       : $Workfile:   SVGigeDeviceParameterStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:28  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeDeviceParameterStruct.h"
#pragma endregion Includes

SVGigeDeviceParameterStruct::SVGigeDeviceParameterStruct(const std::string& p_name, VARTYPE p_dataType, const SVGigeParameterAccessor& p_accessor)
: name(p_name), dataType(p_dataType), accessor(p_accessor)
{
}

SVGigeDeviceParameterStruct::SVGigeDeviceParameterStruct(const SVGigeDeviceParameterStruct& rParam)
: name(rParam.name), dataType(rParam.dataType), accessor(rParam.accessor)
{
}

SVGigeDeviceParameterStruct& SVGigeDeviceParameterStruct::operator=(const SVGigeDeviceParameterStruct& rParam)
{
	if (this != &rParam)
	{
		name = rParam.name;
		dataType = rParam.dataType;
		accessor = rParam.accessor;
	}
	return *this;
}

