//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Embedded ValueObject Values.
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader
{ 
	namespace GuiCommand
	{
		template<typename Items>
		struct TaskObjectSetEmbeddedValues : public boost::noncopyable
		{
			TaskObjectSetEmbeddedValues(const Items& items) : m_Items(items) {}
		
			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
	
				for (Items::const_iterator it = m_Items.begin();it != m_Items.end() && S_OK == hr;++it)
				{
					SvOi::IValueObject* pObject = dynamic_cast<SvOi::IValueObject *>(SvOi::getObject(it->second.GetObjectID()));
					if (pObject)
					{
						if (!it->second.isReadOnly() && it->second.isModified())
						{
							// SEJ999 - need to handle arrays here...
							// This needs to be more robust, more like ProcessInputRequests method of SVInspectionProcess...
							_variant_t value = boost::any_cast<_variant_t>(it->second.GetValue());
							hr = pObject->SetValue(value);
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
