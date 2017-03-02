//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for testing/validating the Equation.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/noncopyable.hpp>
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IEquation.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

enum
{
	validateSuccessful = -1 	// validation is successful
};

namespace Seidenader
{
	namespace GuiCommand
	{
		struct ValidateAndSetEquation : public boost::noncopyable
		{
			// Guid is the instance id of the object
			ValidateAndSetEquation(const SVGUID& ownerID, const SVString& equation, bool bSetValue) 
			: m_ownerID(ownerID)
			, m_equation(equation)
			, m_bSetValue(bSetValue)
			, m_retValue(validateSuccessful)
			, m_result(0.0) {}

			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				m_ErrorMessages.clear();
		
				SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(m_ownerID));
				if (pEquation)
				{
					SVString oldString;
					//save old string
					oldString = pEquation->GetEquationText();
					pEquation->SetEquationText(m_equation);

					SvOi::EquationTestResult testResult = pEquation->Test(&m_ErrorMessages);
					if (testResult.bPassed)
					{// set result and set return value to successful
						m_result = pEquation->GetYACCResult();
						m_retValue = validateSuccessful;
					}
					else
					{  // set return value to position of failed
						m_retValue = testResult.iPositionFailed;
					}
					if (!m_bSetValue || validateSuccessful != m_retValue)
					{
						//reset old string
						pEquation->SetEquationText(oldString);
					}
				}
				return hr;
			}
			int GetValidateStatus() { return m_retValue; }
			double GetResultValue() { return m_result; }
			const SvStl::MessageContainerVector& GetErrorMessages() { return m_ErrorMessages; }

		private:
			SVGUID m_ownerID;
			const SVString& m_equation;
			bool m_bSetValue;
			double m_result;
			int m_retValue;
			SvStl::MessageContainerVector m_ErrorMessages;
		};
	}
}
