//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathEquation
//* .File Name       : $Workfile:   SVMathEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:38  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVMATHEQUATIONCLASS_H
#define SVMATHEQUATIONCLASS_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************
#include "SVEquation.h"

//******************************************************************************
//* CONSTANT(S):
//******************************************************************************

//******************************************************************************
//* MACRO(S):
//******************************************************************************

//******************************************************************************
//* TYPE(S):
//******************************************************************************

//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

//******************************************************************************
//* CONSTANT VARIABLE(S):
//******************************************************************************


//******************************************************************************
//* VARIABLE(S):
//******************************************************************************


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVMathEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVMathEquationClass);

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVMathEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMATHEQUATION );
	virtual ~SVMathEquationClass();

	virtual BOOL OnValidate();

	

protected:
	void init();
	virtual BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStruct);
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	SVDoubleValueObjectClass result;			// resultant value
};

/**************************************************************************/
/* INCLUDE CONTROL:
/**************************************************************************/
#endif	// SVMATHEQUATIONCLASS_H

//** EOF **

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMathEquation.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:29:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Jun 2007 14:16:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 11:16:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   09 Dec 2002 19:40:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added CreateObject method to class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:08:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Oct 1999 17:27:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected typo in defining include guard
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Sep 1999 10:18:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run Method to onRun.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Aug 1999 08:55:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  Steve Jones
 * Change Description:  
 *   New Class derived from SVEquation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
