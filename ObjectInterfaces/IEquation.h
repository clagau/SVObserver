//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IEquation is the interface to the SVEquationClass for the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "EquationTestResult.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IEquation
		{
		public:
			virtual ~IEquation() {}
			virtual const SVString& GetEquationText() const = 0;
			virtual void SetEquationText(const SVString& text) = 0;
			virtual EquationTestResult Test( SvStl::MessageContainerVector *pErrorMessages=nullptr ) = 0;
			virtual double GetYACCResult() const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;