//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAngularProfileTool
//* .File Name       : $Workfile:   SVAngularProfileTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 10:09:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVAngularProfileTool.h"
#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "SVUnaryImageOperatorList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PI 3.1415926535
#define DEGREES_TO_RADIANS PI / 180.0
#define RADIANS_TO_DEGREES 180.0 / PI

SV_IMPLEMENT_CLASS( SVAngularProfileToolClass, SVAngularProfileToolClassGuid );

SVAngularProfileToolClass::SVAngularProfileToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVAngularProfileToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolProfileObjectType;

	// Image Input
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "AngularProfileToolImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &projectAngle, SVProjectAngleObjectGuid, IDS_OBJECTNAME_PROJECT_ANGLE, false, SVResetItemNone, _T("Extent Angle") );

	RegisterEmbeddedObject( &profileImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_svRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svRotationPointX, SVRotationPointXObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_X, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &m_svRotationPointY, SVRotationPointYObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SVResetItemTool, _T("Extent Y") );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

 	// Set Embedded defaults
	projectAngle.SetDefaultValue( 0.0, TRUE );
	m_svRotationAngle.SetDefaultValue( 0.0, TRUE );
	m_svRotationPointX.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_LEFT, TRUE );
	m_svRotationPointY.SetDefaultValue( ( SV_DEFAULT_WINDOWTOOL_TOP + ( SV_DEFAULT_WINDOWTOOL_HEIGHT / 2 ) ), TRUE );

	m_svToolExtent.SetImageType( SVImageTypePhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationProfile );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, &m_svRotationPointX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, &m_svRotationPointY );
	m_svToolExtent.SetExtentObject( SVExtentPropertyRotationAngle, &m_svRotationAngle );

	profileImage.InitializeImage( SVImageTypePhysical );

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

	// ...use In Place image operator list, because we already have an output image! SEJ_24Mar2000
	SVUnaryImageOperatorListClass* pOperatorList = new SVInPlaceImageOperatorListClass;

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

	addDefaultInputObjects();
}

SVAngularProfileToolClass::~SVAngularProfileToolClass()
{
	CloseObject();
}

BOOL SVAngularProfileToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = true;

	bOk = SVToolClass::CreateObject( PCreateStructure );

	if( bOk )
	{
		propertyCount	= 5;
		pPropertyArray	= new SVToolPropertyEntryStruct[ propertyCount ];

		if( pPropertyArray )
		{
			// Sizing 
			//SVImageInfoClass& rImageInfo = 
			double width = 0;

			pPropertyArray[0].StrName	 = _T( "Width of 1" );
			pPropertyArray[0].DWValue	 = ( width == 1 );
			pPropertyArray[0].ID		 = IDC_CHECK1;
			pPropertyArray[0].RadioStart = IDC_CHECK1;
			pPropertyArray[0].RadioEnd	 = IDC_CHECK5;

			pPropertyArray[1].StrName	 = _T( "Width of 2" );
			pPropertyArray[1].DWValue	 = ( width == 4 );
			pPropertyArray[1].ID		 = IDC_CHECK2;
			pPropertyArray[1].RadioStart = IDC_CHECK1;
			pPropertyArray[1].RadioEnd	 = IDC_CHECK5;

			pPropertyArray[2].StrName	 = _T( "Width of 4" );
			pPropertyArray[2].DWValue	 = ( width == 4 );
			pPropertyArray[2].ID		 = IDC_CHECK3;
			pPropertyArray[2].RadioStart = IDC_CHECK1;
			pPropertyArray[2].RadioEnd	 = IDC_CHECK5;

			pPropertyArray[3].StrName	 = _T( "Width of 8" );
			pPropertyArray[3].DWValue	 = ( width == 8 );
			pPropertyArray[3].ID		 = IDC_CHECK4;
			pPropertyArray[3].RadioStart = IDC_CHECK1;
			pPropertyArray[3].RadioEnd	 = IDC_CHECK5;

			pPropertyArray[4].StrName	 = _T( "Width of 16" );
			pPropertyArray[4].DWValue	 = ( width == 16 );
			pPropertyArray[4].ID		 = IDC_CHECK5;
			pPropertyArray[4].RadioStart = IDC_CHECK1;
			pPropertyArray[4].RadioEnd	 = IDC_CHECK5;

			SVImageExtentClass l_svExtents = profileImage.GetImageExtents();

			SVExtentPointStruct l_svPosition;

			double l_dAngle = 0.0;

			if ( l_svExtents.GetExtentProperty( SVExtentPropertyOldRotationAngle, l_dAngle ) == S_OK )
			{
				m_svRotationAngle.SetValue( 1, l_dAngle );
			}

			if ( l_dAngle == 0.0 )
			{
				long l_lHeight = 0;

				if ( l_svExtents.GetExtentProperty( SVExtentPropertyOldAbsPositionPoint, l_svPosition ) == S_OK &&
					   l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_lHeight ) == S_OK )
				{
					l_svPosition.m_dPositionY += l_lHeight / 2;

					m_svRotationPointX.SetValue( 1, l_svPosition.m_dPositionX );
					m_svRotationPointY.SetValue( 1, l_svPosition.m_dPositionY );
				}
			}
			else
			{
				if ( l_svExtents.GetExtentProperty( SVExtentPropertyOldRotationPoint, l_svPosition ) == S_OK )
				{
					m_svRotationPointX.SetValue( 1, l_svPosition.m_dPositionX );
					m_svRotationPointY.SetValue( 1, l_svPosition.m_dPositionY );
				}
			}
		}
		else
		{
			propertyCount = 0;
		}
	}

	bOk &= ( profileImage.InitializeImage( getInputImage() ) == S_OK );

	projectAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_svRotationAngle.ObjectAttributesAllowedRef() &= ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_svRotationPointX.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_svRotationPointY.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	extentLeft.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentTop.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentRight.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentBottom.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentWidth.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentHeight.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;


	isCreated = bOk;

	return bOk;
}

