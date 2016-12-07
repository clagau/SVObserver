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

#pragma region Includes
#include "SVDeviceParams.h"
#pragma endregion Includes

class SVNullDeviceParam : public SVDeviceParam
{
public:
	SVNullDeviceParam();
	SVNullDeviceParam(SVDeviceParamEnum typeEnum);
	SVNullDeviceParam(const SVNullDeviceParam& rhs);
	virtual ~SVNullDeviceParam();
	virtual SVClonable* CloneImpl() const override;
	SVNullDeviceParam& operator=(const SVNullDeviceParam& rhs);

	static SVDeviceParam* CreateNew() { return new SVNullDeviceParam; }
	DEFINE_VISITABLE()

protected:
	virtual void Init(SVDeviceParamEnum e);
};


