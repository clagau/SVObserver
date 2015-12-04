//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Available Objects based on the type.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		typedef boost::function<bool (SvOi::IObjectClass*, bool& bStop)> IsAllowedFunc; 
		struct GetAvailableObjects: public boost::noncopyable
		{
			GetAvailableObjects(const GUID& rObjectID, const SVObjectTypeInfoStruct& typeInfo, IsAllowedFunc func) : m_InstanceID(rObjectID), m_typeInfo(typeInfo), IsAllowed(func) {}

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
								SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(pObject);
								if (pImage)
								{
									SVString name = pImage->getDisplayedName();
									m_list.push_back(std::make_pair(name, pObject->GetUniqueObjectID()));
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
			IsAllowedFunc IsAllowed;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
