//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLevelCreateStruct
//* .File Name       : $Workfile:   SVObjectLevelCreateStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:51:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectInfoStruct.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVObjectLevelCreateStruct
// -----------------------------------------------------------------------------
// .Description : Base structure for SVM_CREATE... messages
//              : Supports structure identifying by structure name
////////////////////////////////////////////////////////////////////////////////
struct SVObjectLevelCreateStruct
{
	SVObjectLevelCreateStruct();
	SVObjectLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS );

	virtual ~SVObjectLevelCreateStruct();

	bool CheckCreateStructName( const char* StrCreateStructureName ) const;

	SVObjectInfoStruct OwnerObjectInfo;

protected:
	const char* strCreateStructName;
};

