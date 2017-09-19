//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Base class for Formulas for Tool Width,Height and Positions for ToolsSizeAdjust 
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "SVEquation.h"
#pragma endregion Includes

//! \brief  Base class for Formulas for Tool Width Height and Positions for ToolsSizeAdjust 
class EQAdjustSize : public SVEquationClass
{
	
public:
	EQAdjustSize(SVObjectSubTypeEnum subType, GUID resultGuid, SVObjectClass* POwner = nullptr, int StringResourceID = 0);
	virtual ~EQAdjustSize();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override ;
	GUID& GetResultGuid();

	//************************************
	//! return white color
	//! \returns DWORD
	//************************************
	DWORD  GetObjectColor() const override;
	
protected:
	void Init();
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override ;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override ;
	
protected:
	SVDoubleValueObjectClass m_result;// resultant value
	GUID m_ResultGuid;
	SVObjectSubTypeEnum m_SubType;
};

