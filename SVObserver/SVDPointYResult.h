//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointYResult
//* .File Name       : $Workfile:   SVDPointYResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:30  $
//******************************************************************************

#pragma once

#include "SVResult.h"

class SVDPointValueObjectClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVDPointYResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDPointYResultClass );

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVDPointYResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_DPOINT_Y );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVDPointYResultClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
protected:
	SVDPointValueObjectClass* getInputPoint();

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
public:

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Output
	SVDoubleValueObjectClass		y;
};
