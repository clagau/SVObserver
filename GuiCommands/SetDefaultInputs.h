//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Resetting an Object.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace SvCmd
{
	template <typename TaskObject> // Note: Only Inspection and ToolSet have the SetDefaultInputs method.
	struct SetDefaultInputs : public boost::noncopyable
	{
		SetDefaultInputs(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			TaskObject* pTask = dynamic_cast<TaskObject *>(SvOi::getObject(m_InstanceID));
			if (pTask)
			{
				pTask->SetDefaultInputs();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }

	private:
		SVGUID m_InstanceID;
	};
} //namespace SvCmd
