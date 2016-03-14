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

#include "stdafx.h"
#include "SVToolLevelCreateStruct.h"

SVToolLevelCreateStruct::SVToolLevelCreateStruct()
						:SVInspectionLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVInspectionLevelCreateStruct\nSVToolLevelCreateStruct";
}


SVToolLevelCreateStruct::SVToolLevelCreateStruct( SVObjectLevelCreateStruct& ROLCS )
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

