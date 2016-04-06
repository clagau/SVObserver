//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcessResetStruct
//* .File Name       : $Workfile:   SVInspectionProcessResetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:02:48  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

struct SVInspectionProcessResetStruct
{
	SVInspectionProcessResetStruct();
	SVInspectionProcessResetStruct( const SVInspectionProcessResetStruct &p_rsvObject );

	const SVInspectionProcessResetStruct &operator=( const SVInspectionProcessResetStruct &p_rsvObject );

	SVResetItemEnum SetResetData( SVResetItemEnum p_eResetItem, SVObjectClass *p_psvObject );

	SVResetItemEnum m_eResetItem;

	SVStdSetSVObjectClassPtr m_svObjectSet;
};

typedef std::map< SVToolClass *, SVInspectionProcessResetStruct > SVStdMapSVToolClassPtrSVInspectionProcessResetStruct;

#include "SVInspectionProcessResetStruct.inl"

