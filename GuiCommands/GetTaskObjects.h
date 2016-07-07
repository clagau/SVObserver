//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the task objects from a task object list.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes


namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetTaskObjects: public boost::noncopyable
		{
			GetTaskObjects(const GUID& rObjectID)
				: m_InstanceID(rObjectID) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				m_list.clear();
				
				SvOi::ITaskObjectListClass* pObject = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(m_InstanceID));

				if (pObject)
				{
					m_list = pObject->GetTaskObjectList();
				}
				else
				{
					hr = E_NOINTERFACE;
				}
				return hr;
			}
			bool empty() const { return false; }
			const SvUl::NameGuidList& TaskObjects() const { return m_list; }

		private:
			SvUl::NameGuidList m_list;
			GUID m_InstanceID;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
