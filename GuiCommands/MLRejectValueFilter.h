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
#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

namespace SvCmd
{
	class MLRejectValueFilter
	{
	public:
		bool operator()(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex) const
		{
			bool Result(false);

			if (SvOi::SV_NO_ATTRIBUTES == Attribute)
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
		static bool IsAllowedType(SVObjectSubTypeEnum type)
		{
			switch (type)
			{
			case  	SVDWordValueObjectType: ///Fall throu 
			case    SVLongValueObjectType:
			case 	SVDoubleValueObjectType:
			case	SVBoolValueObjectType:
			case	SVByteValueObjectType:
			case 	SVInt64ValueObjectType:
			case	DoubleSortValueObjectType:
				return true;
			default:
				return false;
			}

		}

	};
} //namespace SvCmd

