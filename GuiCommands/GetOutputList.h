//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Task Object's Output list (for the object selector).
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		template <typename FilterFunc, typename Results>
		struct GetOutputList : public boost::noncopyable
		{
			GetOutputList(const SVGUID& rInstanceID, FilterFunc filter) : m_InstanceID(rInstanceID), m_filter(filter) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = E_POINTER;

				SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
				if (pObject)
				{
					SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject *>(pObject);
					if (pTaskObject)
					{
						m_outputInfoList = pTaskObject->GetOutputList();
						hr = S_OK;
					}
					else // try inspection
					{
						SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(pObject);
						if (pInspection)
						{
							m_outputInfoList = pInspection->GetToolSetInterface()->GetOutputList(m_filter);
							hr = S_OK;
						}
					}
				}
				return hr;
			}
			bool empty() const { return false; }
			Results GetResults() const { return m_outputInfoList; }

		private:
			SVGUID m_InstanceID;
			FilterFunc m_filter;
			Results m_outputInfoList;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
