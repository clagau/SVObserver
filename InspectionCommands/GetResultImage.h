//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Result(s) Image name/guid.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetResultImage
	{
	public:
		GetResultImage(const GetResultImage&) = delete;
		GetResultImage& operator=(const GetResultImage&) = delete;

		GetResultImage(const SVGUID& rObjectID) : m_InstanceID(rObjectID) {};

		virtual ~GetResultImage() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvDef::SVObjectTypeInfoStruct imageObjectInfo(SvDef::SVImageObjectType);
			SvOi::ISVImage* pImage = SvOi::FindImageObject(m_InstanceID, imageObjectInfo);
			if (pImage)
			{
				std::string name = pImage->getDisplayedName();
				SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass *>(pImage);
				m_list.push_back(std::make_pair(name, pObject->GetUniqueObjectID()));
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvUl::NameGuidList& ResultImages() const { return m_list; }

	private:
		SvUl::NameGuidList m_list;
		SVGUID m_InstanceID;
	};
} //namespace SvCmd
