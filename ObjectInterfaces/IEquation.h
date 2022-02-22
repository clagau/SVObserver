//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// IEquation is the interface to the SVEquation for the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "EquationTestResult.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVStatusLibrary\MessageContainer.h
namespace SvStl
{
	class MessageContainer;
	typedef std::vector<MessageContainer> MessageContainerVector;
}
#pragma endregion Declarations

namespace SvOi
{
	class IEquation
	{
	public:
		virtual ~IEquation() {}
		virtual const std::string& GetEquationText() const = 0;
		virtual void SetEquationText(const std::string& text) = 0;
		virtual EquationTestResult Test( SvStl::MessageContainerVector *pErrorMessages=nullptr ) = 0;
		virtual double GetYACCResult() const = 0;
	};
} //namespace SvOi