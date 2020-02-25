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
			SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum ObjectType = SvPb::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVNotSetSubObjectType, SvPb::EmbeddedIdEnum embeddedID = SvPb::NoEmbeddedId)
				: m_ObjectType(ObjectType ), m_SubType(SubType ), m_EmbeddedID( embeddedID ) {};

			//******************************************************************************
			// Data Element(s):
			//******************************************************************************

			SvPb::SVObjectTypeEnum		m_ObjectType;			// Main Type
			SvPb::SVObjectSubTypeEnum	m_SubType;		// Sub Type  

			// Embedded ID, if any.
			SvPb::EmbeddedIdEnum		m_EmbeddedID;
		};

		typedef std::vector<SVObjectTypeInfoStruct> SVObjectTypeInfoVector;
} //namespace SvDef

