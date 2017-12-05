//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Allowed Image List.
//******************************************************************************
#pragma once

#pragma region Includes
#include "GetAvailableObjects.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetAllowedImageList : public boost::noncopyable
	{
		GetAllowedImageList(const SVGUID& rObjectID, const SvDef::SVObjectTypeInfoStruct& typeInfo, const SVGUID& rTaskObjectID, bool OnlyAboveImages=true)
		: m_pCommand(OnlyAboveImages ? new GetAvailableObjects(rObjectID, typeInfo, IsObjectFromPriorTool(rTaskObjectID)) : new GetAvailableObjects(rObjectID, typeInfo))
		, m_TaskObjectID(rTaskObjectID)
		{}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = !m_pCommand.empty() ? m_pCommand->Execute() : E_FAIL;
			const SvOi::ITaskObject* pObject = dynamic_cast<const SvOi::ITaskObject*>(SvOi::getObject(m_TaskObjectID));
			if (nullptr != pObject)
			{
				pObject->getSpecialImageList(m_specialImageList);
			}
			return hr;
		}
		//bool empty() const { return false; }
		const SvUl::NameGuidList& AvailableObjects() const { return m_pCommand->AvailableObjects(); }
		const SvDef::StringVector& AvailableSpecialImages() const { return m_specialImageList; }

	private:
		typedef SVSharedPtr<GetAvailableObjects> CommandPtr;
		CommandPtr m_pCommand;
		SVGUID m_TaskObjectID;
		SvDef::StringVector m_specialImageList;
	};
} //namespace SvCmd
