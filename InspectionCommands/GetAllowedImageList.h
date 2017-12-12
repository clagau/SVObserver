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

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetAllowedImageList
	{
	public:
		GetAllowedImageList(const GetAllowedImageList&) = delete;
		GetAllowedImageList& operator=(const GetAllowedImageList&) = delete;

		GetAllowedImageList(const SVGUID& rObjectID, const SvDef::SVObjectTypeInfoStruct& typeInfo, const SVGUID& rTaskObjectID, bool OnlyAboveImages = true)
			: m_pCommand(OnlyAboveImages ? new GetAvailableObjects(rObjectID, typeInfo, IsObjectFromPriorTool(rTaskObjectID)) : new GetAvailableObjects(rObjectID, typeInfo))
			, m_TaskObjectID(rTaskObjectID)
		{};

		virtual ~GetAllowedImageList() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = (nullptr != m_pCommand) ? m_pCommand->Execute() : E_FAIL;
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
		typedef std::shared_ptr<GetAvailableObjects> CommandPtr;
		CommandPtr m_pCommand;
		SVGUID m_TaskObjectID;
		SvDef::StringVector m_specialImageList;
	};
} //namespace SvCmd
