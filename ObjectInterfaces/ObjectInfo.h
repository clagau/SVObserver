//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ObjectInfo.h
/// All Rights Reserved 
//*****************************************************************************
/// ObjectInfo holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
//******************************************************************************
#pragma once
#include "SVProtobuf/SVO-Enum.h"
#include "Definitions/ObjectDefines.h"
namespace SvOi
{
/// struct  holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
struct ObjectInfo
{
	ObjectInfo(LPCSTR pName, uint32_t objectId, SvPb::SVObjectTypeEnum type,
		SvPb::SVObjectSubTypeEnum subtype, bool valid):
		DisplayName(pName),
		m_objectId(objectId),
		ObjectType(type),
		ObjectSubType(subtype),
		isValid(valid)
	{
		;
	}
	ObjectInfo()
	{
		;
	}
	SvPb::SVObjectTypeEnum ObjectType = SvPb::SVNotSetObjectType;
	SvPb::SVObjectSubTypeEnum ObjectSubType = SvPb::SVNotSetSubObjectType;
	uint32_t m_objectId = SvDef::InvalidObjectId;
	bool isValid = false;
	std::string DisplayName;
};
typedef std::vector<ObjectInfo> ObjectInfoVector;


}