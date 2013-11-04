//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransform
//* .File Name       : $Workfile:   SVPolarTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:29:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVPolarTransform.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVPolarTransformClass, SVPolarTransformClassGuid )

SVPolarTransformClass::SVPolarTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVPolarTransformObjectType;

	// Identify our input type needs...

	// Center X
	inputCenterXResult.SetInputObjectType( SVOutputEvaluateCenterXResultObjectGuid, SVDoubleValueObjectType );
	inputCenterXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputCenterXResult, _T( "PolarTransformCenterXResult" ) );

	// Center Y
	inputCenterYResult.SetInputObjectType( SVOutputEvaluateCenterYResultObjectGuid, SVDoubleValueObjectType );
	inputCenterYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputCenterYResult, _T( "PolarTransformCenterYResult" ) );

	// Start Radius
	inputStartRadiusResult.SetInputObjectType( SVOutputEvaluateStartRadiusResultObjectGuid, SVDoubleValueObjectType );
	inputStartRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputStartRadiusResult, _T( "PolarTransformStartRadiusResult" ) );

	// End Radius
	inputEndRadiusResult.SetInputObjectType( SVOutputEvaluateEndRadiusResultObjectGuid, SVDoubleValueObjectType );
	inputEndRadiusResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEndRadiusResult, _T( "PolarTransformEndRadiusResult" ) );
	
	// Start Angle
	inputStartAngleResult.SetInputObjectType( SVOutputEvaluateStartAngleResultObjectGuid, SVDoubleValueObjectType );
	inputStartAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputStartAngleResult, _T( "PolarTransformStartAngleResult" ) );

	// End Angle
	inputEndAngleResult.SetInputObjectType( SVOutputEvaluateEndAngleResultObjectGuid, SVDoubleValueObjectType );
	inputEndAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEndAngleResult, _T( "PolarTransformEndAngleResult" ) );

	// Register Embedded Objects
//	RegisterEmbeddedObject( &performTranslation, SVPerformTranslationObjectGuid, IDS_OBJECTNAME_PERFORM_TRANSLATION );


	// Set Embedded defaults
//	performTranslation.SetDefaultValue( FALSE, TRUE );


	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPolarTransformClass::~SVPolarTransformClass()
{
	CloseObject();
}

BOOL SVPolarTransformClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = TRUE;
	}

	isCreated = bOk;

	return bOk;
}

BOOL SVPolarTransformClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() )
	{
		BOOL bRetVal = TRUE;
		return bRetVal;
	}
	return FALSE;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterXResult()
{
	if( inputCenterXResult.IsConnected() && inputCenterXResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputCenterXResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputCenterYResult()
{
	if( inputCenterYResult.IsConnected() && inputCenterYResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputCenterYResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartRadiusResult()
{
	if( inputStartRadiusResult.IsConnected() && inputStartRadiusResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputStartRadiusResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndRadiusResult()
{

	if( inputEndRadiusResult.IsConnected() && inputEndRadiusResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputEndRadiusResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVPolarTransformClass::getInputStartAngleResult()
{
	if( inputStartAngleResult.IsConnected() && inputStartAngleResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputStartAngleResult.GetInputObjectInfo().PObject;

	return NULL;
}


SVDoubleValueObjectClass* SVPolarTransformClass::getInputEndAngleResult()
{
	if( inputEndAngleResult.IsConnected() && inputEndAngleResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputEndAngleResult.GetInputObjectInfo().PObject;

	return NULL;
}

BOOL SVPolarTransformClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputCenterXResult() &&
		getInputCenterYResult() &&
		getInputStartRadiusResult() &&
		getInputEndRadiusResult() &&
		getInputStartAngleResult() &&
		getInputEndAngleResult() )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

DWORD SVPolarTransformClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPolarTransform.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:29:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   12 Jul 2012 15:03:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jun 2012 14:00:56   jspila
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
 *    Rev 1.9   01 Jun 2010 15:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Dec 2009 12:29:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 15:19:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Feb 2005 15:01:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 13:52:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jan 2003 16:08:00   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Jan 2003 14:06:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the class to inherit from SVExtentHolder instead of SVTaskObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 19:56:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 10:44:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed reference to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:07:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides base class for polar transformation.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
