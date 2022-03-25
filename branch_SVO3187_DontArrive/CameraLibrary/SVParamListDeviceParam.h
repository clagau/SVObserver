//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParamListDeviceParam
//* .File Name       : $Workfile:   SVParamListDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:10:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDeviceParam.h"
#pragma region Includes

class SVParamListDeviceParam : public SVDeviceParam
{
public:
	SVParamListDeviceParam();
	explicit SVParamListDeviceParam(SVDeviceParamEnum typeEnum);
	SVParamListDeviceParam(const SVParamListDeviceParam& rRhs);
	virtual ~SVParamListDeviceParam();
	virtual SVClonable* CloneImpl() const override;

	std::map <SVDeviceParamEnum, SVDeviceParamWrapper> mapParameters;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVParamListDeviceParam(typeEnum); }
	DEFINE_VISITABLE()
};

