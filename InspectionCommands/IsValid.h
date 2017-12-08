//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the validity of an Object.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class IsValid
	{
	public:
		IsValid(const IsValid&) = delete;
		IsValid& operator=(const IsValid&) = delete;

		IsValid(const SVGUID& rObjectID) : m_InstanceID(rObjectID), m_bValid(false) {};


		virtual ~IsValid() {};
		
		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				m_bValid = pObject->IsObjectValid();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		bool isValid() const { return m_bValid; }

	private:
		SVGUID m_InstanceID;
		bool m_bValid;
	};
} //namespace SvCmd
