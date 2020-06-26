//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNullDeviceParam
//* .File Name       : $Workfile:   SVNullDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:07:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVNullDeviceParam.h"
#pragma endregion Includes

SVClonable* SVNullDeviceParam::CloneImpl() const
{
	return new SVNullDeviceParam(*this);
}

SVNullDeviceParam::SVNullDeviceParam()
: SVDeviceParam(DeviceParamInvalid)
{
}

// Why pass a parameter and not use it...
SVNullDeviceParam::SVNullDeviceParam(SVDeviceParamEnum )
: SVDeviceParam(DeviceParamInvalid)
{
}

SVNullDeviceParam::SVNullDeviceParam(const SVNullDeviceParam& rhs)
: SVDeviceParam(rhs)
{
}

SVNullDeviceParam::~SVNullDeviceParam()
{
}

SVNullDeviceParam& SVNullDeviceParam::operator=(const SVNullDeviceParam&)
{
	return *this;
}
