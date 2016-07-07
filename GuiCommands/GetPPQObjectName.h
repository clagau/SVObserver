//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the PPQ Object Name from the Inspection.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetPPQObjectName : public boost::noncopyable
		{
			GetPPQObjectName(const SVGUID& rInspectionID) : m_InspectionID(rInspectionID) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = E_POINTER;

				SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(m_InspectionID));
				if (pInspection)
				{
					SvOi::IObjectClass* pPPQ = pInspection->GetPPQInterface();
					if (pPPQ)
					{
						m_Name = pPPQ->GetName();
						hr = S_OK;
					}
				}
				return hr;
			}
			bool empty() const { return false; }
			const SVString& GetName() const { return m_Name; }

		private:
			SVGUID m_InspectionID;
			SVString m_Name;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
