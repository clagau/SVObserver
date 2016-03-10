//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeSelectorFilter is the function object used to filter the output list 
/// for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include <set>
#include <boost\assign\list_of.hpp>
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "SVUtilityLibrary\SVString.h"

#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		class RangeSelectorFilter
		{
		private:
			std::set<SVObjectTypeEnum> m_filter;
			SVString m_excludePath;

			static bool IsExcluded(SVObjectTypeEnum type, const std::set<SVObjectTypeEnum>& rFilter)
			{
				return (rFilter.size() > 0 && rFilter.end() == rFilter.find(type));
			}

			static bool HasAttribute(unsigned int attributesMask, unsigned int attributes)
			{
				return attributesMask == (attributes & attributesMask);
			}

			static bool IsSameLineage(const SVString& name, const SVString& excludedPath)
			{
				bool bSame = false; 
				size_t len = excludedPath.size();
				if (len > 0)
				{
					bSame = (0 == name.Left(len).Compare(excludedPath));
				}
				return bSame;
			}

			static bool IsAllowed(SVObjectTypeEnum type, UINT attributesFilter, UINT attributesAllowed, const SVString& name, const std::set<SVObjectTypeEnum>& filter, const SVString& excludePath)
			{
				return (HasAttribute(attributesFilter, attributesAllowed) && !IsSameLineage(name, excludePath) && !IsExcluded(type, filter));
			}

		public:
			RangeSelectorFilter(const SVString& rExcludePath)
			: m_excludePath(rExcludePath)
			, m_filter(boost::assign::list_of
					(SVDWordValueObjectType)
					(SVLongValueObjectType)
					(SVDoubleValueObjectType)
					(SVBoolValueObjectType)
					(SVPointValueObjectType)
					(SVByteValueObjectType))
			{
			}

			bool RangeSelectorFilter::operator()(const SvOi::IObjectClass* pObject, unsigned int attributeMask, int ArrayIndex) const
			{
				bool bRetVal = false;
				if (pObject)
				{
					const SVObjectTypeEnum& type = pObject->GetObjectType();
					const UINT attributesAllowed = pObject->ObjectAttributesAllowed();
					const SVString& name = pObject->GetCompleteName();
					bRetVal = IsAllowed(type, attributeMask, attributesAllowed, name, m_filter, m_excludePath);
				}
				return bRetVal;
			}
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;