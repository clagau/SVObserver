//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's Name.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetObjectName : public boost::noncopyable
	{
		GetObjectName(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
			if (pObject)
			{
				m_Name = pObject->GetName();
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
		SVString m_Name;
	};
} //namespace SvCmd
