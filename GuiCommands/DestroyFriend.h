//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// \file ConstructAndInsertFriend.h
// All Rights Reserved
//*****************************************************************************
// This is the Command for destroy an friend.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct DestroyFriend : public boost::noncopyable
		{
			enum FlagEnum
			{
				Flag_None = 0,
				Flag_SetDefaultInputs = 1,
				Flag_ResetInspection = 2,
				Flag_SetDefaultInputs_And_ResetInspection = 3
			};

			/// \param rTaskObjectListID [in] GUID of the task-object-list-object.
			/// \param rObjectID [in] GUID of the friend which should be destroyed.
			/// \param flags [in] Define if after destroying object the default inputs are set and/or inspection are reset.
			DestroyFriend(const GUID& rTaskObjectListID, const GUID& rFriendId, FlagEnum flag) 
				: m_InstanceID(rTaskObjectListID)
				, m_FriendID( rFriendId )
				, m_flag(flag)
			{}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::ITaskObjectListClass *pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(m_InstanceID));
				SvOi::IObjectClass *pObject = SvOi::getObject(m_FriendID);
				if( nullptr != pTaskObjectList && nullptr != pObject )
				{
					DWORD flag = 0;
					switch(m_flag)
					{
					case Flag_SetDefaultInputs:
						flag = SVMFSetDefaultInputs;
						break;
					case Flag_ResetInspection:
						flag = SVMFResetInspection;
						break;
					case Flag_SetDefaultInputs_And_ResetInspection:
						flag = SVMFSetDefaultInputs | SVMFResetInspection;
						break;
					default:
						flag = 0;
						break;
					}

					if (!pTaskObjectList->DestroyFriendObject( *pObject, flag ))
					{
						hr = SvOi::Err_10033_DestroyFriendObject_Failed;
					}
				}
				else
				{
					hr = SvOi::Err_10032_DestroyFriendObject_InvalidParameter;
				}
				return hr;
			}
			bool empty() const { return false; }

		private:
			GUID m_InstanceID;
			GUID m_FriendID;
			FlagEnum m_flag;
		};
	}
}