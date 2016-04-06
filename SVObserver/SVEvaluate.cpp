//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:38  $
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


DWORD_PTR SVEvaluateClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

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

SVEvaluateTranslationXClass::~SVEvaluateTranslationXClass()
{
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

SVEvaluateTranslationYClass::~SVEvaluateTranslationYClass()
{
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

SVEvaluateRotationXClass::~SVEvaluateRotationXClass()
{
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

SVEvaluateRotationYClass::~SVEvaluateRotationYClass()
{
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

SVEvaluateRotationAngleClass::~SVEvaluateRotationAngleClass()
{
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

SVEvaluateCenterXClass::~SVEvaluateCenterXClass()
{
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

SVEvaluateCenterYClass::~SVEvaluateCenterYClass()
{
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

SVEvaluateStartRadiusClass::~SVEvaluateStartRadiusClass()
{
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

SVEvaluateEndRadiusClass::~SVEvaluateEndRadiusClass()
{
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

SVEvaluateStartAngleClass::~SVEvaluateStartAngleClass()
{
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

SVEvaluateEndAngleClass::~SVEvaluateEndAngleClass()
{
}