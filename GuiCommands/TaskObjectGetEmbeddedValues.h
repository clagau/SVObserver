//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Embedded ValueObject Values.
//******************************************************************************

#pragma once

#include <boost/noncopyable.hpp>
#include "ObjectInterfaces\IObjectClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVOGUI/BoundValue.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
//#include "SVValueObjectClass.h" // commented out while SVValueObjectClass resides in the SVObserver project

namespace Seidenader
{ 
	namespace GuiCommand
	{
		// while SVValueObjectClass resides in the SVObserver project, use the ValueObject template parameter
		template<typename Items, typename ValueObject>
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
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();
				for (Items::iterator it = m_Items.begin();S_OK == hr && it != m_Items.end();++it)
				{
					SVObjectTypeInfoStruct objectInfo;
					objectInfo.EmbeddedID = it->second.GetEmbeddedID();

					bool bFound = false;

					// try owner, then parent(s), until found or there is no parent, return E_POINTER if not found
					GUID parentID = m_ownerID;
					while (!bFound && GUID_NULL != parentID)
					{
						ValueObject* pValueObject = reinterpret_cast<ValueObject*>(::SVSendMessage(parentID, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)));
						if (pValueObject)
						{
							_variant_t value;
							pValueObject->GetValue(*(value.GetAddress()));
							it->second = Items::mapped_type(pValueObject->GetEmbeddedID(), pValueObject->GetUniqueObjectID(), value);
							bFound = true;
						}
						else
						{
							SvOi::IObjectClass* pParent = SVObjectManagerClass::Instance().GetObject(parentID);
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