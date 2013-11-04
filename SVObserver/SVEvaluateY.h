//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluateY
//* .File Name       : $Workfile:   SVEvaluateY.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:30:04  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVEVALUATEY_H
#define SVEVALUATEY_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMathContainer.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVEvaluateYClass : public SVMathContainerClass
{
	SV_DECLARE_CLASS( SVEvaluateYClass )

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVEvaluateYClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEVALUATEY );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVEvaluateYClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************
public:

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
protected:
	SVBoolValueObjectClass*			getOutputMathEnabled();
	SVDoubleValueObjectClass*		getOutputMathResult();

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:

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

	SVBoolValueObjectClass			outputMathEnabled;
	SVDoubleValueObjectClass		outputMathResult;

};


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVEVALUATEY_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEvaluateY.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:30:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 09:32:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:31:32   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:14:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides Y Value Equation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/