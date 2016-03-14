//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultString
//* .File Name       : $Workfile:   SVResultString.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:58:06  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVRESULTSTRING_H
#define SVRESULTSTRING_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"

class SVStringResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVStringResultClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVStringResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_STRING );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVStringResultClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

protected:
	SVStringValueObjectClass* getInputString();

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

	// Input
	SVInObjectInfoStruct		m_SVInputStringObjectInfo;

	// Output
	SVStringValueObjectClass		m_szValue;
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVRESULTLONG_H

