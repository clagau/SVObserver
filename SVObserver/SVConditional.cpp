//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditional.cpp
//* .File Name       : $Workfile:   SVConditional.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:59:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVConditional.h"
#include "SVSVIMStateClass.h"


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVConditionalClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVConditionalClass, SVConditionalClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditionalClass::SVConditionalClass( SVObjectClass* POwner, int StringResourceID ) 
: SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVConditionalClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :26.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVConditionalClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVConditionalObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVConditionalResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( FALSE, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditionalClass::~SVConditionalClass()
{
}



BOOL SVConditionalClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVEquationClass::CreateObject( PCreateStructure );

	// Set/Reset printable Flags
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVConditionalClass::OnValidate()
{
	BOOL retVal = TRUE;
	
	// validate our outputs
	retVal = SVEquationClass::OnValidate() && retVal;

	if( !retVal )
		SetInvalid();

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
BOOL SVConditionalClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL value = FALSE;

	// Bypass all conditional execution if we are performing an internal run
	// This is used to initially setup certain objects.
	if( SVSVIMStateClass::CheckState( SV_STATE_INTERNAL_RUN ) )
	{
		result.SetValue( RRunStatus.m_lResultDataIndex, 1L );
		return TRUE;
	}// end if

	BOOL retVal = SVEquationClass::onRun( RRunStatus );
	if( !retVal )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	else
	{
		value = TRUE;
		if( HasCondition() && IsEnabled() )
		{
			//value = ( BOOL )yacc.equationResult;  08 Feb 2000 - frb.
            value = yacc.equationResult ? TRUE : FALSE;
		}
	}
	result.SetValue( RRunStatus.m_lResultDataIndex, value );

	return retVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditional.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:59:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   09 Jul 2012 13:59:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   18 Jun 2012 18:09:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   01 Jun 2010 14:13:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   29 Jul 2005 12:17:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   21 Jun 2005 08:02:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   28 Oct 2003 11:34:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  390
   SCR Title:  Update SVObserver to Version 4.22 Release and make Bug Fixes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Made change to handle initial run of tools to update their shadowed parameters by forcing the conditional true.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   17 Apr 2003 17:16:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   16 Apr 2003 17:38:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  342
   SCR Title:  Configuration report printout changes after going online.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new state SV_STATE_INTERNAL_RUN. This is used to force all objects to run during the loading process. Otherwise some objects may be conditionally not executed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   29 Jan 2003 15:06:02   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated CreateObject method to set isCreated flag upon creating object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   09 Dec 2002 19:03:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   19 Nov 2002 11:06:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
