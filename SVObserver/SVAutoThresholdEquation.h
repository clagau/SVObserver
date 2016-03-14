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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVAutoTHRESHOLDEQUATION_H
#define SVAutoTHRESHOLDEQUATION_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************
#include "SVEquation.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVAutoThresholdEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVAutoThresholdEquationClass);

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVAutoThresholdEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVAUTOTHRESHOLDEQUATION );
	virtual ~SVAutoThresholdEquationClass();

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
#endif	// SVAutoTHRESHOLDEQUATION_H

