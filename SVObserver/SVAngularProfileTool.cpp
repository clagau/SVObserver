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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAngularProfileTool.cpp_v  $
 * 
 *    Rev 1.5   15 May 2014 10:09:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Feb 2014 10:16:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Aug 2013 09:46:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVThresholdClass.h and SVUserMaskOperatorClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2013 19:45:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   10 May 2013 11:32:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnValidate override that will always return FALSE.  Re-added original reset object code and added a call to SetInvalid().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   25 Apr 2013 16:02:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  746
 * SCR Title:  Fix SVObserver crash due to discrete outputs after deleting a PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   change ResetObject to return S_FALSE because the tool is no longer able to be used
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:32:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   25 Mar 2013 13:35:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed OnRun to make tool invalid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   16 Jul 2012 08:55:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   12 Jul 2012 14:37:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   02 Jul 2012 16:11:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   18 Jun 2012 17:47:32   jspila
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
 *    Rev 1.37   30 Mar 2011 16:20:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   21 Mar 2011 11:52:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Mar 2011 13:19:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   11 Mar 2011 14:15:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   25 Feb 2011 12:07:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   10 Feb 2011 14:24:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   27 Jan 2011 10:47:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   08 Dec 2010 07:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   04 Nov 2010 13:18:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   01 Jun 2010 13:39:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   15 Dec 2009 14:55:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   07 Mar 2008 11:28:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   25 Jul 2007 08:39:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   21 Jun 2007 11:51:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   19 Feb 2007 15:47:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   15 Feb 2007 11:40:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror function to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   02 Nov 2005 07:33:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   08 Aug 2005 15:22:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   29 Jul 2005 11:58:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 Jun 2005 08:02:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   17 Feb 2005 12:55:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to use the new extent methodology and new drawing methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Feb 2005 13:06:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   13 Jul 2004 10:11:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  409
 * SCR Title:  Fix loading of a configuration from the Run directory.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   In the re-size methods, it will now re-size the threshold object also.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   18 Jul 2003 13:37:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetToolPosition method to fix infinite loop when changing height.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   08 Jul 2003 10:42:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new code the processing of InputRequests (from SetToolParameterList) to verify that extents have changed or the change is ignored.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Apr 2003 16:45:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   07 Mar 2003 11:03:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  333
 * SCR Title:  Profile shows wrong image with no analyzr & is run even with false conditional
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to profile tool to always show the correct output image, regardless if it is conditionally disabled or rotated without an analyzer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   05 Mar 2003 12:13:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Jan 2003 10:38:50   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, onUpdateFigure, processMessage, GetSpecialPropertyList, onRun, SetShowExtents, updateImageWidth, move, sizeL, sizeR, sizeT, sizeB, sizeRT, sizeLB, sizeLT and sizeRB methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Jan 2003 13:49:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a member variable to hold the extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Jan 2003 13:44:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Jan 2003 15:21:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added the SetToolPosition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Dec 2002 18:40:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Nov 2002 10:25:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   File has changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Oct 2001 13:17:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jun 2000 16:24:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  159
 * SCR Title:  Incorrect placement of Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added message handler for SVM_IMAGE_SOURCE_CHANGED.
 * Revised OnRun to set the rotation point in the source image
 * properly when not using the main camera image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 May 2000 08:53:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  140
 * SCR Title:  Angular Profile Tool problem with using adjust position dialog
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised isLResizable to restricted sizing on the left if a line analyzer is present.
 * Revised isLTResizable to restricted sizing on the left/top if a line analyzer is present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2000 11:52:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in. New Class for the Angular Profile Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
