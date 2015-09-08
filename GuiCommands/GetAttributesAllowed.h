//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's AttributeAllowed state.
//******************************************************************************
#pragma once

#include <boost/noncopyable.hpp>
#include "ObjectInterfaces/IObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVString.h"

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetAttributesAllowed : public boost::noncopyable
		{
			GetAttributesAllowed(const GUID& rObjectID) : m_InstanceID(rObjectID), m_attributes(0) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();
				SvOi::IObjectClass* pObject = rMgr.GetObject(m_InstanceID);
				if (pObject)
				{
					m_attributes = pObject->ObjectAttributesAllowed();
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			UINT AttributesAllowed() const { return m_attributes; }

		private:
			UINT m_attributes;
			GUID m_InstanceID;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
