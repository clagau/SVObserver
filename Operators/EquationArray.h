//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file EquationArray.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class to get an array of values.
//******************************************************************************
#pragma once


#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVEquation.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

namespace SvOp
{
	class EquationArray : public SVEquation
	{
	public:
		EquationArray(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEQUATION);
		virtual ~EquationArray() = default;

		virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

#pragma region IEquation
		virtual int GetIndexValue() const override { return m_currentIndexValue; };
#pragma endregion IEquation

		virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		bool setRunValues(int startValue, int stopValue);

#pragma region Methods to replace processMessage
#pragma endregion Methods to replace processMessage

	protected:
		virtual bool onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		std::vector<double> getValueArray() const { return m_valueArray; };

	private:
		int m_startValue = 0;
		int m_stopValue = 0;
		std::vector<double> m_valueArray;

		int m_currentIndexValue = 0;
	};

} //namespace SvOp
