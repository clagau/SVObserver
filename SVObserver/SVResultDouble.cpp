//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultDouble
//* .File Name       : $Workfile:   SVResultDouble.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:55:50  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////

#include "SVResultDouble.h"
#include "SVRange.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVDoubleResultClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVDoubleResultClass, SVDoubleResultClassGuid );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDoubleResultClass::SVDoubleResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVResultDoubleObjectType;

	// Identify our input type needs
	inputObjectInfo.SetInputObjectType( SVDoubleValueObjectType );
	inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputObjectInfo, _T( "DoubleResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SVResetItemNone );

	// Set Embedded defaults
	value.SetDefaultValue( 0, TRUE );
	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
	rangeClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass...
	rangeClassInfo.ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.ClassId = SVRangeClassGuid;
	rangeClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVRANGE );

	//  Construct it
	SVRangeClass* pRange = ( SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVDoubleResultClass::~SVDoubleResultClass()
{
	CloseObject();
}

BOOL SVDoubleResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = getInput() != NULL;
	}

	value.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVDoubleResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

BOOL SVDoubleResultClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( inputObjectInfo.IsConnected() &&
		inputObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
		bRetVal = SVResultClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

BOOL SVDoubleResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun(RRunStatus) )
	{
		SVDoubleValueObjectClass* pValue = static_cast <SVDoubleValueObjectClass*> (getInput());
		ASSERT( pValue );

		double v;

		pValue->GetValue(v);

		// Set Value
		value.SetValue(RRunStatus.m_lResultDataIndex, v);

	
		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}


SVDoubleValueObjectClass* SVDoubleResultClass::getInputDouble()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVDoubleValueObjectClass*>(inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVResultDouble.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:55:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   12 Jul 2012 15:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   19 Jun 2012 14:12:32   jspila
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
 *    Rev 3.9   01 Jun 2010 15:08:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   02 Sep 2005 14:33:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made speed optimizations by making functions inline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   29 Jul 2005 13:22:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   21 Jun 2005 08:30:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Feb 2005 15:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 14:55:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   09 Dec 2002 20:02:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 13:00:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
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
 *    Rev 3.0   23 Nov 1999 13:31:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Sep 1999 16:05:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onRun Method.
 * Revosed Constructor initialization to dymanically allocate the
 * SVRangeClass and set the SVRangeClass's required input
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Sep 1999 13:16:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised local variables isFailed,isWarned
 *  from double to bool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Sep 1999 08:30:18   sjones
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
 *    Rev 1.0   31 Aug 1999 10:56:38   sjones
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
