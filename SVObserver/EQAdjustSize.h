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

	SV_DECLARE_CLASS( EQAdjustSize);
public:
	EQAdjustSize( SVObjectClass* POwner = NULL , int StringResourceID = 0 );
	virtual ~EQAdjustSize();

	virtual BOOL OnValidate()    override;
	virtual HRESULT ResetObject() override ;

protected:
	virtual BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStruct) override ;
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override ;
	
protected:
	SVDoubleValueObjectClass m_result;			// resultant value
};

