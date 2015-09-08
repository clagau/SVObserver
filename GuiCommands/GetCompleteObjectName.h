//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's Complete Name.
// if the type is not set then just get this objcts conplete name
// otherwise get the complete name from the object that matches the type
//******************************************************************************
#pragma once

#include <boost/noncopyable.hpp>
#include "ObjectInterfaces/IObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetCompleteObjectName : public boost::noncopyable
		{
			GetCompleteObjectName(const SVGUID& rInstanceID, SVObjectTypeEnum type = SVNotSetObjectType) : m_InstanceID(rInstanceID), m_type(type) {}

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
					if (pObject->GetObjectType() == m_type)
					{
						m_Name = pObject->GetCompleteName();
					}
					else
					{
						pObject = pObject->GetAncestorInterface(m_type);
						if (pObject)
						{
							m_Name = pObject->GetCompleteName();
						}
						else
						{
							hr = E_POINTER;
						}
					}
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			const SVString& GetName() const { return m_Name; }

		private:
			SVGUID m_InstanceID;
			SVObjectTypeEnum m_type;
			SVString m_Name;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
