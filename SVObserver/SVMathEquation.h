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

#include "SVEquation.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVMathEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVMathEquationClass);

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVMathEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMATHEQUATION );
	virtual ~SVMathEquationClass();

	virtual BOOL OnValidate();

	

protected:
	void init();
	virtual BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStruct);
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	SVDoubleValueObjectClass result;			// resultant value
};
