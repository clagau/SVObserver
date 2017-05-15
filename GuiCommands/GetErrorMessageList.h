//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// Get a list of error messages from this object.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes


namespace SvCmd
{
	struct GetErrorMessageList : public boost::noncopyable
	{
		GetErrorMessageList(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_InstanceID));
			if (pTask)
			{
				m_MessageList = pTask->getErrorMessages();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvStl::MessageContainerVector& GetMessageList() const { return m_MessageList; }

	private:
		SVGUID m_InstanceID;
		SvStl::MessageContainerVector m_MessageList;
	};
} //namespace SvCmd
