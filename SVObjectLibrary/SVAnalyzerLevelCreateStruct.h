//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLevelCreateStruct
//* .File Name       : $Workfile:   SVAnalyzerLevelCreateStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVToolLevelCreateStruct.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVAnalyzerLevelCreateStruct
// -----------------------------------------------------------------------------
// .Description : Structure for SVM_CREATE... messages to objects below the
//				: Analyzer level
//              : Supports structure identifying by structure name
////////////////////////////////////////////////////////////////////////////////
struct SVAnalyzerLevelCreateStruct : public SVToolLevelCreateStruct
{
	SVAnalyzerLevelCreateStruct();
	SVAnalyzerLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS );

	virtual ~SVAnalyzerLevelCreateStruct();

	SVObjectInfoStruct AnalyzerObjectInfo;
};

