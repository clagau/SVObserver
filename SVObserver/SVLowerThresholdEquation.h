//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLowerThresholdEquation
//* .File Name       : $Workfile:   SVLowerThresholdEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:18:50  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVLowerTHRESHOLDEQUATION_H
#define SVLowerTHRESHOLDEQUATION_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************
#include "SVEquation.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVLowerThresholdEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVLowerThresholdEquationClass);

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVLowerThresholdEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLOWERTHRESHOLDEQUATION );
	virtual ~SVLowerThresholdEquationClass();

	virtual BOOL OnValidate();

protected:
	BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStruct);
	void init();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	SVDoubleValueObjectClass		result;			// resultant value
};

/**************************************************************************/
/* INCLUDE CONTROL:
/**************************************************************************/
#endif	// SVLowerTHRESHOLDEQUATION_H

