//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraId
//* .File Name       : $Workfile:   SVMatroxGigeCameraId.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:38  $
//******************************************************************************
#ifndef SVMATROXGIGECAMERAID_H
#define SVMATROXGIGECAMERAID_H

#include "SVUtilityLibrary/SVString.h"

struct SVMatroxGigeCameraId
{
	SVString m_serialNo;
	unsigned __int64 m_macAddress;
	unsigned __int64 m_ipAddress;
};

#endif

