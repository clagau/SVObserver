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

		TaskObjectSetEmbeddedValues(const Items& items, bool shouldSet = true) : m_Items(items), m_shouldSet(shouldSet) {};

		virtual ~TaskObjectSetEmbeddedValues() {};
	
		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			//fill parentMap
			std::map<SVGUID, SvOi::SetValueObjectPairVector> parentMap;
			for (Items::const_iterator it = m_Items.begin();it != m_Items.end() && S_OK == hr;++it)
			{
				SvOi::IObjectClass* pObject = SvOi::getObject(it->second.GetObjectID());
				SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
				if (nullptr != pValueObject )
				{
					if (!it->second.isReadOnly() && it->second.isModified())
					{
						SVGUID parentId = pObject->GetParentID();
						_variant_t value = boost::any_cast<_variant_t> ( it->second.GetValue() );
						parentMap[parentId].push_back( SvOi::SetValueObjectPair( pValueObject, value ) );
					}
				}
				else
				{
					hr = E_POINTER;
				}
			}
			
			//validation and set embedded value over parent object
			for( std::map<SVGUID, SvOi::SetValueObjectPairVector>::iterator it = parentMap.begin(); it != parentMap.end() && S_OK == hr; ++it )
			{
				SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(it->first));
				if (nullptr != pTaskObject)
				{
					SvStl::MessageContainerVector messages = pTaskObject->validateAndSetEmmeddedValues( it->second, m_shouldSet );
					if (0 < messages.size() )
					{
						m_messages.insert(m_messages.end(), messages.begin(), messages.end());
						hr = E_FAIL;
					}
					else
					{
						//clear modified flags
						for (SvOi::SetValueObjectPairVector::iterator vecIt = it->second.begin(); it->second.end() != vecIt; ++vecIt)
						{
							SvOi::IObjectClass *pObject = dynamic_cast<SvOi::IObjectClass*>(vecIt->first);
							if( nullptr != pObject )
							{
								const SVGUID& objectGuid = pObject->GetUniqueObjectID();
								Items::const_iterator itemIt = std::find_if( m_Items.begin(), m_Items.end(), [objectGuid](const Items::value_type item)->bool
								{
									return item.second.GetObjectID() == objectGuid;
								});
								if (m_Items.end() != itemIt)
								{
									itemIt->second.ClearModified();
								}
							}
						}
					}
				}
				else
				{
					hr = E_POINTER;
				}
			}
			return hr;
		}

		SvStl::MessageContainerVector getErrorMessages() {return m_messages; };

	private:
		const Items& m_Items;
		bool m_shouldSet;
		SvStl::MessageContainerVector m_messages;
	};
} //namespace SvCmd
