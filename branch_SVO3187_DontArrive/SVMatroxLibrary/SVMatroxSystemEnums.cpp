//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemEnums
//* .File Name       : $Workfile:   SVMatroxSystemEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:15:30  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxSystemEnums.h"

const MIL_INT cDefaultPitchByteType = M_DEFAULT_PITCH_BYTE_MULTIPLE;

// Assign mappings for Hook Types
SVMatroxSystemHook::SVMatroxSystemHookEnumPair SVMatroxSystemHook::m_convertor
{
	{SVMatroxSystemHook::SVCameraPresent,				M_CAMERA_PRESENT}
};

// Assign mappings for Hook Info Types
SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnumPair SVMatroxSystemHookInfo::m_convertor
{
	{SVMatroxSystemHookInfo::SVCameraNumber,			M_NUMBER},
	{SVMatroxSystemHookInfo::SVCameraPresent,			M_CAMERA_PRESENT},
	{SVMatroxSystemHookInfo::SVGigeMacAddress,			M_GC_MAC_ADDRESS}
};
