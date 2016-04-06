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
	EQAdjustSize( long subType, GUID resultGuid,  SVObjectClass* POwner = nullptr, int StringResourceID = 0 );
	virtual ~EQAdjustSize();

	virtual BOOL OnValidate()    override;
	virtual HRESULT ResetObject() override ;
	GUID& GetResultGuid();

	//************************************
	//! return white color
	//! \returns DWORD
	//************************************
	DWORD  GetObjectColor() const override;
	
protected:
	void Init();
	virtual BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStruct) override ;
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override ;
	
protected:
	SVDoubleValueObjectClass m_result;// resultant value
	GUID m_ResultGuid;
	long m_SubType;
	long m_Type;
};

