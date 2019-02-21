//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraDataInputObject
//* .File Name       : $Workfile:   SVCameraDataInputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:49:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVInputObject.h"
#pragma endregion Includes

class SVCameraDataInputObject : public SVInputObject
{
#pragma region Constructor
public:
	SVCameraDataInputObject(LPCSTR strObjectName);
	SVCameraDataInputObject(SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCAMERADATAINPUTOBJECT);

	virtual ~SVCameraDataInputObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual HRESULT Read(_variant_t& p_rValue) override;
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods
};
