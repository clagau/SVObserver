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
		static bool IsAllowedType(SvDef::SVObjectSubTypeEnum type)
		{
			switch (type)
			{
			case  	SvDef::SVDWordValueObjectType: ///Fall throu 
			case    SvDef::SVLongValueObjectType:
			case 	SvDef::SVDoubleValueObjectType:
			case	SvDef::SVBoolValueObjectType:
			case	SvDef::SVByteValueObjectType:
			case 	SvDef::SVInt64ValueObjectType:
			case	SvDef::DoubleSortValueObjectType:
				return true;
			default:
				return false;
			}

		}

	};
} //namespace SvCmd

