//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Embedded ValueObject Values.
//******************************************************************************

#pragma once

#include <boost/noncopyable.hpp>
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
//#include "SVValueObjectClass.h"  // commented out while SVValueObjectClass resides in the SVObserver project

namespace Seidenader
{ 
	namespace GuiCommand
	{
		// while SVValueObjectClass resides in the SVObserver project, use the ValueObject template parameter
		template<typename Items, typename ValueObject>
		struct TaskObjectSetEmbeddedValues : public boost::noncopyable
		{
			TaskObjectSetEmbeddedValues(const Items& items) : m_Items(items) {}
		
			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();
		
				for (Items::const_iterator it = m_Items.begin();it != m_Items.end() && S_OK == hr;++it)
				{
					ValueObject* pObject = dynamic_cast<ValueObject *>(SVObjectManagerClass::Instance().GetObject(it->second.GetObjectID()));
					if (pObject)
					{
						if (!it->second.isReadOnly() && it->second.isModified())
						{
							// SEJ999 - need to handle arrays here...
							// This needs to be more robust, nmore like ProcessInputRequests method of SVInspectionProcess...
							SVString stringValue = boost::any_cast<_variant_t>(it->second.GetValue());
							hr = pObject->SetValue(pObject->GetLastSetIndex(), stringValue.c_str());
						}
					}
					else
					{
						hr = E_POINTER;
					}
				}
				return hr;
			}

		private:
			const Items& m_Items;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
