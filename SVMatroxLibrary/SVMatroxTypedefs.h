// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxTypedefs
// * .File Name       : $Workfile:   SVMatroxTypedefs.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Oct 2013 08:50:46  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

/**
@SVObjectName Matrox Identifier

@SVObjectOverview

@SVObjectOperations None

*/
typedef __int64 SVMatroxIdentifier;
typedef __int64 MatroxType;
typedef long long SVMatroxInt;


/**
@SVObjectName Matrox Status Code

@SVObjectOverview

@SVObjectOperations None

*/

/**
@SVObjectName Matrox Hook Function Pointer

@SVObjectOverview

@SVObjectOperations None

*/
typedef SVMatroxIdentifier ( _stdcall *SVMatroxHookFunctionPtr )( SVMatroxIdentifier, SVMatroxIdentifier, void* );

