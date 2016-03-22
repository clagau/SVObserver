//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditional
//* .File Name       : $Workfile:   SVConditional.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:59:32  $
//******************************************************************************

#pragma once

#include "SVEquation.h"

class SVConditionalClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVConditionalClass );

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVConditionalClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCONDITIONAL );
	virtual ~SVConditionalClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL OnValidate();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );	

protected:
	void init();

protected:
	SVBoolValueObjectClass result;			// resultant value
};


