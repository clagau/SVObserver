//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Resetting an Object.
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class ResetObject
	{
	public:
		ResetObject(const ResetObject&) = delete;
		ResetObject& operator=(const ResetObject&) = delete;

		ResetObject(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {};

		virtual ~ResetObject() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			m_messages.clear();
			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);

			bool Result = false;
			if (nullptr != pObject)
			{
				Result = pObject->resetAllObjects(&m_messages);
			}
			if (!Result)
			{
				hr = E_FAIL;
			}
			return hr;
		}
		bool empty() const { return false; }
		SvStl::MessageContainerVector getErrorMessages() {return m_messages; };

	private:
		SVGUID m_InstanceID;
		SvStl::MessageContainerVector m_messages;
	};
} //namespace SvCmd
