//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// EquationTestResult holds the result information of the Equaton test operation.
//******************************************************************************
#pragma once

namespace SvOi
{
	class EquationTestResult
	{
	public:
		bool bPassed;
		int iPositionFailed;

		EquationTestResult() : bPassed(false), iPositionFailed(0) {}
	};
} //namespace SvOi