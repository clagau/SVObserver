//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Enumerations from the SVEnumerateValueObject.
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"
#include "ObjectInterfaces\NameValueList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class ValueObjectGetEnums
	{
	public:
		ValueObjectGetEnums(const ValueObjectGetEnums&) = delete;
		ValueObjectGetEnums& operator=(const ValueObjectGetEnums&) = delete;

		ValueObjectGetEnums(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {};

		virtual ~ValueObjectGetEnums() {};

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
		SVGUID m_InstanceID;
		SvOi::NameValueList m_list;
	};
}
