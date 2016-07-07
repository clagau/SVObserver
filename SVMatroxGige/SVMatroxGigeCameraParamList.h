//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamList
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMatroxGigeCameraParam.h"
#pragma endregion Includes

// There are 2 lists, one main list and one transient
// The transient list is for test mode support of LUT and Light Reference

struct SVMatroxGigeCameraParamList
{
	bool m_bUseTransient;
	SVMatroxGigeCameraParamDeque m_mainList;
	SVMatroxGigeCameraParamDeque m_transientList;

	SVMatroxGigeCameraParamList();
	void clear();
};

