//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:32:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVEvaluate.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVEvaluateClass, SVEvaluateClassGuid )

SVEvaluateClass::SVEvaluateClass( SVObjectClass* POwner, int StringResourceID )
				 :SVMathContainerClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateObjectType;

	// Identify our input type needs...

	// Register Embedded Objects
//	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateResultObjectGuid, IDS_OBJECTNAME_RESULT );

	// Set Embedded defaults
//	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateClass::~SVEvaluateClass()
{
	CloseObject();
}

BOOL SVEvaluateClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVMathContainerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable flag
	outputMathResult.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return isCreated;
}

BOOL SVEvaluateClass::CloseObject()
{
	if( SVMathContainerClass::CloseObject() )
	{
		BOOL bRetVal = TRUE;
		return bRetVal;
	}
	return FALSE;
}

SVDoubleValueObjectClass* SVEvaluateClass::getOutputMathResult()
{
	return &outputMathResult;
}

BOOL SVEvaluateClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVDoubleValueObjectClass* pResult = getOutputMathResult();
	ASSERT( pResult );
	
	// All inputs and outputs must be validated first
	if( SVMathContainerClass::onRun( RRunStatus ) )
	{
		SVDoubleValueObjectClass* pInputResult = getInputMathResult();
		ASSERT( pInputResult );

		double result;
		if( S_OK != pInputResult->GetValue( result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		if( S_OK != pResult->SetValue( RRunStatus.m_lResultDataIndex, result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}


LONG_PTR SVEvaluateClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	return( SVMathContainerClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateTranslationXClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateTranslationXClass, SVEvaluateTranslationXClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateTranslationXClass::SVEvaluateTranslationXClass( SVObjectClass* POwner, int StringResourceID )
				 :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateTranslationXObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateTranslationXResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateTranslationYClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateTranslationYClass, SVEvaluateTranslationYClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateTranslationYClass::SVEvaluateTranslationYClass( SVObjectClass* POwner, int StringResourceID )
				 :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateTranslationYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateTranslationYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateRotationXClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateRotationXClass, SVEvaluateRotationXClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateRotationXClass::SVEvaluateRotationXClass( SVObjectClass* POwner, int StringResourceID )
				 :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateRotationXObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateRotationXResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateRotationYClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateRotationYClass, SVEvaluateRotationYClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateRotationYClass::SVEvaluateRotationYClass( SVObjectClass* POwner, int StringResourceID )
				 :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateRotationYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateRotationYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateRotationAngleClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateRotationAngleClass, SVEvaluateRotationAngleClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateRotationAngleClass::SVEvaluateRotationAngleClass( SVObjectClass* POwner, int StringResourceID )
				 :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateRotationAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateRotationAngleResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateCenterXClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateCenterXClass, SVEvaluateCenterXClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateCenterXClass::SVEvaluateCenterXClass( SVObjectClass* POwner, int StringResourceID )
					   :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateCenterXObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateCenterXResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateCenterYClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateCenterYClass, SVEvaluateCenterYClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateCenterYClass::SVEvaluateCenterYClass( SVObjectClass* POwner, int StringResourceID )
					   :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateCenterYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateCenterYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateStartRadiusClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateStartRadiusClass, SVEvaluateStartRadiusClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateStartRadiusClass::SVEvaluateStartRadiusClass( SVObjectClass* POwner, int StringResourceID )
					       :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateStartRadiusObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateStartRadiusResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateEndRadiusClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateEndRadiusClass, SVEvaluateEndRadiusClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateEndRadiusClass::SVEvaluateEndRadiusClass( SVObjectClass* POwner, int StringResourceID )
					     :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateEndRadiusObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateEndRadiusResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateStartAngleClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateStartAngleClass, SVEvaluateStartAngleClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateStartAngleClass::SVEvaluateStartAngleClass( SVObjectClass* POwner, int StringResourceID )
					       :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateStartAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateStartAngleResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEvaluateEndAngleClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVEvaluateEndAngleClass, SVEvaluateEndAngleClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateEndAngleClass::SVEvaluateEndAngleClass( SVObjectClass* POwner, int StringResourceID )
					     :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateEndAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateEndAngleResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEvaluate.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:32:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:27:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Jun 2010 14:19:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Dec 2009 10:21:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Jul 2005 12:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Jun 2005 08:07:56   ebeyeler
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
 *    Rev 1.7   17 Feb 2005 14:04:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Feb 2005 14:06:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 09:25:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jan 2003 15:42:34   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage method to update image depth on a create all objects message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 19:19:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Nov 2002 12:51:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 01 2000 12:00:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new evaluate classes:
 * SVEvaluateCenterXClass, SVEvaluateCenterYClass, 
 * SVEvaluateStartRadiusClass, SVEvaluateEndRadiusClass,
 * SVEvaluateStartAngleClass, SVEvaluateEndAngleClass
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:55:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Base Class for Evaluation classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
