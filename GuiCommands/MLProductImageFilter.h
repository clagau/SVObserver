//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLProductImageFilter.h
/// All Rights Reserved 
//*****************************************************************************
/// MLProductImageFilter is the function object used to filter the output list 
/// for the object selector
//******************************************************************************
#pragma once
#pragma region Includes
#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes
#include "../ObjectInterfaces/ISVImage.h"

namespace SvCmd
{
	class MLProductImageFilter
	{
	public:
		bool operator()(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex) const
		{
			bool Result(false);
			if (nullptr != pObject)
			{
				
				const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (pObject);
				{
					Result = SvOi::SV_PUBLISH_RESULT_IMAGE == (pObject->ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE);
				}
				if (Result == false)
				{
					const SvOi::ISVImage*  pImageObject = dynamic_cast<const SvOi::ISVImage*> (pObject);
					if (pImageObject && (pImageObject->GetImageType() == SvOi::SVImageTypeMain || pImageObject->GetImageType() == SvOi::SVImageTypeRGBMain))
					{
						Result = true;
					}
				
				}
			}

			return Result;
		}
	};
} //namespace SvCmd



