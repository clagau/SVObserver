//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Embedded ValueObject Values.
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

namespace SvCmd
{
	template<typename Items>
	class TaskObjectGetEmbeddedValues 
	{
	public:
		TaskObjectGetEmbeddedValues(const TaskObjectGetEmbeddedValues&) = delete;
		TaskObjectGetEmbeddedValues& operator=(const TaskObjectGetEmbeddedValues&) = delete;

		TaskObjectGetEmbeddedValues(const SVGUID& rInspectionID, const SVGUID& rTaskID) : m_Items{ Items{rInspectionID , rTaskID} }, m_rTaskID(rTaskID) {};

		virtual ~TaskObjectGetEmbeddedValues() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT Result = S_OK;

			SvOi::ITaskObject* pTaskObj = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(m_rTaskID));
			SVGuidVector EmbeddedVector = pTaskObj->getEmbeddedList();

			for (auto const& rEntry : EmbeddedVector)
			{
				SvOi::IObjectClass* pObject = SvOi::getObject(rEntry);
				SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
				if (nullptr != pValueObject)
				{
					_variant_t DefaultValue{ pValueObject->getDefaultValue() };
					_variant_t Value;
					HRESULT tmpHr = pValueObject->getValue(Value);
					if (S_OK == tmpHr)
					{
						m_Items[pObject->GetEmbeddedID()] = Items::mapped_type{ pObject->GetEmbeddedID(), rEntry, Value, DefaultValue };
					}
					else if (E_BOUNDS == tmpHr)
					{
						Value.Clear();
						m_Items[pObject->GetEmbeddedID()] = Items::mapped_type{ pObject->GetEmbeddedID(), rEntry, Value, DefaultValue };
					}
					else
					{
						Result = tmpHr;
						return Result;
					}
				}
			}
			
			return Result;
		}

		bool empty() const { return false; }
		
		const Items& GetItems() const { return m_Items; }

	private:
		SVGUID m_rTaskID;
		Items m_Items;
	};
} //namespace SvCmd
