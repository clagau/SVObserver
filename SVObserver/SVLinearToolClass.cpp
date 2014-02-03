// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass
// * .File Name       : $Workfile:   SVLinearToolClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Feb 2014 11:15:38  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearToolClass.h"

#include "SVImageLibrary/SVDrawContext.h"

#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVLinearImageOperatorList.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVLinearToolClass, SVLinearToolClassGuid );

SVLinearToolClass::SVLinearToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVLinearToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVLinearToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svRotationPointX, SVRotationPointXObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_X, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &m_svRotationPointY, SVRotationPointYObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &m_voProfileOrientation, SVProfileOrientationGuid,IDS_OBJECTNAME_LINEARTOOL_ORIENTATION, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_voUseProfileRotation, SVLinearToolUseRotationGuid, IDS_OBJECTNAME_LINEAR_TOOL_USE_ROTATION, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	// Set Embedded defaults
	m_svRotationAngle.SetDefaultValue( 0.0, TRUE );
	m_svRotationPointX.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_LEFT, TRUE );
	m_svRotationPointY.SetDefaultValue( ( SV_DEFAULT_WINDOWTOOL_TOP + ( SV_DEFAULT_WINDOWTOOL_HEIGHT / 2 ) ), TRUE );
	m_voProfileOrientation.SetEnumTypes(g_strOrientationEnums);
	m_voProfileOrientation.SetDefaultValue( "Horizontal", TRUE );
	m_voUseProfileRotation.SetDefaultValue(TRUE,TRUE);

	m_svToolExtent.SetImageType( SVImageTypePhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationProfile );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, &m_svRotationPointX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, &m_svRotationPointY );
	m_svToolExtent.SetExtentObject( SVExtentPropertyRotationAngle, &m_svRotationAngle );

	// Populate the available analyzer list
	SVClassInfoStruct analyzerClassInfo;
	
	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearPixelCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARPIXELCOUNTINGLINEANALYZER );
	availableChildren.Add( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEAREDGECOUNTINGLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearEdgePositionAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMeasurementAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMAXIMUMOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
		
	// Build an operator list...

	// ...use In Place image operator list, because we already have an output image! SEJ_24Mar2000
//	SVUnaryImageOperatorListClass* pOperatorList = new SVInPlaceImageOperatorListClass;
	SVUnaryImageOperatorListClass* pOperatorList = new SVLinearImageOperatorListClass;

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

SVLinearToolClass::~SVLinearToolClass()
{
	CloseObject();
}

BOOL SVLinearToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	bOk = SVToolClass::CreateObject( PCreateStructure );

	BOOL l_bValue = FALSE;

	if( m_voUseProfileRotation.GetValue( l_bValue ) == S_OK )
	{
		if( l_bValue )
		{
			bOk &= m_svToolExtent.SetTranslation( SVExtentTranslationProfile ) == S_OK;
		}
		else
		{
			bOk &= m_svToolExtent.SetTranslation( SVExtentTranslationProfileShift ) == S_OK;
		}
	}

	m_svRotationAngle.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE; 
	m_svRotationPointX.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_svRotationPointY.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_voProfileOrientation.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_voUseProfileRotation.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	extentLeft.ObjectAttributesAllowedRef() &= ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentTop.ObjectAttributesAllowedRef() &= ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

BOOL SVLinearToolClass::CloseObject()
{
	BOOL bRetVal =  SVToolClass::CloseObject();

	return bRetVal;
}

HRESULT SVLinearToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	long l_lValue = 0;
	BOOL l_bValue = FALSE;

	if( m_voUseProfileRotation.GetValue( l_bValue ) == S_OK )
	{
		long l_DataIndex = m_voUseProfileRotation.GetLastSetIndex();

		if( l_bValue )
		{
			if( m_svToolExtent.GetTranslation() != SVExtentTranslationProfile )
			{
				m_voProfileOrientation.SetValue( l_DataIndex, "Horizontal" );

				l_hrOk = m_svToolExtent.SetTranslation( SVExtentTranslationProfile, l_DataIndex );
			}
		}
		else
		{
			if( m_svToolExtent.GetTranslation() != SVExtentTranslationProfileShift )
			{
				m_svRotationAngle.SetValue( l_DataIndex, 0.0 );

				l_hrOk = m_svToolExtent.SetTranslation( SVExtentTranslationProfileShift, l_DataIndex );
			}
		}

		SVExtentPropertyInfoStruct info;

		if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyRotationAngle, info ) == S_OK )
		{
			info.bHidden = ! l_bValue;

			if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyRotationAngle, info ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( SVToolClass::ResetObject() != S_OK )
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
					extentLeft.SetValue( 1, l_svFigure.m_svTopLeft.m_dPositionX );
				}
				
				if( extentTop.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					extentTop.SetValue( 1, l_svFigure.m_svTopLeft.m_dPositionY );
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

SVTaskObjectClass *SVLinearToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
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

HRESULT SVLinearToolClass::DoesObjectHaveExtents() const
{
	return S_OK;
}

BOOL SVLinearToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	return SVToolClass::onRun( RRunStatus );
}

LONG_PTR SVLinearToolClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

CString SVLinearToolClass::GetProfileOrientation()
{
	CString sRet;

	m_voProfileOrientation.GetValue(sRet);

	return sRet;
}

BOOL SVLinearToolClass::GetRotation()
{
	BOOL bVal = FALSE;
	
	m_voUseProfileRotation.GetValue(bVal);

	return bVal;
}

BOOL SVLinearToolClass::IsToolRotated()
{
	BOOL bRet = FALSE;
	SVImageExtentClass svImageExtent;
	double dRotationAngle;

	GetImageExtent(svImageExtent);
	svImageExtent.GetExtentProperty(SVExtentPropertyRotationAngle,dRotationAngle);

	if ( dRotationAngle != 0 )
	{
		bRet = TRUE;
	}
	return bRet;
}

HRESULT SVLinearToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageNames;
	return S_OK;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLinearToolClass.cpp_v  $
 * 
 *    Rev 1.3   01 Feb 2014 11:15:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Aug 2013 10:13:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVThresholdClass.h and SVUserMaskOperatorClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   18 Apr 2013 16:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  821
 * SCR Title:  Fix issue with Linear Tool when Enabling Rotation
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Linear tool Extent Translation funtioanlity to Tool Extent Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:04:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Jul 2012 09:01:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Jul 2012 17:15:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Mar 2011 16:24:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Mar 2011 12:11:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Mar 2011 13:40:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Dec 2010 12:56:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Nov 2010 13:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Jun 2010 14:44:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 11:28:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Mar 2008 16:05:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * and
 * Added Attributes to support Image Definition Lists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Aug 2005 16:07:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jul 2005 12:39:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2005 08:13:02   ebeyeler
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
 *    Rev 1.1   07 Mar 2005 14:27:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element for hiding rotation angle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:49:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
