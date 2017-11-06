//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Available Objects based on the type.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectClass.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#pragma endregion Includes

namespace SvCmd
{
	// This function object return always true if nullptr != object
	struct IsValidObject 
	{
		bool operator()(SvOi::IObjectClass* pObject, bool& bStop) const
		{
			return (nullptr != pObject);
		}
	};

	class IsObjectFromPriorTool
	{
	public:
		IsObjectFromPriorTool(const GUID& rTaskObjectID)
			: m_TaskObjectID(rTaskObjectID)
		{
		}

		bool operator()(SvOi::IObjectClass* pObject, bool& bStop) const
		{
			// Ensure only image sources which are produced by tools above the current tool...
			bStop = IsObjectCurrentTask(pObject);
			return !bStop;
		}

	private:
		bool IsObjectCurrentTask(SvOi::IObjectClass* pObject) const
		{
			bool bRetVal = false;
			if (nullptr != pObject)
			{
				SvOi::IObjectClass* pOwnerTool = pObject->GetAncestorInterface(SVToolObjectType);
				if (nullptr != pOwnerTool)
				{
					GUID ownerID = pOwnerTool->GetUniqueObjectID();
					if (ownerID == m_TaskObjectID) // stop at this tool...
					{
						bRetVal = true;
					}
				}
			}
			return bRetVal;
		}

	private:
		GUID m_TaskObjectID;
	};

	typedef boost::function<bool (SvOi::IObjectClass*, bool& bStop)> IsAllowedFunc; 
	struct GetAvailableObjects: public boost::noncopyable
	{
		/// \param rObjectID [in] Object Id of the object from which level the available objects will search for.
		/// \param typeInfo [in] Type of the available objects
		/// \param objectTypeToInclude [in] Object type until the name of the available object will set. SVNotSetObjectType means only object name and e.g. SVToolSetObjectType means "Tool Set.Window Tool....". This parameter will not used for image objects.
		/// \param func [in]
		GetAvailableObjects(const GUID& rObjectID, const SVObjectTypeInfoStruct& typeInfo, IsAllowedFunc func = IsValidObject(), SVObjectTypeEnum objectTypeToInclude = SVNotSetObjectType )
			: m_InstanceID(rObjectID), m_typeInfo(typeInfo), IsAllowed(func), m_objectTypeToInclude(objectTypeToInclude) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SVGetObjectDequeByTypeVisitor visitor(m_typeInfo);
			SvOi::visitElements(visitor, m_InstanceID);

			bool bStop = false;
			for (SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator it = visitor.GetObjects().begin(); !bStop && it != visitor.GetObjects().end(); ++it)
			{
				SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass *>(*it);
				if (pObject)
				{
					if (IsAllowed)// required, even if it does nothing...
					{
						if (IsAllowed(pObject, bStop))
						{
							switch (m_typeInfo.ObjectType)
							{
							case SVImageObjectType:
								{
									SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(pObject);
									if (pImage)
									{
										const SVString& name = pImage->getDisplayedName();
										if (!name.empty())
										{
											m_list.push_back(std::make_pair(name, pObject->GetUniqueObjectID()));
										}
									}
								}
								break;
							default:
								{
									SVString name;
									if (SVNotSetObjectType == m_objectTypeToInclude)
									{
										name = pObject->GetName();
									}
									else
									{
										name = pObject->GetObjectNameToObjectType(nullptr, m_objectTypeToInclude);
									}
									if (!name.empty())
									{
										m_list.push_back(std::make_pair(name, pObject->GetUniqueObjectID()));
									}
								}
								break;
							}
						}
					}
					else
					{
						bStop = true;
						hr = E_NOINTERFACE;
					}
				}
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvUl::NameGuidList& AvailableObjects() const { return m_list; }

	private:
		SVObjectTypeInfoStruct m_typeInfo;
		SvUl::NameGuidList m_list;
		GUID m_InstanceID;
		SVObjectTypeEnum m_objectTypeToInclude;
		IsAllowedFunc IsAllowed;
	};
} //namespace SvCmd
