//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraId
//* .File Name       : $Workfile:   SVMatroxGigeCameraId.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:38  $
//******************************************************************************
#pragma once

#pragma region Includes

#pragma endregion Includes

struct SVMatroxGigeCameraId
{
	std::string m_serialNo;
	unsigned __int64 m_macAddress;
	unsigned __int64 m_ipAddress;
};

