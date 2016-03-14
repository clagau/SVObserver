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

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVObjectLevelCreateStruct.h"

#include "SVObjectLibrary.h"

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Structure Name : SVObjectLevelCreateStruct
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVObjectLevelCreateStruct::SVObjectLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct";
}


SVObjectLevelCreateStruct::SVObjectLevelCreateStruct( SVObjectLevelCreateStruct& ROLCS )
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

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVObjectLevelCreateStruct::CheckCreateStructType( SVObjectLevelCreateStruct* PCreateStructure )
{
	if( PCreateStructure != NULL )
	{
		return( strstr( strCreateStructName, PCreateStructure->strCreateStructName ) != NULL );
	}
	return FALSE;
}

BOOL SVObjectLevelCreateStruct::CheckCreateStructName( const char* StrCreateStructureName )
{
	ASSERT( StrCreateStructureName );
	if( StrCreateStructureName )
	{
		return( strstr( strCreateStructName, StrCreateStructureName ) != NULL );
	}
	return FALSE;
}

