// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearMaximumBackgroundObjectLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLineMaximumBackgroundObjectAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 14:57:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineMaximumBackgroundObjectAnalyzer.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVGlobal.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMaximumBackgroundObjectLineAnalyzerClass, SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid );

SVLinearMaximumBackgroundObjectLineAnalyzerClass::SVLinearMaximumBackgroundObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = FALSE;
	m_bEnableEdgeSelect = FALSE;
	m_bEnablePolarisation = FALSE;
	m_bEnablePosition = FALSE;
	m_bEnableThreshold = TRUE;

	init();
}

void SVLinearMaximumBackgroundObjectLineAnalyzerClass::init()
{
	// Identify our type
	outObjectInfo.ObjectTypeInfo.SubType = SVLineMaximumBackgroundObjectAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdgeA = new SVLinearEdgeAProcessingClass(this);
	SVLinearEdgeProcessingClass *l_pEdgeB = new SVLinearEdgeBProcessingClass(this);

	if( l_pEdgeA != NULL )
	{
		AddFriend(l_pEdgeA->GetUniqueObjectID());

		l_pEdgeA->m_svPolarisation.SetDefaultValue( SV_NEGATIVE_POLARISATION, TRUE );

		l_pEdgeA->m_svEdgeSelect.SetDefaultValue( SV_THIS_EDGE, TRUE );
		l_pEdgeA->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, TRUE );
	}

	if( l_pEdgeB != NULL )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svPolarisation.SetDefaultValue( SV_POSITIVE_POLARISATION, TRUE );

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue( SV_THIS_EDGE, TRUE );
		l_pEdgeB->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, TRUE );
	}

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SVResetItemNone );

	// Set Edge defaults

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


SVLinearMaximumBackgroundObjectLineAnalyzerClass::~SVLinearMaximumBackgroundObjectLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVLinearMaximumBackgroundObjectLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	mdpEdgeA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpEdgeB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpCenter.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svLinearDistanceA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svLinearDistanceB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;


	isCreated = bOk;

	return bOk;
}

BOOL SVLinearMaximumBackgroundObjectLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}

HRESULT SVLinearMaximumBackgroundObjectLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	double l_dDistance = 0.0;

	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( l_hrOk == S_OK && GetEdgeA() != NULL && 
	    ( m_svLinearDistanceA.GetValue( l_dDistance ) == S_OK ) )
	{
		SVExtentLineStruct l_svLine;

		if( GetEdgeA()->GetEdgeOverlayFromDistance( l_dDistance, l_svLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_MultiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	if( l_hrOk == S_OK && GetEdgeB() != NULL &&
	    ( m_svLinearDistanceB.GetValue( l_dDistance ) == S_OK ) )
	{
		SVExtentLineStruct l_svLine;

		if( GetEdgeB()->GetEdgeOverlayFromDistance( l_dDistance, l_svLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_MultiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

BOOL SVLinearMaximumBackgroundObjectLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVImageExtentClass l_svExtents;
	std::vector<double> l_svAEdges;
	std::vector<double> l_svBEdges;
	SVDPointClass l_svDPointA, l_svDPointB;
	SVExtentPointStruct l_svEdgePointA;
	SVExtentPointStruct l_svEdgePointB;
	SVDPointClass l_svCenterPoint;
	double l_dDistanceA = 0.0;
	double l_dDistanceB = 0.0;

	BOOL l_bOk = SVLinearAnalyzerClass::onRun(RRunStatus);

	l_bOk &= GetEdgeA() != NULL && GetEdgeB() != NULL && GetTool() != NULL;
	
	l_bOk = (GetEdgeA()->m_svLinearEdges.GetValues( l_svAEdges ) == S_OK ) && l_bOk;
	l_bOk = (GetEdgeB()->m_svLinearEdges.GetValues( l_svBEdges ) == S_OK ) && l_bOk;

	if( l_bOk )
	{
		int resultAIndex = -1;
		int resultBIndex = -1;
		int offsetB = 0;

		double l_dMaxBackgroundDistance = 0.0;

		int l_iACount = static_cast<int>(l_svAEdges.size());
		int l_iBCount = static_cast<int>(l_svBEdges.size());

		// Calc maximum backgoundobject...
		for( int indexA = 0; indexA < l_iACount && indexA + offsetB < l_iBCount; ++ indexA )
		{
			// Throw lower positive edges away...
			while( l_svAEdges.at( indexA ) >= l_svBEdges.at( indexA + offsetB ) &&
					 indexA + ( ++offsetB ) < l_iBCount );

			// Check for indexB bounds...
			if( indexA + offsetB >= l_iBCount )
				break;

			if( l_svBEdges.at( indexA + offsetB ) - l_svAEdges.at( indexA ) > l_dMaxBackgroundDistance )
			{
				l_dMaxBackgroundDistance = l_svBEdges.at( indexA + offsetB ) - l_svAEdges.at( indexA );
				resultAIndex = indexA;
				resultBIndex = indexA + offsetB;
			}
		}

		if( 0 <= resultAIndex )
		{
			l_dDistanceA = l_svAEdges.at( resultAIndex );
		}
		else
		{
			RRunStatus.SetFailed();
		}

		if(	0 <= resultBIndex )
		{
			l_dDistanceB = l_svBEdges.at( resultBIndex );
		}
		else
		{
			RRunStatus.SetFailed();
		}

		l_bOk &= GetEdgeA()->GetPointFromDistance( l_dDistanceA, l_svEdgePointA ) == S_OK;
		l_bOk &= GetEdgeB()->GetPointFromDistance( l_dDistanceB, l_svEdgePointB ) == S_OK;
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

	l_dWidth = l_dDistanceB - l_dDistanceA;

	l_bOk = ( m_svLinearDistanceA.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceA) == S_OK ) && l_bOk;
	l_bOk = ( m_svLinearDistanceB.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceB) == S_OK ) && l_bOk;
	
	l_bOk = ( mdpEdgeA.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointA) == S_OK ) && l_bOk;
	l_bOk = ( mdpEdgeB.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointB) == S_OK ) && l_bOk;
	l_bOk = ( mdpCenter.SetValue(RRunStatus.m_lResultDataIndex, l_svCenterPoint) == S_OK ) && l_bOk;
	l_bOk = ( mdWidth.SetValue(RRunStatus.m_lResultDataIndex, fabs( l_dWidth ) ) == S_OK ) && l_bOk;

	if ( !l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}

BOOL SVLinearMaximumBackgroundObjectLineAnalyzerClass::OnValidate()
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

