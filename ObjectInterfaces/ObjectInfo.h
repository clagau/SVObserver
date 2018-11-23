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
namespace SvOi
{
/// struct  holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
struct ObjectInfo
{
	ObjectInfo(LPCSTR pName, const GUID& rGuid, SvPb::SVObjectTypeEnum type,
		SvPb::SVObjectSubTypeEnum subtype, bool valid):
		DisplayName(pName),
		guid(rGuid),
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
	GUID guid = GUID_NULL;
	bool isValid = false;
	std::string DisplayName;
};
typedef std::vector<ObjectInfo> ObjectInfoVector;


}