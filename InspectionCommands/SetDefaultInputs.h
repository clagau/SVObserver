//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Resetting an Object.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	template <typename TaskObject> // Note: Only Inspection and ToolSet have the SetDefaultInputs method.
	struct SetDefaultInputs
	{
	public:
		SetDefaultInputs(const SetDefaultInputs&) = delete;
		SetDefaultInputs& operator=(const SetDefaultInputs&) = delete;

		SetDefaultInputs(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {};

		virtual ~SetDefaultInputs() {};

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
