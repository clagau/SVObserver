//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's Dependencies.
//******************************************************************************
#pragma once

//Moved to precompiled header: #include <boost/noncopyable.hpp>
#include "ObjectInterfaces/ITaskObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVString.h"

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetDependencies : public boost::noncopyable
		{
			GetDependencies(const GUID& rObjectID, bool bImagesOnly, SVObjectTypeEnum objectType) : m_InstanceID(rObjectID), m_bImagesOnly(bImagesOnly), m_objectType(objectType) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();
				SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(rMgr.GetObject(m_InstanceID));
				if (pObject)
				{
					m_dependencies = pObject->GetDependents(m_bImagesOnly, m_objectType);
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			const SvOi::DependencyList& Dependencies() const { return m_dependencies; }

		private:
			SvOi::DependencyList m_dependencies;
			GUID m_InstanceID;
			bool m_bImagesOnly;
			SVObjectTypeEnum m_objectType;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
