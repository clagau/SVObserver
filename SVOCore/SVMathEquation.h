//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathEquation
//* .File Name       : $Workfile:   SVMathEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:38  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#pragma endregion Includes

class SVMathEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVMathEquationClass);

public:
	SVMathEquationClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMATHEQUATION );
	virtual ~SVMathEquationClass();

protected:
	void init();
	virtual BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStruct) override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	SVDoubleValueObjectClass result;			// resultant value
};
