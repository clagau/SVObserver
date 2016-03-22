//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:38  $
//******************************************************************************

#pragma once

#include "SVMathContainer.h"


class SVEvaluateClass : public SVMathContainerClass
{
	SV_DECLARE_CLASS( SVEvaluateClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVEvaluateClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
protected:
	SVDoubleValueObjectClass*		getOutputMathResult();

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
protected:
	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Embedded Object: ( Outputs )
	SVDoubleValueObjectClass		outputMathResult;
};


class SVEvaluateTranslationXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateTranslationXClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateTranslationXClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_X );
};

class SVEvaluateTranslationYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateTranslationYClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateTranslationYClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_Y );
};

class SVEvaluateRotationXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationXClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateRotationXClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_X );
};

class SVEvaluateRotationYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationYClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateRotationYClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_Y );
};

class SVEvaluateRotationAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationAngleClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateRotationAngleClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_ANGLE );
};



// New for polar transform...RO_27Feb2000

class SVEvaluateCenterXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateCenterXClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateCenterXClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_CENTER_X );
};

class SVEvaluateCenterYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateCenterYClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateCenterYClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_CENTER_Y );
};


class SVEvaluateStartRadiusClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateStartRadiusClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateStartRadiusClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_RADIUS );
};

class SVEvaluateEndRadiusClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateEndRadiusClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateEndRadiusClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_RADIUS );
};


class SVEvaluateStartAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateStartAngleClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateStartAngleClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_ANGLE );
};

class SVEvaluateEndAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateEndAngleClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateEndAngleClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_ANGLE );
};