BOOL SVAngularProfileToolClass::CloseObject()
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

HRESULT SVAngularProfileToolClass::ResetObject()
{
	HRESULT l_hrOk = profileImage.InitializeImage( getInputImage() );

	if ( SVToolClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( l_hrOk == S_OK )
	{
		SVImageExtentClass l_svExtents;

		if ( GetImageExtent( l_svExtents ) == S_OK )
		{
			SVExtentFigureStruct l_svFigure;

			if ( l_svExtents.GetFigure( l_svFigure ) == S_OK )
			{
				if( extentLeft.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					long l_lValue = static_cast<long>(l_svFigure.m_svTopLeft.m_dPositionX);

					extentLeft.SetValue( 1, l_lValue );
				}
				
				if( extentTop.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					long l_lValue = static_cast<long>(l_svFigure.m_svTopLeft.m_dPositionY);

					extentTop.SetValue( 1, l_lValue );
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	UpdateImageWithExtent( 1 );

	SetInvalid();
	return l_hrOk;
}

HRESULT SVAngularProfileToolClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL )
	{
		if ( p_psvImage == getInputImage() )
		{
			l_hrOk = S_OK;
		}
		else
		{
			l_hrOk = SVToolClass::IsInputImage( p_psvImage );
		}
	}

	return l_hrOk;
}

SVTaskObjectClass *SVAngularProfileToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = NULL;

	if( m_svToolExtent.GetImageExtent( l_svExtents ) == S_OK &&
	    l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVAngularProfileToolClass::DoesObjectHaveExtents() const
{
	return true;
}

DWORD_PTR SVAngularProfileToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// SEJ June 13,2000
		// Added so that the Angular profile tool will adjust it's image extent when
		// the source image changes
		case SVMSGID_IMAGE_SOURCE_CHANGED:
		{
			// Get the Output Image Extent
			SVImageExtentClass l_svExtents;
      
			long l_lHeight = 0;

			if( m_svToolExtent.GetImageExtent( l_svExtents ) == S_OK &&
   				l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_lHeight ) == S_OK )
			{
				// Set Rotation Point
				if( l_svExtents.SetExtentProperty( SVExtentPropertyPositionPointX, SV_DEFAULT_WINDOWTOOL_LEFT ) == S_OK &&
				    l_svExtents.SetExtentProperty( SVExtentPropertyPositionPointY, ( SV_DEFAULT_WINDOWTOOL_TOP + ( l_lHeight / 2 ) ) ) == S_OK &&
				    m_svToolExtent.SetImageExtent( 1, l_svExtents ) == S_OK )
				{
					return SVMR_SUCCESS;
				}
			}

			return SVMR_NO_SUCCESS;
		}
		break;
	}
	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

SVToolPropertyEntryStruct* SVAngularProfileToolClass::GetSpecialPropertyList( int& RCount )
{
	RCount = propertyCount;

	// initialize the width variables
	SVImageClass* pOutputImage = getOutputImage();
	if( pOutputImage )
	{
		long l_lHeight = 0;

		extentHeight.GetValue( l_lHeight );

		// clear all selections
		pPropertyArray[ 0 ].DWValue = FALSE;
		pPropertyArray[ 1 ].DWValue = FALSE;
		pPropertyArray[ 2 ].DWValue = FALSE;
		pPropertyArray[ 3 ].DWValue = FALSE;
		pPropertyArray[ 4 ].DWValue = FALSE;

		switch ( l_lHeight )
		{
			case 1:
				pPropertyArray[ 0 ].DWValue = TRUE;
				break;
			case 2:
				pPropertyArray[ 1 ].DWValue = TRUE;
				break;
			case 4:
				pPropertyArray[ 2 ].DWValue = TRUE;
				break;
			case 8:
				pPropertyArray[ 3 ].DWValue = TRUE;
				break;
			case 16:
				pPropertyArray[ 4 ].DWValue = TRUE;
				break;
			default:
				break;
		}

	}
	return pPropertyArray;
}

SVObjectClass* SVAngularProfileToolClass::getImageToLineProject()
{
	SVObjectTypeInfoStruct objectInfo;

	objectInfo.ObjectType = SVImageToLineProjectObjectType;

	SVObjectClass* pProject = reinterpret_cast<SVObjectClass *>(::SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ) );

	return pProject;
}

SVLineClass* SVAngularProfileToolClass::getOutputLine()
{
	SVObjectClass* pProject = getImageToLineProject();
	if( pProject )
	{
		// Get the Line
		SVObjectTypeInfoStruct objectInfo;
		
		objectInfo.ObjectType = SVLineObjectType;

		SVLineClass* pLine = reinterpret_cast<SVLineClass *>(::SVSendMessage( pProject, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ) );
	
		return pLine;
	}
	return NULL;
}

SVImageClass* SVAngularProfileToolClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVAngularProfileToolClass::getOutputImage()
{
	return &profileImage;
}

BOOL SVAngularProfileToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

SVStaticStringValueObjectClass* SVAngularProfileToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}
BOOL SVAngularProfileToolClass::OnValidate()
{
	SetInvalid();
	return FALSE;
}

