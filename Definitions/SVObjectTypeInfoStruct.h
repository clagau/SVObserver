//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// enums and struct for object types
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtobuf/SVO-Enum.h"
#pragma endregion Includes

namespace SvDef
{
		struct SVObjectTypeInfoStruct
		{
			SVObjectTypeInfoStruct( SvPb::SVObjectTypeEnum ObjectType= SvPb::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum SubType= SvPb::SVNotSetSubObjectType, const GUID& rEmbeddedID=GUID_NULL)
				: ObjectType(ObjectType ), SubType(SubType ), EmbeddedID( rEmbeddedID ) {};

			//******************************************************************************
			// Data Element(s):
			//******************************************************************************

			SvPb::SVObjectTypeEnum		ObjectType;			// Main Type
			SvPb::SVObjectSubTypeEnum		SubType;		// Sub Type  

			// Embedded ID, if any.
			GUID				EmbeddedID;
		};

		typedef std::vector<SVObjectTypeInfoStruct> SVObjectTypeInfoVector;
} //namespace SvDef

