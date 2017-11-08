//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's Instance ID from the Dotted Name.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IObjectManager.h"

#pragma endregion Includes

namespace SvCmd
{
	struct GetInstanceIDFromDottedName : public boost::noncopyable
	{
		GetInstanceIDFromDottedName(const std::string& rName) : m_Name(rName), m_InstanceID(GUID_NULL) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IObjectClass* pObject = SvOi::getObjectByDottedName(m_Name.c_str());
			if (pObject)
			{
				m_InstanceID = pObject->GetUniqueObjectID();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const GUID& GetInstanceID() const { return m_InstanceID; }

	private:
		std::string m_Name;
		GUID m_InstanceID;
	};
} //namespace SvCmd
