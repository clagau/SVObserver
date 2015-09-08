//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the PPQ Object Name from the Inspection.
//******************************************************************************
#pragma once

#include <boost/noncopyable.hpp>
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"

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
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();

				SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(rMgr.GetObject(m_InspectionID));
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
