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

class SVObjectClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVObjectLevelCreateStruct
// -----------------------------------------------------------------------------
// .Description : Base structure for SVM_CREATE... messages
//              : Supports structure identifying by structure name
////////////////////////////////////////////////////////////////////////////////
struct SVObjectLevelCreateStruct
{
	explicit SVObjectLevelCreateStruct(SVObjectClass& rOwner);

	virtual ~SVObjectLevelCreateStruct();

	SVObjectClass& m_rOwner;
	SVObjectClass* m_pInspection = nullptr;
	SVObjectClass* m_pTool = nullptr;
	SVObjectClass* m_pAnalyzer = nullptr;
};

