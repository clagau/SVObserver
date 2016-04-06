//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoThresholdEquation
//* .File Name       : $Workfile:   SVAutoThresholdEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:00  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEquation.h"
#pragma endregion Includes

class SVAutoThresholdEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVAutoThresholdEquationClass);

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVAutoThresholdEquationClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVAUTOTHRESHOLDEQUATION );
	virtual ~SVAutoThresholdEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL OnValidate();

protected:
	void init();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	SVDoubleValueObjectClass		result;			// resultant value
};
