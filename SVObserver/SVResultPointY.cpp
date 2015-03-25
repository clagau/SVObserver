//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointY
//* .File Name       : $Workfile:   SVResultPointY.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:28  $
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

#include "SVResultPointY.h"
#include "SVRange.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVPointYResultClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVPointYResultClass, SVPointYResultClassGuid );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVPointYResultClass::SVPointYResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVResultPointYObjectType;

	// Identify our input type needs
	
	inputObjectInfo.SetInputObjectType( SVPointValueObjectType );
	inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputObjectInfo, _T( "PointYResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &y, SVYObjectGuid, IDS_OBJECTNAME_Y, false, SVResetItemNone );

	// Set Embedded defaults
	y.SetDefaultValue( 0, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVYObjectGuid;
	rangeClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.ClassId = SVRangeClassGuid;
	rangeClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = (SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPointYResultClass::~SVPointYResultClass()
{

}

BOOL SVPointYResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = getInputPoint() != NULL;
	}

	y.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVPointYResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVPointValueObjectClass* SVPointYResultClass::getInputPoint()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVPointValueObjectClass*>(inputObjectInfo.GetInputObjectInfo().PObject);

	return NULL;
}


BOOL SVPointYResultClass::OnValidate()
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

BOOL SVPointYResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( RRunStatus ) )
	{
		SVPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		POINT p;
		pPoint->GetValue( p );

		// Set X
		y.SetValue( RRunStatus.m_lResultDataIndex, p.y );
	
		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVResultPointY.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:57:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   12 Jul 2012 15:05:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   19 Jun 2012 14:12:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   01 Jun 2010 15:08:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   29 Jul 2005 13:22:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   21 Jun 2005 08:30:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   17 Feb 2005 15:25:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   22 Apr 2003 14:55:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   09 Dec 2002 20:02:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 13:02:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
