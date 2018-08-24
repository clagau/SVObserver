//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// \file GetInputs.h
// All Rights Reserved
//*****************************************************************************
// This is the Command for getting the inputs with name and Connected Objects based on the type.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectClass.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetInputs
	{
	public:
		GetInputs(const GetInputs&) = delete;
		GetInputs& operator=(const GetInputs&) = delete;

		/// \param rObjectID [in] Object Id to the task object.
		/// \param typeInfo [in] Type of the requested inputs. SvDef::SVNotSetObjectType return all inputs
		/// \param objectTypeToInclude [in] Object type until the name of the connected object will set. SvDef::SVNotSetObjectType means only object name and e.g. SvDef::SVToolSetObjectType means "Tool Set.Window Tool....". This parameter will not used for image objects.
		/// \param shouldExcludeFirstObjectType [in] Remove first object name. (If objectTypeToInclude == SvDef::SVNotsetObjectType this parameter will not used) e.g. SvDef::SVToolSetObjectType means "Window Tool....". This parameter will not used for image objects.
		/// \param func [in]
		GetInputs(const SVGUID& rObjectID, const SvDef::SVObjectTypeInfoStruct& typeInfo = SvDef::SVObjectTypeInfoStruct(SvDef::SVNotSetObjectType), SvDef::SVObjectTypeEnum objectTypeToInclude = SvDef::SVNotSetObjectType, bool shouldExcludeFirstObjectName = false)
			: m_InstanceID(rObjectID), m_typeInfo(typeInfo), m_objectTypeToInclude(objectTypeToInclude), m_shouldExcludeFirstObjectName(shouldExcludeFirstObjectName) {};


		virtual ~GetInputs() {}
		
		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_InstanceID));
			if (nullptr != pTaskObject)
			{
				pTaskObject->GetInputs(m_list, m_typeInfo, m_objectTypeToInclude, m_shouldExcludeFirstObjectName);
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvUl::InputNameGuidPairList& ConnectedObjects() const { return m_list; }

	private:
		SvDef::SVObjectTypeInfoStruct m_typeInfo;
		SvUl::InputNameGuidPairList m_list;
		SvDef::SVObjectTypeEnum m_objectTypeToInclude;
		bool m_shouldExcludeFirstObjectName;
		SVGUID m_InstanceID;
	};
} //namespace SvCmd
