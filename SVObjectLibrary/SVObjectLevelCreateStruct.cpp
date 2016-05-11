//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLevelCreateStruct
//* .File Name       : $Workfile:   SVObjectLevelCreateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectLevelCreateStruct.h"

#include "SVObjectLibrary.h"
#pragma endregion Includes

SVObjectLevelCreateStruct::SVObjectLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct";
}

SVObjectLevelCreateStruct::SVObjectLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS )
{
	strCreateStructName = "SVObjectLevelCreateStruct";
	if( ROLCS.CheckCreateStructName( strCreateStructName ) )
	{
		OwnerObjectInfo = ROLCS.OwnerObjectInfo;		
	}
}

SVObjectLevelCreateStruct::~SVObjectLevelCreateStruct()
{
}

BOOL SVObjectLevelCreateStruct::CheckCreateStructName( const char* StrCreateStructureName ) const
{
	ASSERT( StrCreateStructureName );
	if( StrCreateStructureName )
	{
		return( nullptr != strstr( strCreateStructName, StrCreateStructureName ) );
	}
	return false;
}

