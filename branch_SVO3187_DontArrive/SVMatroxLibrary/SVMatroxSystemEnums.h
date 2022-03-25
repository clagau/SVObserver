//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemEnums
//* .File Name       : $Workfile:   SVMatroxSystemEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:15:32  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

/**
@SVObjectName Matrox System Hook Enum

@SVObjectOverview This enum is used to get the system hook values.

@SVObjectOperations None

*/
struct SVMatroxSystemHook
{
	enum SVMatroxSystemHookEnum 
	{
		SVCameraPresent
	};

	typedef std::vector<std::pair<SVMatroxSystemHookEnum, long long>> SVMatroxSystemHookEnumPair;
	static SVMatroxSystemHookEnumPair m_convertor;
};

/**
@SVObjectName Matrox System Hook Info Enum

@SVObjectOverview This enum is used to get the system hook values.

@SVObjectOperations None

*/
struct SVMatroxSystemHookInfo
{
	enum SVMatroxSystemHookInfoEnum 
	{
		SVCameraNumber,
		SVCameraPresent,
		SVGigeMacAddress
	};

	typedef std::vector<std::pair<SVMatroxSystemHookInfoEnum, long long>> SVMatroxSystemHookInfoEnumPair;
	static SVMatroxSystemHookInfoEnumPair m_convertor;
};

