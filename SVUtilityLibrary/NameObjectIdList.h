//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
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
	typedef std::map<std::string, NameObjectIdPair> InputNameObjectIdPairList;

	template<typename T>
	uint32_t FindObjectId(const T& rList, const std::string& rName)
	{
		uint32_t retID = SvDef::InvalidObjectId;
		T::const_iterator it = find_if(rList.begin(), rList.end(), [rName](const auto item)->bool
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
