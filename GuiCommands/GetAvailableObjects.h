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
#include "ObjectInterfaces\IObjectClass.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces/ITool.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#pragma endregion Includes

namespace SvCmd
{
	class IsValidObject 
	{
	public:
		bool operator()(const SvOi::IObjectClass* pObject, bool& bStop) const
		{
			bool Result{ false };
			if( nullptr != pObject )
			{
				Result = !isHidden(pObject);
			}
			return Result;
		}

	private:
		bool isHidden(const SvOi::IObjectClass* pObject) const
		{
			return (pObject->ObjectAttributesAllowed() & SvDef::SV_HIDDEN) ? true : false;
		}
	};

	class IsObjectFromPriorTool
	{
	public:
		IsObjectFromPriorTool(const SVGUID& rTaskObjectID)
			: m_TaskObjectID(rTaskObjectID)
		{
			const SvOi::ITool* pTool = dynamic_cast<const SvOi::ITool*> (SvOi::getObject(m_TaskObjectID));
			if (nullptr != pTool)
			{
				m_ToolPos = pTool->getToolPosition();
			}
		}

		bool operator()(const SvOi::IObjectClass* pObject, bool& bStop) const
		{
			bStop = !isObjectAboveTask(pObject);
			return !bStop && !isHidden(pObject);
		}

	private:
		bool isHidden(const SvOi::IObjectClass* pObject) const
		{
			return (pObject->ObjectAttributesAllowed() & SvDef::SV_HIDDEN) ? true : false;
		}

		bool isObjectAboveTask(const SvOi::IObjectClass* pObject) const
		{
			bool bRetVal = true;
			const SvOi::ITool* pOwnerTool = dynamic_cast<const SvOi::ITool*> (pObject->GetAncestorInterface(SvDef::SVToolObjectType));

			if (nullptr != pOwnerTool)
			{
				//Stop when tool position greater or the same m_ToolPos must be valid
				if (0 <= m_ToolPos && m_ToolPos <= pOwnerTool->getToolPosition())
				{
					bRetVal = false;
				}
			}
			return bRetVal;
		}

	private:
		SVGUID m_TaskObjectID;
		int m_ToolPos{ -1 };
	};

	typedef boost::function<bool (const SvOi::IObjectClass*, bool& bStop)> IsAllowedFunc; 
	struct GetAvailableObjects: public boost::noncopyable
	{
		/// \param rObjectID [in] Object Id of the object from which level the available objects will search for.
		/// \param typeInfo [in] Type of the available objects
		/// \param objectTypeToInclude [in] Object type until the name of the available object will set. SvDef::SVNotSetObjectType means only object name and e.g. SvDef::SVToolSetObjectType means "Tool Set.Window Tool....". This parameter will not used for image objects.
		/// \param func [in]
		GetAvailableObjects(const SVGUID& rObjectID, const SvDef::SVObjectTypeInfoStruct& typeInfo, IsAllowedFunc func = IsValidObject(), SvDef::SVObjectTypeEnum objectTypeToInclude = SvDef::SVNotSetObjectType )
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
				const SvOi::IObjectClass* pObject = dynamic_cast<const SvOi::IObjectClass*> (*it);
				if (nullptr != pObject)
				{
					if (IsAllowed)// required, even if it does nothing...
					{
						if (IsAllowed(pObject, bStop))
						{
							switch (m_typeInfo.ObjectType)
							{
							case SvDef::SVImageObjectType:
								{
									const SvOi::ISVImage* pImage = dynamic_cast<const SvOi::ISVImage*>(pObject);
									if (pImage)
									{
										const std::string& name = pImage->getDisplayedName();
										if (!name.empty())
										{
											m_list.push_back(std::make_pair(name, pObject->GetUniqueObjectID()));
										}
									}
								}
								break;
							default:
								{
									std::string name;
									if (SvDef::SVNotSetObjectType == m_objectTypeToInclude)
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
		SvDef::SVObjectTypeInfoStruct m_typeInfo;
		SvUl::NameGuidList m_list;
		SVGUID m_InstanceID;
		SvDef::SVObjectTypeEnum m_objectTypeToInclude;
		IsAllowedFunc IsAllowed;
	};
} //namespace SvCmd
