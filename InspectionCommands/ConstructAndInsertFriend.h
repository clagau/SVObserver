//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// \file ConstructAndInsertFriend.h
// All Rights Reserved
//*****************************************************************************
// This is the Command for construct an object and insert it as friend.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectBuilder.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class ConstructAndInsertFriend
	{
	public:
		ConstructAndInsertFriend(const ConstructAndInsertFriend&) = delete;
		ConstructAndInsertFriend& operator=(const ConstructAndInsertFriend&) = delete;

		ConstructAndInsertFriend(const SVGUID& rParentID, const SVGUID& rClassID, LPCTSTR objectName, const SVGUID& rAddPreGuid = GUID_NULL)
			: m_InstanceID(rParentID), m_ClassID(rClassID), m_objectName(objectName), m_addPreGuid(rAddPreGuid) {};

		virtual ~ConstructAndInsertFriend() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			return SVObjectBuilder::CreateFriendObject(m_ClassID, GUID_NULL, m_objectName, m_InstanceID, m_addPreGuid);
		}
		bool empty() const { return false; }

	private:
		SVGUID m_InstanceID;
		SVGUID m_ClassID;
		SVGUID m_addPreGuid;
		LPCTSTR m_objectName;
	};
} //namespace SvCmd
