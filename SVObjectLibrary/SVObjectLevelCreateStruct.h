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
#include "SVObjectClass.h"
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

	virtual ~SVObjectLevelCreateStruct();

	SVObjectInfoStruct OwnerObjectInfo;
	SVObjectClass* m_pInspection;
	SVObjectClass* m_pTool;
	SVObjectClass* m_pAnalyzer;
};

