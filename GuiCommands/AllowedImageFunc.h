//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Object to filter available images for selection
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/IToolSet.h"
#pragma endregion Includes

namespace SvCmd
{
	class AllowedImageFunc
	{
	private:
		GUID m_TaskObjectID;
		SvDef::SVObjectSubTypeEnum m_subType;
		bool m_bAllowColor;

		bool IsHidden(const SvOi::IObjectClass* pObject) const
		{
			return (pObject->ObjectAttributesAllowed() & SvDef::SV_HIDDEN) ? true : false;
		}

		bool HasOneBand(const SvOi::ISVImage* pImage) const
		{
			long bandNumber = pImage->getBands();
			return (1 == bandNumber);
		}

		bool IsObjectCurrentTask(const SvOi::IObjectClass* pObject) const
		{
			bool bRetVal = true;
			const SvOi::IObjectClass* pImageOwnerTool = pObject->GetAncestorInterface(SvDef::SVToolObjectType);

			if (nullptr != pImageOwnerTool)
			{
				GUID ownerID = pImageOwnerTool->GetUniqueObjectID();
				const SvOi::IToolSet* pToolSet = dynamic_cast<const SvOi::IToolSet*> (pImageOwnerTool->GetAncestorInterface(SvDef::SVToolSetObjectType));
				if (nullptr != pToolSet)
				{
					if (ownerID == m_TaskObjectID) // stop at this tool...
					{
						bRetVal = false;
					}
				}
			}
			return bRetVal;
		}

	public:
		AllowedImageFunc(const GUID& rTaskObjectID, SvDef::SVObjectSubTypeEnum subType, bool bAllowColor)
		: m_TaskObjectID(rTaskObjectID)
		, m_subType(subType)
		, m_bAllowColor(bAllowColor)
		{
		}

		bool operator()(const SvOi::IObjectClass* pObject, bool& bStop) const
		{
			bool bUseImage = false;
			const SvOi::ISVImage* pImage = dynamic_cast<const SvOi::ISVImage*> (pObject);

			if (nullptr != pImage)
			{
				if (SvDef::SVToolImageObjectType != m_subType)
				{
					// Ensure only image sources which are produced by tools above the current tool...
					bStop = !IsObjectCurrentTask(pObject);
					bUseImage = !bStop && !IsHidden(pObject) && (m_bAllowColor || HasOneBand(pImage));
				}
				else
				{
					// The ImageTool does not have the requirement of using only image sources which are produced by tools above the current tool.
					bUseImage = !IsHidden(pObject) && HasOneBand(pImage);
				}
			}
			return bUseImage;
		}
	};
} //namespace SvCmd