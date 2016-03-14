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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVUPPERTHRESHOLDEQUATION_H
#define SVUPPERTHRESHOLDEQUATION_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************
#include "SVEquation.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVUpperThresholdEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVUpperThresholdEquationClass);

public:
	SVUpperThresholdEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVUPPERTHRESHOLDEQUATION );
	virtual ~SVUpperThresholdEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL OnValidate();

protected:
	void init();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	SVDoubleValueObjectClass		result;			// resultant value
};

/**************************************************************************/
/* INCLUDE CONTROL:
/**************************************************************************/
#endif	// SVUPPERTHRESHOLDEQUATION_H

