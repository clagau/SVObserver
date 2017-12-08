//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's Name.
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
	class GetObjectName
	{
	public:
		GetObjectName(const GetObjectName&) = delete;
		GetObjectName& operator=(const GetObjectName&) = delete;

		GetObjectName(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {};

		virtual ~GetObjectName() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
			if (pObject)
			{
				m_Name = pObject->GetName();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const std::string& GetName() const { return m_Name; }

	private:
		SVGUID m_InstanceID;
		std::string m_Name;
	};
} //namespace SvCmd
