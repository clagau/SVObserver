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

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVAnalyzerLevelCreateStruct.h"

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Structure Name : SVAnalyzerLevelCreateStruct
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVAnalyzerLevelCreateStruct::SVAnalyzerLevelCreateStruct()
						:SVToolLevelCreateStruct()
{
	strCreateStructName = "SVObjectLevelCreateStruct\nSVIPDocLevelCreateStruct\nSVToolLevelCreateStruct\nSVAnalyzerLevelCreateStruct";
}


SVAnalyzerLevelCreateStruct::SVAnalyzerLevelCreateStruct( SVObjectLevelCreateStruct& ROLCS )
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

