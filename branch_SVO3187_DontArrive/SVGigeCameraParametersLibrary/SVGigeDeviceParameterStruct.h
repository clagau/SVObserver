//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeDeviceParameterStruct
//* .File Name       : $Workfile:   SVGigeDeviceParameterStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/SVGigeEnums.h"
#include "SVGigeParameterAccessor.h"
#pragma endregion Includes

struct SVGigeDeviceParameterStruct
{
	std::string name;
	VARTYPE dataType;

	SVGigeParameterAccessor accessor;
	SVGigeDeviceParameterStruct(const std::string& p_name, VARTYPE p_dataType, const SVGigeParameterAccessor& accessor);
	SVGigeDeviceParameterStruct(const SVGigeDeviceParameterStruct& rParam);
	SVGigeDeviceParameterStruct& operator=(const SVGigeDeviceParameterStruct& rParam);
};

typedef std::map<SvDef::SVGigeParameterEnum, SVGigeDeviceParameterStruct> SVGigeDeviceParameterMap;


