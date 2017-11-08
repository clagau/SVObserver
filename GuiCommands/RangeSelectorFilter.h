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

#pragma endregion Includes

namespace SvCmd
{
	class RangeSelectorFilter
	{
	private:
		std::set<SVObjectSubTypeEnum> m_filter;
		std::string m_excludePath;

		static bool IsExcluded(SVObjectSubTypeEnum type, const std::set<SVObjectSubTypeEnum>& rFilter)
		{
			return (rFilter.size() > 0 && rFilter.end() == rFilter.find(type));
		}

		static bool HasAttribute(unsigned int attributesMask, unsigned int attributes)
		{
			return attributesMask == (attributes & attributesMask);
		}

		static bool IsSameLineage(const std::string& name, const std::string& excludedPath)
		{
			bool bSame = false; 
			size_t len = excludedPath.size();
			if (len > 0)
			{
				bSame = (0 == name.substr(0,len).compare(excludedPath));
			}
			return bSame;
		}

		static bool IsAllowed(SVObjectSubTypeEnum type, UINT attributesFilter, UINT attributesAllowed, const std::string& name, const std::set<SVObjectSubTypeEnum>& filter, const std::string& excludePath)
		{
			return (HasAttribute(attributesFilter, attributesAllowed) && !IsSameLineage(name, excludePath) && !IsExcluded(type, filter));
		}

	public:
		RangeSelectorFilter(const std::string& rExcludePath)
		: m_excludePath(rExcludePath)
			, m_filter((boost::assign::list_of
				(SVDWordValueObjectType)
				(SVLongValueObjectType)
				(SVDoubleValueObjectType)
				(DoubleSortValueObjectType)
				(SVBoolValueObjectType)
				(SVPointValueObjectType)
				(SVByteValueObjectType)).convert_to_container<std::set<SVObjectSubTypeEnum>>())

		{
		}

		bool RangeSelectorFilter::operator()(const SvOi::IObjectClass* pObject, unsigned int attributeMask, int ArrayIndex) const
		{
			bool bRetVal = false;
			if (pObject)
			{
				const SVObjectSubTypeEnum& type = pObject->GetObjectSubType();
				const UINT attributesAllowed = pObject->ObjectAttributesAllowed();
				const std::string& name = pObject->GetCompleteName();
				bRetVal = IsAllowed(type, attributeMask, attributesAllowed, name, m_filter, m_excludePath);
			}
			return bRetVal;
		}
	};
} //namespace SvCmd
