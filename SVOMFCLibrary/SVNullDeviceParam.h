//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNullDeviceParam
//* .File Name       : $Workfile:   SVNullDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:08:06  $
//******************************************************************************

#pragma once

#include "SVDeviceParams.h"

class SVNullDeviceParam : public SVDeviceParam
{
public:
	SVNullDeviceParam();
	SVNullDeviceParam(SVDeviceParamEnum typeEnum);
	SVNullDeviceParam(const SVNullDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	SVNullDeviceParam& operator=(const SVNullDeviceParam& rhs);

	static SVDeviceParam* CreateNew() { return new SVNullDeviceParam; }
	DEFINE_VISITABLE()

protected:
	virtual void Init(SVDeviceParamEnum e);
};


