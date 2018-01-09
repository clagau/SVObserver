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
	SvPB::UUID setGuidToMessage(const GUID& rGuid)
	{
		SvPB::UUID UUId;
		__int64 temp = 0;
		memcpy(&temp, &rGuid.Data1, sizeof(__int64));
		UUId.set_part1(temp);
		memcpy(&temp, rGuid.Data4, sizeof(rGuid.Data4));
		UUId.set_part2(temp);

		return UUId;
	}

	GUID getGuidFromMessage(const SvPB::UUID& rUUId)
	{
		GUID guid = GUID_NULL;
		__int64 temp = rUUId.part1();
		memcpy(&guid.Data1, &temp, sizeof(__int64));
		temp = rUUId.part2();
		memcpy(guid.Data4, &temp, sizeof(guid.Data4));
		return guid;
	}
} //namespace SvPB