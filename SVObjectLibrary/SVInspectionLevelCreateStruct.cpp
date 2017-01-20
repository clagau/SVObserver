//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionLevelCreateStruct
//* .File Name       : $Workfile:   SVInspectionLevelCreateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVInspectionLevelCreateStruct.h"

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVInspectionLevelCreateStruct::SVInspectionLevelCreateStruct()
						 :SVObjectLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVInspectionLevelCreateStruct";
}

SVInspectionLevelCreateStruct::SVInspectionLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS )
						 :SVObjectLevelCreateStruct( ROLCS )
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVInspectionLevelCreateStruct";
	if( ROLCS.CheckCreateStructName( strCreateStructName ) )
	{
		InspectionObjectInfo	= ( ( SVInspectionLevelCreateStruct& ) ROLCS ).InspectionObjectInfo;
	}
}

SVInspectionLevelCreateStruct::~SVInspectionLevelCreateStruct()
{
}

