//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Setting the Equation text.
//******************************************************************************
#pragma once
#include <boost/noncopyable.hpp>
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
//#include "SVEquation.h" // commented out while SVEquationClass resides in the SVObserver project

namespace Seidenader
{
	namespace GuiCommand
	{
		template <typename Equation>
		struct SetEquation : public boost::noncopyable
		{
			SetEquation(const SVGUID& rInstanceID, const SVString& rEquation) : m_InstanceID(rInstanceID), m_Equation(rEquation) {}
	
			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();

				Equation* pEquation = dynamic_cast<Equation*>(rMgr.GetObject(m_InstanceID));
				if (pEquation)
				{
					pEquation->SetEquationText(m_Equation);
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }

		private:
			SVGUID m_InstanceID;
			SVString m_Equation;
		};
	}
}
namespace GuiCmd = Seidenader::GuiCommand;