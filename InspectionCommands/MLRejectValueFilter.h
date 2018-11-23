//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLRejectValueFilter.h
/// All Rights Reserved 
//*****************************************************************************
// MLRejectValueFilter is the function object used to filter the output list 
/// for the object selector
//******************************************************************************
#pragma once
#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class MLRejectValueFilter
	{
	public:
		MLRejectValueFilter(const MLRejectValueFilter&) = delete;
		MLRejectValueFilter& operator=(const MLRejectValueFilter&) = delete;

		MLRejectValueFilter() = default;

		virtual ~MLRejectValueFilter() {};

		bool operator()(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex) const
		{
			bool Result(false);

			if (SvDef::SV_NO_ATTRIBUTES == Attribute)
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
} //namespace SvCmd

