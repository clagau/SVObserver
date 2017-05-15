//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Enumerations from the SVEnumerateValueObject.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"
#include "ObjectInterfaces\NameValueList.h"
#pragma endregion Includes

namespace SvCmd
{
	struct ValueObjectGetEnums : public boost::noncopyable
	{
		ValueObjectGetEnums(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {}
		virtual ~ValueObjectGetEnums() {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			SvOi::IEnumerateValueObject* pValueObject = dynamic_cast<SvOi::IEnumerateValueObject *>(SvOi::getObject(m_InstanceID));
			if (pValueObject)
			{
				m_list = pValueObject->GetEnumList();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}

		const SvOi::NameValueList& GetEnumList() const { return m_list; }

	private:
		GUID m_InstanceID;
		SvOi::NameValueList m_list;
	};
}
