//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file EquationArray.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class to get an array of values.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EquationArray.h"
#include "SVStatusLibrary/RunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

	EquationArray::EquationArray(SVObjectClass* POwner, int StringResourceID)
		:SVEquation(POwner, StringResourceID)
	{
	}




	bool EquationArray::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		bool bOk = __super::CreateObject(rCreateStructure) && nullptr != GetInspection();

		// Set / Reset Printable Flag
		m_enabled.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

		m_isCreated = bOk;

		return bOk;
	}

	bool EquationArray::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
	{
		if (0 >= m_stopValue - m_startValue + 1)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		if (IsEnabled())
		{
			m_valueArray.resize(m_stopValue - m_startValue + 1);
			if (HasCondition())
			{
				auto iter = m_valueArray.begin();
				for (int i = m_startValue; i <= m_stopValue && iter != m_valueArray.end(); ++i)
				{
					m_currentIndexValue = i;
					/////////////////////////////////////////////////////
					// The lexical Stack must be valid at this point
					// And the yacc parsing must not have any errors
					/////////////////////////////////////////////////////
					m_isDataValid = true;

					ParseYacc();

					/////////////////////////////////////////////////////
					// Check for Valid Data
					/////////////////////////////////////////////////////
					if (!m_isDataValid || m_Yacc.yacc_err)
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
							pErrorMessages->push_back(Msg);
						}
						rRunStatus.SetInvalid();
						return false;
					}
					else
					{
						*iter = m_Yacc.equationResult;
						++iter;
					}
				}
			}
			else
			{
				memset(m_valueArray.data(), 0, sizeof(double) * m_valueArray.size());
			}
		}

		rRunStatus.SetPassed();

		return true;
	}

	bool EquationArray::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool Result = __super::ResetObject(pErrorMessages);

		if (Result)
		{
			if (m_startValue > m_stopValue)
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
			else
			{
				m_valueArray.resize(m_stopValue - m_startValue + 1);
			}
		}

		return Result;
	}

	bool EquationArray::setRunValues(int startValue, int stopValue)
	{
		assert(startValue <= stopValue);
		if (startValue <= stopValue)
		{
			m_startValue = startValue;
			m_stopValue = stopValue;
			return true;
		}
		else
		{
			return false;
		}
	}
} //namespace SvOp
