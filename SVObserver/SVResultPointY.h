//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointY
//* .File Name       : $Workfile:   SVResultPointY.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:42  $
//******************************************************************************

#pragma once

#include "SVResult.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVPointYResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVPointYResultClass );

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPointYResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_POINT_Y );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVPointYResultClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

protected:
	SVPointValueObjectClass* getInputPoint();

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
	SVLongValueObjectClass		y;
};
