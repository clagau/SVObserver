//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the creatable Objects based on the type.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetCreatableObjects: public boost::noncopyable
	{
		GetCreatableObjects(const GUID& rObjectID, const SVObjectTypeInfoStruct& typeInfo) : m_InstanceID(rObjectID), m_typeInfo(typeInfo) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);

			if (pObject)
			{
				m_list = pObject->GetCreatableObjects(m_typeInfo);
			}
			else
			{
				hr = E_NOINTERFACE;
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvUl::NameGuidList& AvailableObjects() const { return m_list; }

	private:
		SVObjectTypeInfoStruct m_typeInfo;
		SvUl::NameGuidList m_list;
		GUID m_InstanceID;
	};
} //namespace SvCmd
