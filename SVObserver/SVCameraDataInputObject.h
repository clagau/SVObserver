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

#ifndef SVCAMERADATAINPUTOBJECT_H
#define SVCAMERADATAINPUTOBJECT_H

#include "SVInputObject.h"

class SVCameraDataInputObject : public SVInputObject
{
public:
	SVCameraDataInputObject(LPCSTR strObjectName);
	SVCameraDataInputObject(SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCAMERADATAINPUTOBJECT);

	virtual ~SVCameraDataInputObject();

	virtual HRESULT Read(_variant_t& p_rValue);
};

#endif

