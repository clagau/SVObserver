//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUpperThresholdEquation
//* .File Name       : $Workfile:   SVUpperThresholdEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:56:32  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#pragma endregion Includes

class SVUpperThresholdEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVUpperThresholdEquationClass);

public:
	SVUpperThresholdEquationClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUPPERTHRESHOLDEQUATION );
	virtual ~SVUpperThresholdEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

protected:
	void init();
	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	SVDoubleValueObjectClass		result;			// resultant value
};

