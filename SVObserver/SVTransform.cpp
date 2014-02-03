//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransform
//* .File Name       : $Workfile:   SVTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 12:22:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVTransform.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVTransformClass, SVTransformClassGuid )

SVTransformClass::SVTransformClass( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVTransformObjectType;

	// Identify our input type needs...
	// Translation X
	inputTranslationXResult.SetInputObjectType( SVOutputEvaluateTranslationXResultObjectGuid, SVDoubleValueObjectType );
	inputTranslationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputTranslationXResult, _T( "TransformTranslationXResult" ) );

	// Translation Y
	inputTranslationYResult.SetInputObjectType( SVOutputEvaluateTranslationYResultObjectGuid, SVDoubleValueObjectType );
	inputTranslationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputTranslationYResult, _T( "TransformTranslationYResult" ) );

	// Rotation X
	inputRotationXResult.SetInputObjectType( SVOutputEvaluateRotationXResultObjectGuid, SVDoubleValueObjectType );
	inputRotationXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationXResult, _T( "TransformRotationXResult" ) );

	// Rotation Y
	inputRotationYResult.SetInputObjectType( SVOutputEvaluateRotationYResultObjectGuid, SVDoubleValueObjectType );
	inputRotationYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationYResult, _T( "TransformRotationYResult" ) );

	// Rotation Angle
	inputRotationAngleResult.SetInputObjectType( SVOutputEvaluateRotationAngleResultObjectGuid, SVDoubleValueObjectType );
	inputRotationAngleResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputRotationAngleResult, _T( "TransformRotationAngleResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &performTranslation, SVPerformTranslationObjectGuid, IDS_OBJECTNAME_PERFORM_TRANSLATION, false, SVResetItemTool );
	RegisterEmbeddedObject( &performRotation, SVPerformRotationObjectGuid, IDS_OBJECTNAME_PERFORM_ROTATION, false, SVResetItemTool );

	RegisterEmbeddedObject( &learnedTranslationX, SVLearnedTranslationXObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedTranslationY, SVLearnedTranslationYObjectGuid, IDS_OBJECTNAME_LEARNED_TRANSLATION_Y, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationX, SVLearnedRotationXObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationY, SVLearnedRotationYObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_Y, false, SVResetItemTool );
	RegisterEmbeddedObject( &learnedRotationAngle, SVLearnedRotationAngleObjectGuid, IDS_OBJECTNAME_LEARNED_ROTATION_ANGLE, false, SVResetItemTool );

	// Set Embedded defaults
	performTranslation.SetDefaultValue( FALSE, TRUE );
	performRotation.SetDefaultValue( FALSE, TRUE );

	learnedTranslationX.SetDefaultValue( 0.0, TRUE );
	learnedTranslationY.SetDefaultValue( 0.0, TRUE );
	learnedRotationX.SetDefaultValue( 0.0, TRUE );
	learnedRotationY.SetDefaultValue( 0.0, TRUE );
	learnedRotationAngle.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVTransformClass::~SVTransformClass()
{
	CloseObject();
}

BOOL SVTransformClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = TRUE;
	}

	performTranslation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	performRotation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	learnedTranslationX.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedTranslationY.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedRotationX.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedRotationY.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedRotationAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVTransformClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() )
	{
		BOOL bRetVal = TRUE;
		return bRetVal;
	}
	return FALSE;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationXResult()
{
	if( inputTranslationXResult.IsConnected() && inputTranslationXResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputTranslationXResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVTransformClass::getInputTranslationYResult()
{
	if( inputTranslationYResult.IsConnected() && inputTranslationYResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputTranslationYResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationXResult()
{
	if( inputRotationXResult.IsConnected() && inputRotationXResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputRotationXResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVTransformClass::getInputRotationYResult()
{
	if( inputRotationYResult.IsConnected() && inputRotationYResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputRotationYResult.GetInputObjectInfo().PObject;

	return NULL;
}


SVDoubleValueObjectClass* SVTransformClass::getInputRotationAngleResult()
{
	if( inputRotationAngleResult.IsConnected() && inputRotationAngleResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputRotationAngleResult.GetInputObjectInfo().PObject;

	return NULL;
}

BOOL SVTransformClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputTranslationXResult() && getInputTranslationYResult() &&
		getInputRotationXResult()    && getInputRotationYResult() &&
		getInputRotationAngleResult() )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

LONG_PTR SVTransformClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTransform.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 12:22:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 12:26:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   05 Dec 2012 12:17:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed code associated with SCR 798
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Nov 2012 13:40:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new value object for the new translation mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Jul 2012 15:12:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 Jun 2012 14:23:20   jspila
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
 *    Rev 1.10   01 Jun 2010 15:16:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 13:22:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   29 Jul 2005 13:32:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jun 2005 08:36:30   ebeyeler
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
 *    Rev 1.6   17 Feb 2005 14:04:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Create Object / Reset Methodology
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Feb 2005 15:23:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 17:07:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jan 2003 09:33:46   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 20:15:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 14:46:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed SystemObjectInfoStruct from CreateObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:35:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Class for Transformations (base class).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
