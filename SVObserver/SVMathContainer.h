//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathContainer
//* .File Name       : $Workfile:   SVMathContainer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:16  $
//******************************************************************************

#pragma once

#include "SVTaskObject.h"

class SVIPDoc;
class SVToolClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVMathContainerClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVMathContainerClass )

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVMathContainerClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMATHCONTAINER );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVMathContainerClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

protected:
	SVDoubleValueObjectClass*	getInputMathResult();

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	SVInObjectInfoStruct		inputMathResult;

};

