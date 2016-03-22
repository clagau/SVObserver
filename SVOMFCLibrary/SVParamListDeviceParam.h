//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParamListDeviceParam
//* .File Name       : $Workfile:   SVParamListDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:10:48  $
//******************************************************************************

#pragma once

#include "SVDeviceParams.h"

class SVParamListDeviceParam : public SVDeviceParam
{
public:
	SVParamListDeviceParam();
	SVParamListDeviceParam(SVDeviceParamEnum typeEnum);
	SVParamListDeviceParam(const SVParamListDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;

	std::map <SVDeviceParamEnum, SVDeviceParamWrapper> mapParameters;

	static SVDeviceParam* CreateNew() { return new SVParamListDeviceParam; }
	DEFINE_VISITABLE()
};

