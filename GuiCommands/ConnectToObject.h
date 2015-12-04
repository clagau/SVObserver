//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Connecting an Object.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct ConnectToObject: public boost::noncopyable
		{
			ConnectToObject(const GUID& rObjectID, const SVString& rInputName, const GUID& rNewID, SVObjectTypeEnum objectType) 
				: m_InstanceID(rObjectID), m_InputName(rInputName), m_NewID(rNewID), m_objectType(objectType) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_InstanceID));
				if (pObject)
				{
					if (SVImageObjectType == m_objectType)
					{
						hr = pObject->ConnectToImage(m_InputName, m_NewID);
					}
					else
					{
						hr = E_INVALIDARG;
					}
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
		private:
			GUID m_InstanceID;
			SVString m_InputName;
			GUID m_NewID;
			SVObjectTypeEnum m_objectType;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
