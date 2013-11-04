//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:00  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVEVALUATE_H
#define SVEVALUATE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	virtual DWORD processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext );

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





//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVEVALUATE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEvaluate.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:26:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 09:25:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 01 2000 12:00:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new evaluate classes:
 * SVEvaluateCenterXClass, SVEvaluateCenterYClass, 
 * SVEvaluateStartRadiusClass, SVEvaluateEndRadiusClass,
 * SVEvaluateStartAngleClass, SVEvaluateEndAngleClass
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:55:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Base Class for Evaluation classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/