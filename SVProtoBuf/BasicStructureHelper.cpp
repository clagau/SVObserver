//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file BasicStructureHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "BasicStructureHelper.h"
#pragma endregion Includes

namespace SvPB
{

void SetGuidInProtoBytes(std::string  *pString, const GUID& guid)
{
	if (pString)
	{
		pString->assign(reinterpret_cast<const char*>(&guid), sizeof(GUID));
	}
}

void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid)
{
	if (strguid.size() == sizeof(GUID))
	{
		rGuid = *(reinterpret_cast<const GUID*>(strguid.data()));
	}
}

GUID GetGuidFromProtoBytes(const std::string& strguid)
{
	GUID guid = GUID_NULL;
	GetGuidFromProtoBytes(strguid, guid);
	return guid;
}
} //namespace SvPB