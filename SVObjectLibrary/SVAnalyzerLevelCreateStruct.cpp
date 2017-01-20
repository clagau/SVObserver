//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLevelCreateStruct
//* .File Name       : $Workfile:   SVAnalyzerLevelCreateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVAnalyzerLevelCreateStruct.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVAnalyzerLevelCreateStruct::SVAnalyzerLevelCreateStruct()
						:SVToolLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVIPDocLevelCreateStruct\nSVToolLevelCreateStruct\nSVAnalyzerLevelCreateStruct";
}

SVAnalyzerLevelCreateStruct::SVAnalyzerLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS )
						:SVToolLevelCreateStruct( ROLCS )
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVIPDocLevelCreateStruct\nSVToolLevelCreateStruct\nSVAnalyzerLevelCreateStruct";
	if( ROLCS.CheckCreateStructName( strCreateStructName ) )
	{
		AnalyzerObjectInfo		= ( ( SVAnalyzerLevelCreateStruct& ) ROLCS ).AnalyzerObjectInfo;
	}
}

SVAnalyzerLevelCreateStruct::~SVAnalyzerLevelCreateStruct()
{
}

