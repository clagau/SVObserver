//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

class SVObjectClass;

namespace SvIe
{


struct SVClassInfoStruct
{
	SVClassInfoStruct() = default;
	SVClassInfoStruct( const SVClassInfoStruct& rRhs ) = default;
	virtual ~SVClassInfoStruct() = default;
	SVClassInfoStruct& operator=( const SVClassInfoStruct& rRhs ) = default;

	// Construct Object and set given ClassName...
	SVObjectClass* Construct();

	SvDef::SVObjectTypeInfoStruct m_ObjectTypeInfo;
	std::string m_ClassName;
	SvPb::ClassIdEnum m_ClassId = SvPb::NoObjectClassId;

	// Must be in the same order, like the class defines its input interface!!!
	SvDef::SVObjectTypeInfoVector m_DesiredInputVector;
};

typedef std::vector<SVClassInfoStruct> SVClassInfoStructVector;

} //namespace SvIe
