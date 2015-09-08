//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the PPQ input names.
//******************************************************************************
#pragma once

#include <algorithm>
#include <boost/noncopyable.hpp>
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetPPQVariablesNames: public boost::noncopyable
		{
			GetPPQVariablesNames(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();
				SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(rMgr.GetObject(m_InstanceID));
				if (pInspection)
				{
					m_Names = pInspection->GetPPQInputNames();
				}
				return hr;
			}
			bool empty() const { return false; }
			const SVStringArray& GetResults() const { return m_Names; }

		private:
			SVGUID m_InstanceID;
			SVStringArray m_Names;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
