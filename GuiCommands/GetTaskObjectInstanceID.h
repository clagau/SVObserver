//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the TaskObect Instance GUID.
//******************************************************************************
#pragma once
#include <boost/noncopyable.hpp>
#include <comdef.h>
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/ObjectDefines.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetTaskObjectInstanceID: public boost::noncopyable
		{
			GetTaskObjectInstanceID(const SVGUID& ownerID, const SVObjectTypeInfoStruct& info) : m_ownerID(ownerID), m_info(info), m_instanceID(GUID_NULL) {}
	
			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectClass* pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage(m_ownerID, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&m_info)));
				if (pObject)
				{
					m_instanceID = pObject->GetUniqueObjectID();
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			const GUID& GetInstanceID() const { return m_instanceID; }

		private:
			GUID m_ownerID;
			SVObjectTypeInfoStruct m_info;
			GUID m_instanceID;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
