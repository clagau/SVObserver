//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxErrorEnum
//* .File Name       : $Workfile:   SVMatroxErrorEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:12:08  $
//******************************************************************************

#pragma once

/**
@SVObjectName Matrox Error Enum

@SVObjectOverview This enum is used to hold basic errors for the Matrox interface.

@SVObjectOperations None
*/

//@WARNING [Jim] It makes no sense to abstract the Matrox library and then 
// check Matrox specific codes outside that wrapper.  The following codes 
// should only be explicitly used within the Matrox wrapper.  Any value that
// is being checked outside the wrapper should be mapped to the standard 
// SVO error codes.
enum SVMatroxErrorEnum
{
	SVMEE_STATUS_OK					= 0x00000000,
	SVMEE_INVALID_HANDLE			= 0xcf000001,
	SVMEE_INVALID_PARAMETER			= 0xcf000002,
	SVMEE_WRONG_PARAMETER			= 0xcf000003,
	SVMEE_STRING_TOO_LARGE			= 0xcf000004,
	SVMEE_BAD_POINTER				= 0xcf000005,
	SVMEE_INVALID_STATE				= 0xcf000006,
	SVMEE_RESULT_ARRAY_TOO_SMALL	= 0xcf000007,
	SVMEE_MATROX_THREW_EXCEPTION    = 0xcf000008,
	SVMEE_INVALID_LOCK              = 0xcf000009,

	SVMEE_MATROX_ERROR				= 0xcf000000,

	SVMEE_INTERNAL_CONVERSION_ERROR	= 0xcf000bad,

	SVMEE_MBUF_ALLOCATION_FAILED	= 0xcf00c678
};

