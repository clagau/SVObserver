//******************************************************************************
/// COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
#include "SVProtoBuf/SVO-Enum.h"
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes

namespace SvUl
{
	typedef std::pair<std::string, uint32_t> NameObjectIdPair;
	typedef std::vector<NameObjectIdPair> NameObjectIdList;
	typedef std::pair<std::string, SvPb::ClassIdEnum> NameClassIdPair;
	typedef std::vector<NameClassIdPair> NameClassIdList;

	template<typename T>
	uint32_t FindObjectId(const T& rList, const std::string& rName)
	{
		uint32_t retID = SvDef::InvalidObjectId;
		typename T::const_iterator it = find_if(rList.begin(), rList.end(), [rName](const auto item)->bool
		{
			return rName == item.first;
		});
		if (it != rList.end())
		{
			retID = it->second;
		}
		return retID;
	}
} // namespace SvUl
