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

#pragma region Includes
#include "stdafx.h"
#include "SVLinearMeasurementAnalyzerClass.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVMessage/SVMessage.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMeasurementAnalyzerClass, SVLinearMeasurementAnalyzerClassGuid );

SVLinearMeasurementAnalyzerClass::SVLinearMeasurementAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = true;
	m_bEnableEdgeSelect = true;
	m_bEnablePolarisation = true;
	m_bEnablePosition = true;
	m_bEnableThreshold = true;

	init();
}

void SVLinearMeasurementAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLinearMeasurementAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SVResetItemNone );

	// Set Edge defaults
	SVLinearEdgeProcessingClass *l_pEdgeA = new SVLinearEdgeAProcessingClass(this);
	SVLinearEdgeProcessingClass *l_pEdgeB = new SVLinearEdgeBProcessingClass(this);

	if( nullptr != l_pEdgeA )
	{
		AddFriend(l_pEdgeA->GetUniqueObjectID());
	}

	if( nullptr != l_pEdgeB )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue( SV_LAST_EDGE, true );
	}

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	mdpEdgeA.SetDefaultValue( defaultPoint, true );
	mdpEdgeB.SetDefaultValue( defaultPoint, true );
	mdpCenter.SetDefaultValue( defaultPoint, true );
	mdWidth.SetDefaultValue( 0.0, true );

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
	m_availableChildren.Add( resultClassInfo );
	
	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	m_availableChildren.Add( resultClassInfo );

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
	m_availableChildren.Add( resultClassInfo );
	
	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	m_availableChildren.Add( resultClassInfo );

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
	m_availableChildren.Add( resultClassInfo );
	
	// Add the Sub-pixel Center Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	m_availableChildren.Add( resultClassInfo );

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
	m_availableChildren.Add( resultClassInfo );

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
	m_availableChildren.Add( resultClassInfo );

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
	m_availableChildren.Add( resultClassInfo );
}

SVLinearMeasurementAnalyzerClass::~SVLinearMeasurementAnalyzerClass()
{
	CloseObject();
}

BOOL SVLinearMeasurementAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	mdpEdgeA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpEdgeB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpCenter.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_svLinearDistanceA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svLinearDistanceB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

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

	l_bOk = l_bOk && nullptr != GetEdgeA() && nullptr != GetEdgeB() && nullptr != GetTool();

	if ( l_bOk )
	{
		SVImageExtentClass l_svExtents;

		if ( S_OK != GetEdgeA()->GetOutputEdgePoint(l_svEdgePointA) )
		{
			RRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeA()->GetOutputEdgeDistance(l_dDistanceA) )
		{
			RRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeB()->GetOutputEdgePoint(l_svEdgePointB) )
		{
			RRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeB()->GetOutputEdgeDistance(l_dDistanceB) )
		{
			RRunStatus.SetFailed();
		}

		l_bOk &= S_OK == GetImageExtent( l_svExtents ) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(l_svEdgePointA, l_svEdgePointA) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(l_svEdgePointB, l_svEdgePointB);

		l_bOk &= S_OK == GetTool()->GetImageExtent( l_svExtents ) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(l_svEdgePointA, l_svEdgePointA) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(l_svEdgePointB, l_svEdgePointB);

		l_svDPointA.x = l_svEdgePointA.m_dPositionX;
		l_svDPointA.y = l_svEdgePointA.m_dPositionY;

		l_svDPointB.x = l_svEdgePointB.m_dPositionX;
		l_svDPointB.y = l_svEdgePointB.m_dPositionY;


		l_svCenterPoint.x = l_svDPointA.x + ((l_svDPointB.x - l_svDPointA.x) / 2.0);
		l_svCenterPoint.y = l_svDPointA.y + ((l_svDPointB.y - l_svDPointA.y) / 2.0);

		double l_dWidth = 0.0;

		l_dWidth = l_dDistanceB - l_dDistanceA + 1.0;

		l_bOk = ( S_OK == m_svLinearDistanceA.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceA) ) && l_bOk;
		l_bOk = ( S_OK == m_svLinearDistanceB.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceB) ) && l_bOk;
		
		l_bOk = ( S_OK == mdpEdgeA.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointA) ) && l_bOk;
		l_bOk = ( S_OK == mdpEdgeB.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointB) ) && l_bOk;
		l_bOk = ( S_OK == mdpCenter.SetValue(RRunStatus.m_lResultDataIndex, l_svCenterPoint) ) && l_bOk;
		l_bOk = ( S_OK == mdWidth.SetValue(RRunStatus.m_lResultDataIndex, fabs( l_dWidth ) ) ) && l_bOk;
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
	BOOL retVal = ( GetEdgeA() && GetEdgeB() );
	
	retVal = SVLinearAnalyzerClass::OnValidate() && retVal;

	// Set this object and all objects it owns, to invalid
	if( !retVal )
	{
		SetInvalid();
	}
	return retVal;
}

