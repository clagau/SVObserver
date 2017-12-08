//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Connecting an Object.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\ITaskObject.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class ConnectToObject
	{
	public:
		ConnectToObject(const ConnectToObject&) = delete;
		ConnectToObject& operator=(const ConnectToObject&) = delete;

		ConnectToObject(const SVGUID& rObjectID, const std::string& rInputName, const SVGUID& rNewID, SvDef::SVObjectTypeEnum objectType)
			: m_InstanceID(rObjectID), m_InputName(rInputName), m_NewID(rNewID), m_objectType(objectType) {};

		virtual ~ConnectToObject() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				hr = pObject->ConnectToObject(m_InputName, m_NewID, m_objectType);
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
		std::string m_InputName;
		SVGUID m_NewID;
		SvDef::SVObjectTypeEnum m_objectType;
	};
} //namespace SvCmd
