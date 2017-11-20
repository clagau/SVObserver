//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Connected Objects based on the type.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectClass.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IObjectManager.h"

#include "SVUtilityLibrary\NameGuidList.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetConnectedObjects: public boost::noncopyable
	{
		GetConnectedObjects(const GUID& rObjectID, const SvDef::SVObjectTypeInfoStruct& typeInfo, int maxRequested) : m_InstanceID(rObjectID), m_typeInfo(typeInfo), m_maxRequested(maxRequested) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
			if (nullptr != pObject)
			{
				if (SvDef::SVImageObjectType == m_typeInfo.ObjectType)
				{
					SvOi::ITaskObject* pTaskObject  = dynamic_cast<SvOi::ITaskObject *>(pObject);
					if (nullptr != pTaskObject)
					{
						pTaskObject->GetConnectedImages(m_list, m_maxRequested);
					}
					else
					{
						hr = E_POINTER;
					}
				}
				else
				{
					hr = E_INVALIDARG;
				}
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
		GUID m_InstanceID;
		int m_maxRequested;
	};
} //namespace SvCmd
