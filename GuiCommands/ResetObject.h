//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Resetting an Object.
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
		struct ResetObject : public boost::noncopyable
		{
			ResetObject(const SVGUID& rInstanceID, bool bNotifyFriends) : m_InstanceID(rInstanceID), m_bNotifyFriends(bNotifyFriends) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				DWORD flags = SVM_RESET_ALL_OBJECTS;
				if (!m_bNotifyFriends)
				{
					flags &= ~SVM_NOTIFY_FRIENDS;
				}
				DWORD_PTR dwRet = ::SVSendMessage(m_InstanceID, flags, 0, 0);
				if (SVMR_SUCCESS != dwRet)
				{
					hr = E_FAIL;
				}
				return hr;
			}
			bool empty() const { return false; }

		private:
			SVGUID m_InstanceID;
			bool m_bNotifyFriends;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
