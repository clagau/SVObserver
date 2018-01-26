//*****************************************************************************
// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for move a friend in the friend list of the owner.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class MoveFriendObject
	{
	public:
		MoveFriendObject(const MoveFriendObject&) = delete;
		MoveFriendObject& operator=(const MoveFriendObject&) = delete;

		MoveFriendObject(const SVGUID& rParentID, const SVGUID& rObjectID, const SVGUID& rMovePreGuid = GUID_NULL)
			: m_InstanceID(rParentID), m_ObjectID(rObjectID), m_movePreGuid(rMovePreGuid) {};
		
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
		SVGUID m_InstanceID;
		SVGUID m_ObjectID;
		SVGUID m_movePreGuid;
	};
} //namespace SvCmd

