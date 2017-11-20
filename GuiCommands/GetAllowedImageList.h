//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Allowed Image List.
//******************************************************************************
#pragma once

#pragma region Includes
#include "AllowedImageFunc.h"
#include "GetAvailableObjects.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetAllowedImageList : public boost::noncopyable
	{
		GetAllowedImageList(const GUID& rObjectID, const SvDef::SVObjectTypeInfoStruct& typeInfo, const GUID& rTaskObjectID, SvDef::SVObjectSubTypeEnum subType, bool bAllowColor = false )
		: m_command(rObjectID, typeInfo, AllowedImageFunc(rTaskObjectID, subType, bAllowColor)) 
		, m_TaskObjectID(rTaskObjectID) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = m_command.Execute();
			const SvOi::ITaskObject* pObject = dynamic_cast<const SvOi::ITaskObject*>(SvOi::getObject(m_TaskObjectID));
			if (nullptr != pObject)
			{
				pObject->getSpecialImageList(m_specialImageList);
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvUl::NameGuidList& AvailableObjects() const { return m_command.AvailableObjects(); }
		const SvDef::StringVector& AvailableSpecialImages() const { return m_specialImageList; }

	private:
		GetAvailableObjects m_command;
		GUID m_TaskObjectID;
		SvDef::StringVector m_specialImageList;
	};
} //namespace SvCmd
