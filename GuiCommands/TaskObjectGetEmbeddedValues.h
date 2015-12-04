//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Embedded ValueObject Values.
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\IValueObject.h"
#include "SVOGUI/BoundValue.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader
{ 
	namespace GuiCommand
	{
		template<typename Items>
		struct TaskObjectGetEmbeddedValues : public boost::noncopyable
		{
			TaskObjectGetEmbeddedValues(const SVGUID& ownerID, const Items& items) : m_ownerID(ownerID), m_Items(items) {}
			virtual ~TaskObjectGetEmbeddedValues() {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				for (Items::iterator it = m_Items.begin();S_OK == hr && it != m_Items.end();++it)
				{
					SVObjectTypeInfoStruct objectInfo;
					objectInfo.EmbeddedID = it->second.GetEmbeddedID();

					bool bFound = false;

					// try owner, then parent(s), until found or there is no parent, return E_POINTER if not found
					GUID parentID = m_ownerID;
					while (!bFound && GUID_NULL != parentID)
					{
						DWORD flags = SVM_GETFIRST_OBJECT;

						const SVObjectTypeInfoStruct& ownerInfo = it->second.GetOwnerInfo();
						if (SVNotSetObjectType != ownerInfo.ObjectType)
						{
							flags &= ~SVM_NOTIFY_FRIENDS;
						}
						SvOi::IValueObject* pValueObject = SvOi::FindValueObject(parentID, objectInfo);
						if (pValueObject)
						{
							bFound = true;

							_variant_t value;
							hr = pValueObject->GetValue(value);
							if (S_OK == hr)
							{
								SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass *>(pValueObject);
								it->second = Items::mapped_type(objectInfo.EmbeddedID, pObject->GetUniqueObjectID(), value, ownerInfo, it->second.isReadOnly());
							}
						}
						else
						{
							SvOi::IObjectClass* pParent = SvOi::getObject(parentID);
							if (pParent)
							{
								parentID = pParent->GetParentID();
							}
							else
							{
								parentID = GUID_NULL; 
							}
						}
					}
					if (!bFound)
					{
						hr = E_POINTER;
					}
				}
				return hr;
			}
			bool empty() const { return false; }
			const Items& GetItems() const { return m_Items; }

		private:
			SVGUID m_ownerID;
			Items m_Items;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;