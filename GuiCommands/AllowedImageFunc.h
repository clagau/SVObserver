//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Object to filter available images for selection
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/IToolSet.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		class AllowedImageFunc
		{
		private:
			GUID m_TaskObjectID;
			SVObjectSubTypeEnum m_subType;
			bool m_bAllowColor;

			bool IsHidden(SvOi::IObjectClass* pObject) const
			{
				return (pObject->ObjectAttributesAllowed() & SV_HIDDEN) ? true : false;
			}

			bool HasOneBand(SvOi::ISVImage* pImage) const
			{
				long bandNumber = pImage->getBands();
				return (1 == bandNumber);
			}

			bool IsObjectCurrentTask(SvOi::IObjectClass* pObject) const
			{
				bool bRetVal = true;
				SvOi::IObjectClass* pImageOwnerTool = pObject->GetAncestorInterface(SVToolObjectType);

				if (nullptr != pImageOwnerTool)
				{
					GUID ownerID = pImageOwnerTool->GetUniqueObjectID();
					SvOi::IToolSet* pToolSet = dynamic_cast<SvOi::IToolSet *>(pImageOwnerTool->GetAncestorInterface(SVToolSetObjectType));
					if (pToolSet)
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
			AllowedImageFunc(const GUID& rTaskObjectID, SVObjectSubTypeEnum subType, bool bAllowColor)
			: m_TaskObjectID(rTaskObjectID)
			, m_subType(subType)
			, m_bAllowColor(bAllowColor)
			{
			}
	
			bool operator()(SvOi::IObjectClass* pObject, bool& bStop) const
			{
				bool bUseImage = false;
				SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage *>(pObject);

				if (pImage)
				{
					if (SVToolImageObjectType != m_subType)
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
	}
}

namespace GuiCmd = Seidenader::GuiCommand;