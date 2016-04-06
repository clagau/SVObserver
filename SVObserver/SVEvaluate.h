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

#pragma region Includes
#include "SVMathContainer.h"
#pragma endregion Includes

class SVEvaluateClass : public SVMathContainerClass
{
	SV_DECLARE_CLASS( SVEvaluateClass )

public:
	SVEvaluateClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE );
	virtual ~SVEvaluateClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

protected:
	SVDoubleValueObjectClass*		getOutputMathResult();

	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

protected:
	// Embedded Object: ( Outputs )
	SVDoubleValueObjectClass		outputMathResult;
};

class SVEvaluateTranslationXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateTranslationXClass )

public:
	SVEvaluateTranslationXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_X );
	virtual ~SVEvaluateTranslationXClass();
};

class SVEvaluateTranslationYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateTranslationYClass )

public:
	SVEvaluateTranslationYClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_Y );
	virtual ~SVEvaluateTranslationYClass();
};

class SVEvaluateRotationXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationXClass )

public:
	SVEvaluateRotationXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_X );
	virtual ~SVEvaluateRotationXClass();
};

class SVEvaluateRotationYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationYClass )
public:
	SVEvaluateRotationYClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_Y );
	virtual ~SVEvaluateRotationYClass();
};

class SVEvaluateRotationAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationAngleClass )

public:
	SVEvaluateRotationAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_ANGLE );
	virtual ~SVEvaluateRotationAngleClass();
};

// New for polar transform...RO_27Feb2000

class SVEvaluateCenterXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateCenterXClass )

public:
	SVEvaluateCenterXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_CENTER_X );
	virtual ~SVEvaluateCenterXClass();
};

class SVEvaluateCenterYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateCenterYClass )

public:
	SVEvaluateCenterYClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_CENTER_Y );
	virtual ~SVEvaluateCenterYClass();
};

class SVEvaluateStartRadiusClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateStartRadiusClass )

public:
	SVEvaluateStartRadiusClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_RADIUS );
	virtual ~SVEvaluateStartRadiusClass();
};

class SVEvaluateEndRadiusClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateEndRadiusClass )

public:
	SVEvaluateEndRadiusClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_RADIUS );
	virtual ~SVEvaluateEndRadiusClass();
};

class SVEvaluateStartAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateStartAngleClass )

public:
	SVEvaluateStartAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_ANGLE );
	virtual ~SVEvaluateStartAngleClass();
};

class SVEvaluateEndAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateEndAngleClass )

public:
	SVEvaluateEndAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_ANGLE );
	virtual ~SVEvaluateEndAngleClass();
};
