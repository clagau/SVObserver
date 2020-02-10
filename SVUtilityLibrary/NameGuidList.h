//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <guiddef.h>
#include "SVGUID.h"
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes
namespace SvUl
{
	typedef std::pair<std::string, SVGUID> NameGuidPair;
	typedef std::vector<NameGuidPair> NameGuidList;
	typedef std::pair<std::string, SvPb::ClassIdEnum> NameClassIdPair;
	typedef std::vector<NameClassIdPair> NameClassIdList;
	typedef std::map<std::string, NameGuidPair> InputNameGuidPairList;

	template<typename T>
	SVGUID FindGUID(const T& rList, const std::string& rName)
	{
		SVGUID retID = GUID_NULL;
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
