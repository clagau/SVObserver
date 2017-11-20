//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassInfoStruct.h
//* .File Name       : $Workfile:   SVClassInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:34  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

class SVObjectClass;

struct SVClassInfoStruct
{
	SVClassInfoStruct();
	SVClassInfoStruct( const SVClassInfoStruct& rRhs );
	virtual ~SVClassInfoStruct();
	SVClassInfoStruct operator=( const SVClassInfoStruct& rRhs );

	// Construct Object and set given ClassName...
	SVObjectClass* Construct();

	SvDef::SVObjectTypeInfoStruct m_ObjectTypeInfo;
	std::string m_ClassName;
	SVGUID m_ClassId;

	// Must be in the same order, like the class defines its input interface!!!
	SvDef::SVObjectTypeInfoVector m_DesiredInputVector;
};

typedef std::vector<SVClassInfoStruct> SVClassInfoStructVector;

