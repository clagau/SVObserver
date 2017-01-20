//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLevelCreateStruct
//* .File Name       : $Workfile:   SVToolLevelCreateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:25:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolLevelCreateStruct.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVToolLevelCreateStruct::SVToolLevelCreateStruct()
						:SVInspectionLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVInspectionLevelCreateStruct\nSVToolLevelCreateStruct";
}


SVToolLevelCreateStruct::SVToolLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS )
						:SVInspectionLevelCreateStruct( ROLCS )
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVInspectionLevelCreateStruct\nSVToolLevelCreateStruct";
	if( ROLCS.CheckCreateStructName( strCreateStructName ) )
	{
		ToolObjectInfo		= ( ( SVToolLevelCreateStruct& ) ROLCS ).ToolObjectInfo;
	}
}

SVToolLevelCreateStruct::~SVToolLevelCreateStruct()
{
}

