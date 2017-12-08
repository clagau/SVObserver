//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Equation text.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IEquation.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetEquation
	{
	public:
		GetEquation(const GetEquation&) = delete;
		GetEquation& operator=(const GetEquation&) = delete;

		GetEquation(const SVGUID& rInstanceID) : m_InstanceID(rInstanceID) {};

		virtual ~GetEquation() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(m_InstanceID));
			if (pEquation)
			{
				m_Equation = pEquation->GetEquationText();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const std::string& GetEquationString() const { return m_Equation; }

	private:
		SVGUID m_InstanceID;
		std::string m_Equation;
	};
} //namespace SvCmd
