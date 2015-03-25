//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointResultX
//* .File Name       : $Workfile:   SVDPointXResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:14  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVDPOINTXRESULT_H
#define SVDPOINTXRESULT_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"
#include "SVValueObjectImpl.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVDPointXResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDPointXResultClass );

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVDPointXResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_DPOINT_X );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVDPointXResultClass();

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
	SVDoubleValueObjectClass		x;
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVDPOINTXRESULT_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDPointXResult.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:21:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Aug 2005 15:40:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Feb 2005 13:51:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 09:09:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Nov 2001 16:05:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new classes to maintain and calculate sub-pixel values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/