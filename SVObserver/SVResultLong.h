//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultLong
//* .File Name       : $Workfile:   SVResultLong.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:56:52  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVRESULTLONG_H
#define SVRESULTLONG_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"
#include "SVValueObject.h"

class SVLongResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVLongResultClass );

public:
	SVLongResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_LONG );
	virtual ~SVLongResultClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

	const GUID & GetInputEmbeddedID() const;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVLongValueObjectClass* getInputLong();

// Data Element(s):
protected:
	// Output
	SVLongValueObjectClass		value;
};

inline const GUID & SVLongResultClass::GetInputEmbeddedID() const
{
	const SVLongValueObjectClass * l_vo = static_cast <SVLongValueObjectClass *> (const_cast<SVLongResultClass *>(this)->getInput());
	if (l_vo)
	{
		return l_vo->GetEmbeddedID();
	}
	else
	{
		return SVInvalidGUID;
	}
}


#endif	// SVRESULTLONG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVResultLong.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:56:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 Jun 2012 14:12:34   jspila
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
 *    Rev 3.5   10 Dec 2008 20:07:18   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added GetInputEmbeddedID() method
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
 *    Rev 3.3   17 Feb 2005 15:25:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 14:55:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   16 Feb 2000 17:00:42   mike
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
 *    Rev 3.0   23 Nov 1999 13:34:52   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Sep 1999 08:30:14   sjones
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
 *    Rev 1.2   31 Aug 1999 09:41:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVRunStatusClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Aug 1999 15:01:16   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced register input interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Aug 1999 13:14:54   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
