//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileTool
//* .File Name       : $Workfile:   SVToolProfile.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:13:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolProfile.h"

#include "SVUnaryImageOperatorList.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVProfileToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVProfileToolClass, SVProfileToolClassGuid );


//******************************************************************************
// Constructor(s):
//******************************************************************************


SVProfileToolClass::SVProfileToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVProfileToolClass::init()
{

	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolProfileObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &projectAngle, SVProjectAngleObjectGuid, IDS_OBJECTNAME_PROJECT_ANGLE, false, SVResetItemTool );

	// Set Embedded defaults
	projectAngle.SetDefaultValue( 0.0, TRUE );

	// Populate the available analyzer list
	SVClassInfoStruct analyzerClassInfo;
	
	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinePixelCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVPIXELCOUNTINGLINEANALYZER );
	availableChildren.Add( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGECOUNTINGLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineEdgePositionAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGEPOSITIONLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMeasurementLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMEASUREMENTANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
		
	// Build an operator list...
	// ...use Standard image operator list, because we need an output image! RO_20Mar2000
	SVUnaryImageOperatorListClass* pOperatorList = new SVStdImageOperatorListClass;
	// Operator list defaults:
	if( pOperatorList )
	{
		// Requires a SVThresholdClass Object
		pOperatorList->Add( new SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass Object
		pOperatorList->Add( new SVUserMaskOperatorClass( pOperatorList ) );

		// Add the UnaruyImageOperatorList to the Tool's List
		Add( pOperatorList );
	}

}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVProfileToolClass::~SVProfileToolClass()
{
	CloseObject();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVProfileToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		double projAngle;
		projectAngle.GetValue( projAngle );
		propertyCount	= 2;
		pPropertyArray	= new SVToolPropertyEntryStruct[ propertyCount ];
		if( pPropertyArray )
		{
			pPropertyArray[0].StrName	 = _T( "Column Profile" );
			pPropertyArray[0].DWValue	 = ( projAngle == 0.0 );
			pPropertyArray[0].ID		 = IDC_CHECK1;
			pPropertyArray[0].RadioStart = IDC_CHECK1;
			pPropertyArray[0].RadioEnd	 = IDC_CHECK2;

			pPropertyArray[1].StrName	 = _T( "Row Profile" );
			pPropertyArray[1].DWValue	 = ( projAngle == 90.0 );
			pPropertyArray[1].ID		 = IDC_CHECK2;
			pPropertyArray[1].RadioStart = IDC_CHECK1;
			pPropertyArray[1].RadioEnd	 = IDC_CHECK2;

			bOk = TRUE;
		}
		else
			propertyCount = 0;

	}

	// Set / Reset Printable Flags
	projectAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVProfileToolClass::CloseObject()
{
	BOOL bRetVal =  SVToolClass::CloseObject();

	if( pPropertyArray )
	{
		delete [] pPropertyArray;
		pPropertyArray = NULL;
		propertyCount = 0;
	}

	return bRetVal;
}

bool SVProfileToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVProfileToolClass::ResetObject()
{
	HRESULT l_Status = SVToolClass::ResetObject();

	UpdateImageWithExtent( 1 );

	return l_Status;
}

BOOL SVProfileToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	return SVToolClass::onRun( RRunStatus );
}

