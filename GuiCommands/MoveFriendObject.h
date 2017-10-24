//*****************************************************************************
// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for move a friend in the friend list of the owner.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "SVObjectLibrary\SVObjectReference.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#pragma endregion Includes

namespace SvCmd
{
	struct MoveFriendObject : public boost::noncopyable
	{
		MoveFriendObject(const GUID& rParentID, const GUID& rObjectID, const GUID& rMovePreGuid = SV_GUID_NULL)
			: m_InstanceID(rParentID), m_ObjectID(rObjectID), m_movePreGuid(rMovePreGuid) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
			if (nullptr != pObject)
			{
				pObject->moveFriendObject(m_ObjectID, m_movePreGuid);
				return S_OK;
			}
			else
			{
				return E_FAIL;
			}
		}
		bool empty() const { return false; }

	private:
		GUID m_InstanceID;
		GUID m_ObjectID;
		GUID m_movePreGuid;
	};
} //namespace SvCmd

