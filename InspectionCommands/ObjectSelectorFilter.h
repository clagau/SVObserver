//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Objects to filter the object selector items
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtobuf/SVO-Enum.h"
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


class RangeSelectorFilter
{
public:
	RangeSelectorFilter(const RangeSelectorFilter&) = default;
	RangeSelectorFilter& operator=(const RangeSelectorFilter&) = delete;

	RangeSelectorFilter() {};
	explicit RangeSelectorFilter(const std::string& rExcludePath)
		: m_excludePath(rExcludePath)
	{
	}

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
	bool RangeSelectorFilter::operator()(const SvOi::IObjectClass* pObject, unsigned int attributeMask, int ArrayIndex) const
	{
		bool bRetVal = false;
		auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(pObject);
		if (nullptr != pValueObject)
		{
			DWORD type = pValueObject->GetType();
			const UINT attributesAllowed = pObject->ObjectAttributesAllowed();
			const std::string& name = pObject->GetCompleteName();
			//@Todo[MEC][8.20] [10.09.2019] EXTERNAL TOOL to allow output from External tool insert VT_VARIANT ..
			constexpr std::array<DWORD, 11> filter {VT_I2, VT_I4, VT_I8, VT_R4, VT_R8, VT_UI2, VT_UI4, VT_UI8, VT_INT, VT_UINT, VT_BOOL};
			bRetVal = (attributeMask == (attributesAllowed & attributeMask) &&
					   !IsSameLineage(name, m_excludePath) &&
					   filter.end() != std::find(filter.begin(), filter.end(), type));
		}
		return bRetVal;
	}
};

class MLRejectValueFilter
{
public:
	MLRejectValueFilter(const MLRejectValueFilter&) = default;
	MLRejectValueFilter& operator=(const MLRejectValueFilter&) = delete;

	MLRejectValueFilter() = default;

	virtual ~MLRejectValueFilter() {};

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
				Result = IsAllowedType(pObject->GetObjectSubType());
			}
		}
		return Result;
	}
private:
	static bool IsAllowedType(SvPb::SVObjectSubTypeEnum type)
	{
		switch (type)
		{
			case  	SvPb::SVDWordValueObjectType: ///Fall throu 
			case    SvPb::SVLongValueObjectType:
			case 	SvPb::SVDoubleValueObjectType:
			case	SvPb::SVBoolValueObjectType:
			case	SvPb::SVByteValueObjectType:
			case 	SvPb::SVInt64ValueObjectType:
			case	SvPb::DoubleSortValueObjectType:
				return true;
			default:
				return false;
		}

	}

};
class TableObjectSelector
{
public:
	TableObjectSelector(const TableObjectSelector&) = default;
	TableObjectSelector& operator=(const TableObjectSelector&) = delete;

	TableObjectSelector() {};

	virtual ~TableObjectSelector() {};

	bool operator()(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex) const
	{
		if (!pObject)
		{
			return false;
		}
		if (pObject->GetObjectType() != SvPb::TableObjectType)
		{
			return false;
		}
		
		unsigned int  AttributesAllowed = pObject->ObjectAttributesAllowed();
		if (0 == (SvPb::taskObject & AttributesAllowed))
		{
			return false;
		}
		
		return true;
	}
};

} //namespace SvCmd
