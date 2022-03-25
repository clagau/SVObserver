//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerControlEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerControlEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:26  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxDigitizerControlEnums.h"

// Assign mappings for Control Camera Types
SVMatroxDigitizerControl::SVCameraEnumPair SVMatroxDigitizerControl::m_CameraEnumConvertor
{
	{SVMatroxDigitizerControl::SVSourceOffsetX,			M_SOURCE_OFFSET_X},
	{SVMatroxDigitizerControl::SVSourceOffsetY,			M_SOURCE_OFFSET_Y},
	{SVMatroxDigitizerControl::SVSourceSizeX,			M_SOURCE_SIZE_X},
	{SVMatroxDigitizerControl::SVSourceSizeY,			M_SOURCE_SIZE_Y}
};
