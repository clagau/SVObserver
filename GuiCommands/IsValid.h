//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the validity of an Object.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct IsValid: public boost::noncopyable
		{
			IsValid(const GUID& rObjectID) 
				: m_InstanceID(rObjectID), m_bValid(false) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_InstanceID));
				if (pObject)
				{
					m_bValid = pObject->IsObjectValid();
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			bool isValid() const { return m_bValid; }

		private:
			GUID m_InstanceID;
			bool m_bValid;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
