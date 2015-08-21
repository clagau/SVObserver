// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearMeasurementAnalyzerClass
// * .File Name       : $Workfile:   SVLinearMeasurementAnalyzerClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:04:02  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearMeasurementAnalyzerClass.h"
#include "SVMeasureAnalyzerAdjustment.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVMessage/SVMessage.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS( SVLinearMeasurementAnalyzerClass, SVLinearMeasurementAnalyzerClassGuid );

SVLinearMeasurementAnalyzerClass::SVLinearMeasurementAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = TRUE;
	m_bEnableEdgeSelect = TRUE;
	m_bEnablePolarisation = TRUE;
	m_bEnablePosition = TRUE;
	m_bEnableThreshold = TRUE;

	init();
}

void SVLinearMeasurementAnalyzerClass::init()
{
	// Identify our type
	outObjectInfo.ObjectTypeInfo.SubType = SVLinearMeasurementAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SVResetItemNone );


    //
	// Set Edge defaults

	SVLinearEdgeProcessingClass *l_pEdgeA = new SVLinearEdgeAProcessingClass(this);
	SVLinearEdgeProcessingClass *l_pEdgeB = new SVLinearEdgeBProcessingClass(this);

	if( l_pEdgeA != NULL )
	{
		AddFriend(l_pEdgeA->GetUniqueObjectID());
	}

	if( l_pEdgeB != NULL )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue( SV_LAST_EDGE, TRUE );
	}

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	mdpEdgeA.SetDefaultValue( defaultPoint, TRUE );
	mdpEdgeB.SetDefaultValue( defaultPoint, TRUE );
	mdpCenter.SetDefaultValue( defaultPoint, TRUE );
	mdWidth.SetDefaultValue( 0.0, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge A Results...
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the Sub-pixel EdgeA X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge B Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeBObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel EdgeB X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Center Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPCenterObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel Center X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Sub-pixel Center Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Width Result...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDWidthObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel Width Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.ClassId = SVDoubleResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DWIDTH );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.ClassId = SVDoubleResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A );
	strTitle.LoadString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeBObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.ClassId = SVDoubleResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B );
	strTitle.LoadString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

}


SVLinearMeasurementAnalyzerClass::~SVLinearMeasurementAnalyzerClass()
{
	CloseObject();
}


BOOL SVLinearMeasurementAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	mdpEdgeA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpEdgeB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpCenter.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_svLinearDistanceA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svLinearDistanceB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVLinearMeasurementAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}



BOOL SVLinearMeasurementAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = SVLinearAnalyzerClass::onRun( RRunStatus );

	SVDPointClass l_svDPointA, l_svDPointB;
	SVExtentPointStruct l_svEdgePointA;
	SVExtentPointStruct l_svEdgePointB;
	SVDPointClass l_svCenterPoint;
	double l_dDistanceA = 0.0;
	double l_dDistanceB = 0.0;

	l_bOk = l_bOk && GetEdgeA() != NULL && GetEdgeB() != NULL && GetTool() != NULL;

	if ( l_bOk )
	{
		SVImageExtentClass l_svExtents;

		if ( GetEdgeA()->GetOutputEdgePoint(l_svEdgePointA) != S_OK )
		{
			RRunStatus.SetFailed();
		}

		if ( GetEdgeA()->GetOutputEdgeDistance(l_dDistanceA) != S_OK )
		{
			RRunStatus.SetFailed();
		}

		if ( GetEdgeB()->GetOutputEdgePoint(l_svEdgePointB) != S_OK )
		{
			RRunStatus.SetFailed();
		}

		if ( GetEdgeB()->GetOutputEdgeDistance(l_dDistanceB) != S_OK )
		{
			RRunStatus.SetFailed();
		}

		l_bOk &= GetImageExtent( l_svExtents ) == S_OK &&
				l_svExtents.TranslateFromOutputSpace(l_svEdgePointA, l_svEdgePointA) == S_OK &&
				l_svExtents.TranslateFromOutputSpace(l_svEdgePointB, l_svEdgePointB) == S_OK;

		l_bOk &= GetTool()->GetImageExtent( l_svExtents ) == S_OK &&
				l_svExtents.TranslateFromOutputSpace(l_svEdgePointA, l_svEdgePointA) == S_OK &&
				l_svExtents.TranslateFromOutputSpace(l_svEdgePointB, l_svEdgePointB) == S_OK;

		l_svDPointA.x = l_svEdgePointA.m_dPositionX;
		l_svDPointA.y = l_svEdgePointA.m_dPositionY;

		l_svDPointB.x = l_svEdgePointB.m_dPositionX;
		l_svDPointB.y = l_svEdgePointB.m_dPositionY;


		l_svCenterPoint.x = l_svDPointA.x + ((l_svDPointB.x - l_svDPointA.x)/2.0);
		l_svCenterPoint.y = l_svDPointA.y + ((l_svDPointB.y - l_svDPointA.y)/2.0);

		double l_dWidth = 0.0;

		l_dWidth = l_dDistanceB - l_dDistanceA + 1.0;

		l_bOk = ( m_svLinearDistanceA.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceA) == S_OK ) && l_bOk;
		l_bOk = ( m_svLinearDistanceB.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceB) == S_OK ) && l_bOk;
		
		l_bOk = ( mdpEdgeA.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointA) == S_OK ) && l_bOk;
		l_bOk = ( mdpEdgeB.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointB) == S_OK ) && l_bOk;
		l_bOk = ( mdpCenter.SetValue(RRunStatus.m_lResultDataIndex, l_svCenterPoint) == S_OK ) && l_bOk;
		l_bOk = ( mdWidth.SetValue(RRunStatus.m_lResultDataIndex, fabs( l_dWidth ) ) == S_OK ) && l_bOk;
	}

	if ( !l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}

BOOL SVLinearMeasurementAnalyzerClass::OnValidate()
{
	BOOL retVal = FALSE;
	
	// Validate our local needs
	if( NULL )
	{
		SetInvalid();
	}
	else
	{
		retVal = TRUE;

		// Validate our base class needs
		retVal = SVLinearAnalyzerClass::OnValidate() && retVal;

		// Set this object and all objects it owns, to invalid
		if( !retVal )
			SetInvalid();
	}
	return retVal;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLinearMeasurementAnalyzerClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:04:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Aug 2012 13:01:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Jan 2011 11:28:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Dec 2010 12:56:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   01 Jun 2010 14:44:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 11:28:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   03 Sep 2009 10:29:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Aug 2005 13:11:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   put fabs back in for dWidth ( error made when transitioning value object from BOOL to HRESULT )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jul 2005 12:39:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2005 08:13:00   ebeyeler
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
 *    Rev 1.4   22 Mar 2005 07:52:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
