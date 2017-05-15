//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Mask Data.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IMask.h"
#pragma endregion Includes

namespace SvCmd
{
	struct SetMaskData : public boost::noncopyable
	{
		SetMaskData(const GUID& rObjectID, HGLOBAL hGlobal) 
			: m_InstanceID(rObjectID), m_hGlobal(hGlobal) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				pObject->SetMaskData(m_hGlobal);
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
		HGLOBAL m_hGlobal;
	};
} //namespace SvCmd
