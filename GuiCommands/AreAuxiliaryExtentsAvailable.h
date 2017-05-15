//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for determining if the Auxiliary Extents are available.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

namespace SvCmd
{
	struct AreAuxiliaryExtentsAvailable : public boost::noncopyable
	{
		AreAuxiliaryExtentsAvailable(const GUID& rObjectID) : m_InstanceID(rObjectID), m_bAvailable(false) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(m_InstanceID));
			if (pTool)
			{
				m_bAvailable = pTool->areAuxExtentsAvailable();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		bool AuxExtentsAvailable() const { return m_bAvailable; }

	private:
		bool m_bAvailable;
		GUID m_InstanceID;
	};
} //namespace SvCmd
