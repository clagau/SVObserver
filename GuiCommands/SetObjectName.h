//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Object's Name.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct SetObjectName : public boost::noncopyable
		{
			SetObjectName(const SVGUID& rInstanceID, LPCTSTR name) : m_InstanceID(rInstanceID), m_Name(name) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
				if (pObject)
				{
					pObject->SetName(m_Name);
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }

		private:
			SVGUID m_InstanceID;
			LPCTSTR m_Name;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
