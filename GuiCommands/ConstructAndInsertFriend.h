//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// \file ConstructAndInsertFriend.h
// All Rights Reserved
//*****************************************************************************
// This is the Command for construct an object and insert it as friend.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "SVObjectLibrary\SVObjectReference.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct ConstructAndInsertFriend : public boost::noncopyable
		{
			ConstructAndInsertFriend(const GUID& rParentID, const GUID& rClassID, LPCTSTR objectName) 
				: m_InstanceID(rParentID), m_ClassID(rClassID), m_objectName(objectName) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				return SVObjectBuilder::CreateFriendObject(m_ClassID, SV_GUID_NULL, m_objectName, m_InstanceID);
			}
			bool empty() const { return false; }

		private:
			GUID m_InstanceID;
			GUID m_ClassID;
			LPCTSTR m_objectName;
		};
	}
}