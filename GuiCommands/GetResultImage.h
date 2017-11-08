//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Result(s) Image name/guid.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetResultImage: public boost::noncopyable
	{
		GetResultImage(const GUID& rObjectID) : m_InstanceID(rObjectID) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SVObjectTypeInfoStruct imageObjectInfo(SVImageObjectType);
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
		GUID m_InstanceID;
	};
} //namespace SvCmd
