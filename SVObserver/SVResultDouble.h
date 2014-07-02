//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultDouble
//* .File Name       : $Workfile:   SVResultDouble.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Jul 2014 15:15:22  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVRESULTDOUBLE_H
#define SVRESULTDOUBLE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"



class SVDoubleResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDoubleResultClass );

public:
	SVDoubleResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_DOUBLE );
	virtual ~SVDoubleResultClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();
	SVDoubleValueObjectClass* getInputDouble();
protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

// Data Element(s):
public:
	// Input: Point
	SVInObjectInfoStruct		inputDoubleObjectInfo;
protected:
	// Output
	SVDoubleValueObjectClass		value;
};



#endif	// SVRESULTDOUBLE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVResultDouble.h_v  $
 * 
 *    Rev 1.1   01 Jul 2014 15:15:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  913
 * SCR Title:  Add Blob Feature Default Value to the Configuration Print (SVO-214)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   chnaged getInputDouble to be a Public Method so it could be used in the Configuration Print
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:56:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 Jun 2012 14:12:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   02 Sep 2005 14:33:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made speed optimizations by making functions inline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Feb 2005 15:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 14:55:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   16 Feb 2000 17:00:40   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:31:56   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Sep 1999 11:33:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised default resid for the classname to be
 * IDS_CLASSNAME_RESULT_DOUBLE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Sep 1999 08:30:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Validate method to OnValidate.
 * Revised Run method to onRun.
 * Removed resultColor as there is now a color in the base
 * class SVObjectClass. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Aug 1999 10:56:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New SVDoubleResultClass for use with doible values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
