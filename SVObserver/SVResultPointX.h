//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointX
//* .File Name       : $Workfile:   SVResultPointX.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:16  $
//******************************************************************************

#pragma once

#include "SVResult.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVPointXResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVPointXResultClass );

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPointXResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_POINT_X );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVPointXResultClass();

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
	SVLongValueObjectClass		x;
};
