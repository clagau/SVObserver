//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ObjectInfo.h
/// All Rights Reserved 
//*****************************************************************************
/// ObjectInfo holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
//******************************************************************************
#pragma once
#include "Definitions\SVObjectTypeInfoStruct.h"
namespace SvOi
{
/// struct  holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
struct ObjectInfo
{
	ObjectInfo(LPCSTR pName, const GUID& rGuid, SvDef::SVObjectTypeEnum type,
		SvDef::SVObjectSubTypeEnum subtype, bool valid):
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
	SvDef::SVObjectTypeEnum ObjectType = SvDef::SVNotSetObjectType;
	SvDef::SVObjectSubTypeEnum ObjectSubType = SvDef::SVNotSetSubObjectType;
	GUID guid = GUID_NULL;
	bool isValid = false;
	std::string DisplayName;
};
typedef std::vector<ObjectInfo> ObjectInfoVector;


}