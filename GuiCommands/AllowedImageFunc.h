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

			bool IsHidden(SvOi::IObjectClass* pObject) const
			{
				return (pObject->ObjectAttributesAllowed() & SV_HIDDEN) ? true : false;
			}

			bool HasOneBand(SvOi::ISVImage* pImage) const
			{
				long bandNumber = pImage->getBands();
				return (1 == bandNumber);
			}

			bool IsOwnerGageTool(SvOi::ISVImage* pImage) const
			{
				bool bRetVal = false;
				// Gets the owner from the image info, why not the SVObject owner info struct?
				// Are they the same or out of sync?
				SvOi::IObjectClass* pImageOwnerTool = pImage->getOwner(); 

				if (pImageOwnerTool)
				{
					SVObjectSubTypeEnum subType = pImageOwnerTool->GetObjectSubType();
					bRetVal = (SVGageToolObjectType == subType);
				}
				return bRetVal;
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
			AllowedImageFunc(const GUID& rTaskObjectID, SVObjectSubTypeEnum subType)
			: m_TaskObjectID(rTaskObjectID)
			, m_subType(subType)
			{
			}
	
			bool operator()(SvOi::IObjectClass* pObject, bool& bStop) const
			{
				bool bUseImage = false;
				SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage *>(pObject);

				// Ensure only image sources which are produced by tools above the current tool....
				if (pImage)
				{
					if (SVToolImageObjectType != m_subType)
					{
						bStop = !IsObjectCurrentTask(pObject);
						bUseImage = !bStop && !IsHidden(pObject) && !IsOwnerGageTool(pImage) && HasOneBand(pImage);
					}
					else
					{
						bUseImage = !IsHidden(pObject) && !IsOwnerGageTool(pImage) && HasOneBand(pImage);
					}
				}
				return bUseImage;
			}
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;