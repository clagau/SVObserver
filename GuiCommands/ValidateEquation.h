//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for testing/validating the Equation.
//******************************************************************************

#pragma once

#include <string>
#include <boost/noncopyable.hpp>
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
//#include "SVEquation.h" // comment out while SVEquationClass resides in the SVObserver project

enum
{
	validateSuccessful = -1 	// validation is successful
};

namespace Seidenader
{
	namespace GuiCommand
	{
		template <typename Equation>
		struct ValidateEquation : public boost::noncopyable
		{
			// Guid is the instance id of the object
			ValidateEquation(const SVGUID& ownerID, const SVString& equation, bool bRestore) 
			: m_ownerID(ownerID)
			, m_equation(equation)
			, m_bRestore(bRestore)
			, m_retValue(validateSuccessful)
			, m_result(0.0) {}

			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				SVObjectManagerClass& rMgr = SVObjectManagerClass::Instance();
		
				Equation* pEquation = dynamic_cast<Equation*>(SVObjectManagerClass::Instance().GetObject(m_ownerID));
				if (pEquation)
				{
					SVString oldString;
					if (m_bRestore)
					{
						//save old string
						pEquation->GetEquationText(oldString);
					}
					pEquation->SetEquationText(m_equation);

					SVEquationTestResult testResult = pEquation->Test();
					if (testResult.bPassed)
					{// set result and set return value to successful
						m_result = pEquation->GetYACCResult();
						m_retValue = validateSuccessful;
					}
					else
					{  // set return value to position of failed
						m_retValue = testResult.iPositionFailed;
					}
					if (m_bRestore)
					{
						//reset old string
						pEquation->SetEquationText(oldString);
					}
				}
				return hr;
			}
			int GetValidateStatus() { return m_retValue; }
			double GetResultValue() { return m_result; }

		private:
			SVGUID m_ownerID;
			const SVString& m_equation;
			bool m_bRestore;
			double m_result;
			int m_retValue;
		};
	}
}
