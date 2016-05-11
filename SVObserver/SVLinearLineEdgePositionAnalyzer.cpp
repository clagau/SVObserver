// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgePositionLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLineEdgePositionAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:59:10  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineEdgePositionAnalyzer.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVTool.h"
#include "SVGlobal.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearEdgePositionLineAnalyzerClass, SVLinearEdgePositionLineAnalyzerClassGuid );

SVLinearEdgePositionLineAnalyzerClass::SVLinearEdgePositionLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								:SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVLinearEdgePositionLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLinearEdgePositionAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdge = new SVLinearEdgeAProcessingClass( this );

	AddFriend( l_pEdge->GetUniqueObjectID() );

	RegisterEmbeddedObject( &dpEdge, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistance, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SVResetItemNone );

	m_bEnableDirection = TRUE;
	m_bEnableEdgeSelect = TRUE;
	m_bEnablePolarisation = TRUE;
	m_bEnablePosition = TRUE;
	m_bEnableThreshold = TRUE;

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;

	dpEdge.SetDefaultValue( defaultPoint, TRUE );

	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge Result...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	
	// Add the Sub-pixel Edge X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Add the Sub-pixel Edge Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
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

	// Set default inputs and outputs
	addDefaultInputObjects();
}


SVLinearEdgePositionLineAnalyzerClass::~SVLinearEdgePositionLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVLinearEdgePositionLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	dpEdge.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;	// Point
	m_svLinearDistance.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearEdgePositionLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}

BOOL SVLinearEdgePositionLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVDPointClass l_svDPoint;
	SVExtentPointStruct l_svEdgePoint;
	double l_dDistance = 0.0;

	// All inputs and outputs must be validated first
	BOOL l_bOk = SVLinearAnalyzerClass::onRun( RRunStatus );

	l_bOk = l_bOk && nullptr != GetEdgeA() && nullptr != GetTool();

	if( l_bOk )
	{
		SVImageExtentClass l_svExtents;

		if( S_OK != GetEdgeA()->GetOutputEdgePoint( l_svEdgePoint ) )
		{
			RRunStatus.SetFailed();	
		}

		if( S_OK != GetEdgeA()->GetOutputEdgeDistance( l_dDistance ) )
		{
			RRunStatus.SetFailed();	
		}

		l_bOk &= S_OK == GetImageExtent( l_svExtents ) &&
		         S_OK == l_svExtents.TranslateFromOutputSpace( l_svEdgePoint, l_svEdgePoint );

		l_bOk &= S_OK == GetTool()->GetImageExtent( l_svExtents ) &&
		         S_OK == l_svExtents.TranslateFromOutputSpace( l_svEdgePoint, l_svEdgePoint );
	}

	l_svDPoint.x = l_svEdgePoint.m_dPositionX;
	l_svDPoint.y = l_svEdgePoint.m_dPositionY;

	l_bOk = ( S_OK == m_svLinearDistance.SetValue( RRunStatus.m_lResultDataIndex, l_dDistance ) ) && l_bOk;
	l_bOk = ( S_OK == dpEdge.SetValue( RRunStatus.m_lResultDataIndex, l_svDPoint ) ) && l_bOk;
	
	if( ! l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();	
	}

	return l_bOk;
}

BOOL SVLinearEdgePositionLineAnalyzerClass::OnValidate()
{
	BOOL retVal = SVLinearAnalyzerClass::OnValidate();
	
	retVal &= nullptr != GetEdgeA();

	if( ! retVal )
	{
		SetInvalid();
	}

	return retVal;
}

