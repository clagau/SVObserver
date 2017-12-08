//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Object's Type Info.
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetObjectTypeInfo
	{
	public:
		GetObjectTypeInfo(const GetObjectTypeInfo&) = delete;
		GetObjectTypeInfo& operator=(const GetObjectTypeInfo&) = delete;

		GetObjectTypeInfo(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {};

		virtual ~GetObjectTypeInfo() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
			if (pObject)
			{
				m_TypeInfo = SvDef::SVObjectTypeInfoStruct(pObject->GetObjectType(), pObject->GetObjectSubType());
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvDef::SVObjectTypeInfoStruct& GetTypeInfo() const { return m_TypeInfo; }

	private:
		SVGUID m_InstanceID;
		SvDef::SVObjectTypeInfoStruct m_TypeInfo;
	};
} //namespace SvCmd
