//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Embedded ValueObject Values.
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ITaskObject.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	template<typename Items>
	class TaskObjectSetEmbeddedValues
	{
	public:
		TaskObjectSetEmbeddedValues(const TaskObjectSetEmbeddedValues&) = delete;
		TaskObjectSetEmbeddedValues& operator=(const TaskObjectSetEmbeddedValues&) = delete;

		TaskObjectSetEmbeddedValues(const GUID& rTaskID, const Items& rItems, bool shouldSet = true) 
			: m_rTaskID{ rTaskID }, m_rItems{ rItems }, m_shouldSet{ shouldSet } {};

		virtual ~TaskObjectSetEmbeddedValues() {};
	
		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			SvOi::SetValueStructVector ChangedValues;
			SvOi::SetValueStructVector ChangedDefaultValues;
			for (Items::const_iterator it = m_rItems.begin();it != m_rItems.end() && S_OK == hr;++it)
			{
				SvOi::IObjectClass* pObject = SvOi::getObject(it->second.GetObjectID());
				SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
				if (nullptr != pValueObject )
				{
					if (it->second.isModified())
					{
						ChangedValues.push_back(SvOi::SetValueStruct{ pValueObject, it->second.GetValue(), it->second.GetArrayIndex() });
					}
					if (it->second.isDefaultModified())
					{
						// Default value has no array index
						ChangedDefaultValues.push_back(SvOi::SetValueStruct{ pValueObject, it->second.GetDefaultValue(), -1 });
					}
				}
				else
				{
					hr = E_POINTER;
				}
			}
			
			SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(m_rTaskID));
			if (nullptr != pTaskObject)
			{
				//! Set the values
				SvStl::MessageContainerVector messages = pTaskObject->validateAndSetEmbeddedValues(ChangedValues, m_shouldSet);
				if (0 == messages.size())
				{
					//clear modified flags
					for (auto rEntry : ChangedValues)
					{
						SvOi::IObjectClass *pObject = dynamic_cast<SvOi::IObjectClass*>(rEntry.m_pValueObject);
						if (nullptr != pObject)
						{
							const SVGUID& objectGuid = pObject->GetUniqueObjectID();
							Items::const_iterator itemIt = std::find_if(m_rItems.begin(), m_rItems.end(), [objectGuid](const Items::value_type item)->bool
							{
								return item.second.GetObjectID() == objectGuid;
							});
							if (m_rItems.end() != itemIt)
							{
								itemIt->second.ClearModified();
								itemIt->second.ClearArrayIndex();
							}
						}
					}

					//Set the default values
					messages = pTaskObject->setEmbeddedDefaultValues(ChangedDefaultValues);
					if (0 == messages.size())
					{
						//clear default modified flags
						for (auto rEntry : ChangedDefaultValues)
						{
							SvOi::IObjectClass *pObject = dynamic_cast<SvOi::IObjectClass*>(rEntry.m_pValueObject);
							if (nullptr != pObject)
							{
								const SVGUID& objectGuid = pObject->GetUniqueObjectID();
								Items::const_iterator itemIt = std::find_if(m_rItems.begin(), m_rItems.end(), [objectGuid](const Items::value_type item)->bool
								{
									return item.second.GetObjectID() == objectGuid;
								});
								if (m_rItems.end() != itemIt)
								{
									itemIt->second.ClearDefaultModified();
								}
							}
						}
					}
					else
					{
						m_messages.insert(m_messages.end(), messages.begin(), messages.end());
						hr = E_FAIL;
					}
				}
				else
				{
					m_messages.insert(m_messages.end(), messages.begin(), messages.end());
					hr = E_FAIL;
				}
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}

		SvStl::MessageContainerVector getErrorMessages() {return m_messages; };

	private:
		const GUID& m_rTaskID;
		const Items& m_rItems;
		bool m_shouldSet;
		SvStl::MessageContainerVector m_messages;
	};
} //namespace SvCmd
