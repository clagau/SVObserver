//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Objects to filter the object selector items
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/SVO-Enum.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

namespace SvCmd
{
class AttributesAllowedFilter
{
public:
	AttributesAllowedFilter(const AttributesAllowedFilter&) = default;
	AttributesAllowedFilter& operator=(const AttributesAllowedFilter&) = delete;

	AttributesAllowedFilter() {};

	virtual ~AttributesAllowedFilter() {};

	bool operator()(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ) const
	{
		bool Result(false);

		if (SvPb::noAttributes == Attribute)
		{
			Result = true;
		}
		else
		{
			if (nullptr != pObject)
			{
				Result = (Attribute == (pObject->ObjectAttributesAllowed() & Attribute));
			}
		}
		return Result;
	}
};

class AttributesSetFilter
{
public:
	AttributesSetFilter(const AttributesSetFilter&) = default;
	AttributesSetFilter& operator=(const AttributesSetFilter&) = delete;

	AttributesSetFilter() {};

	virtual ~AttributesSetFilter() {};

	bool operator()(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex) const
	{
		bool Result(false);

		if (SvPb::noAttributes == Attribute)
		{
			Result = true;
		}
		else
		{
			if (nullptr != pObject)
			{
				const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (pObject);
				if (nullptr != pValueObject && pValueObject->isArray())
				{
					Result = (Attribute == (pObject->ObjectAttributesSet(ArrayIndex) & Attribute));
				}
				else
				{
					Result = (Attribute == (pObject->ObjectAttributesSet() & Attribute));
				}
			}
		}
		return Result;
	}
};


class ExcludeSameLineageSelectorFilter
{
public:
	ExcludeSameLineageSelectorFilter(const ExcludeSameLineageSelectorFilter&) = default;
	ExcludeSameLineageSelectorFilter& operator=(const ExcludeSameLineageSelectorFilter&) = delete;

	ExcludeSameLineageSelectorFilter() {};
	explicit ExcludeSameLineageSelectorFilter(std::vector<std::string>&& rExcludePath)
		: m_excludePath(rExcludePath)
	{
	}

	virtual ~ExcludeSameLineageSelectorFilter() {};
private:
	std::vector<std::string> m_excludePath;

	static bool IsSameLineage(const std::string& name, const std::vector <std::string>& excludedList)
	{
		return std::any_of(excludedList.cbegin(), excludedList.cend(), [name](const auto& excludedPath)
			{
				size_t len = excludedPath.size();
				//After the exclude path name must have a '.' for it to match
				if (len > 0 && name.size() > len && '.' == name[len])
				{
					return (0 == name.substr(0, len).compare(excludedPath));
				}
				return false;
			});
	}

public:
	bool ExcludeSameLineageSelectorFilter::operator()(const SvOi::IObjectClass* pObject, unsigned int attributeMask, int ) const
	{
		bool bRetVal = false;
		if (nullptr != pObject)
		{
			const UINT attributesAllowed = pObject->ObjectAttributesAllowed();
			const std::string& name = pObject->GetCompleteName();
			bRetVal = (attributeMask == (attributesAllowed & attributeMask) && !IsSameLineage(name, m_excludePath));
		}
		return bRetVal;
	}
};
} //namespace SvCmd
