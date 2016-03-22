//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeDeviceParameterStruct
//* .File Name       : $Workfile:   SVGigeDeviceParameterStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:32  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVUtilityLibrary/SVString.h"
#include "SVGigeEnums.h"
#include "SVGigeParameterAccessor.h"

struct SVGigeDeviceParameterStruct
{
	SVString name;
	VARTYPE dataType;

	SVGigeParameterAccessor accessor;
	SVGigeDeviceParameterStruct(const SVString& p_name, VARTYPE p_dataType, const SVGigeParameterAccessor& accessor);
	SVGigeDeviceParameterStruct(const SVGigeDeviceParameterStruct& rParam);
	SVGigeDeviceParameterStruct& operator=(const SVGigeDeviceParameterStruct& rParam);
};

typedef std::map<SVGigeParameterEnum, SVGigeDeviceParameterStruct> SVGigeDeviceParameterMap;


