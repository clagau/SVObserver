//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeSelectorFilter is the function object used to filter the output list 
/// for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <boost\assign\list_of.hpp>
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\IValueObject.h"

#pragma endregion Includes

namespace SvCmd
{
class RangeSelectorFilter
{
public:
	RangeSelectorFilter(const RangeSelectorFilter&) = delete;
	RangeSelectorFilter& operator=(const RangeSelectorFilter&) = delete;

	RangeSelectorFilter() {};

	virtual ~RangeSelectorFilter() {};
private:
	std::string m_excludePath;

	static bool IsSameLineage(const std::string& name, const std::string& excludedPath)
	{
		bool bSame = false;
		size_t len = excludedPath.size();
		//After the exclude path name must have a '.' for it to match
		if (len > 0 && name.size() > len && '.' == name[len])
		{
			bSame = (0 == name.substr(0, len).compare(excludedPath));
		}
		return bSame;
	}

public:
	RangeSelectorFilter(const std::string& rExcludePath)
		: m_excludePath(rExcludePath)
	{
	}

	bool RangeSelectorFilter::operator()(const SvOi::IObjectClass* pObject, unsigned int attributeMask, int ArrayIndex) const
	{
		bool bRetVal = false;
		auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(pObject);
		if (nullptr != pValueObject)
		{
			DWORD type = pValueObject->GetType();
			const UINT attributesAllowed = pObject->ObjectAttributesAllowed();
			const std::string& name = pObject->GetCompleteName();
			constexpr std::array<DWORD, 11> filter {VT_I2, VT_I4, VT_I8, VT_R4, VT_R8, VT_UI2, VT_UI4, VT_UI8, VT_INT, VT_UINT, VT_BOOL};
			bRetVal = (attributeMask == (attributesAllowed & attributeMask) &&
				!IsSameLineage(name, m_excludePath) &&
				filter.end() != std::find(filter.begin(), filter.end(), type));
		}
		return bRetVal;
	}
};
} //namespace SvCmd
