// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearLineMaximumObjectAnalyzer
// * .File Name       : $Workfile:   SVLinearLineMaximumObjectAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 14:57:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineMaximumObjectAnalyzer.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVGlobal.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMaximumObjectLineAnalyzerClass, SVLinearMaximumObjectLineAnalyzerClassGuid );

SVLinearMaximumObjectLineAnalyzerClass::SVLinearMaximumObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void SVLinearMaximumObjectLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLinearMaximumObjectAnalyzerObjectType;

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

		l_pEdgeA->m_svPolarisation.SetDefaultValue( SV_POSITIVE_POLARISATION, true );

		l_pEdgeA->m_svEdgeSelect.SetDefaultValue( SV_THIS_EDGE, true );
		l_pEdgeA->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true );
	}

	if( nullptr != l_pEdgeB )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svPolarisation.SetDefaultValue( SV_NEGATIVE_POLARISATION, true );

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue( SV_THIS_EDGE, true );
		l_pEdgeB->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true );
	}

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	mdpEdgeA.SetDefaultValue( defaultPoint, true );
	mdpEdgeB.SetDefaultValue( defaultPoint, true );
	mdpCenter.SetDefaultValue( defaultPoint, true );
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

SVLinearMaximumObjectLineAnalyzerClass::~SVLinearMaximumObjectLineAnalyzerClass()
{
	CloseObject();
}

BOOL SVLinearMaximumObjectLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	mdpEdgeA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpEdgeB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpCenter.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_svLinearDistanceA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svLinearDistanceB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearMaximumObjectLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}

HRESULT SVLinearMaximumObjectLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	double l_dDistance = 0.0;

	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( S_OK == l_hrOk && nullptr != GetEdgeA() && 
	    ( S_OK == m_svLinearDistanceA.GetValue( l_dDistance ) ) )
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeA()->GetEdgeOverlayFromDistance( l_dDistance, l_svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_MultiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	if( S_OK == l_hrOk && nullptr != GetEdgeB() &&
	    ( S_OK == m_svLinearDistanceB.GetValue( l_dDistance ) ) )
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeB()->GetEdgeOverlayFromDistance( l_dDistance, l_svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_MultiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}
	return l_hrOk;
}

BOOL SVLinearMaximumObjectLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
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

	l_bOk = l_bOk && nullptr != GetEdgeA() && nullptr != GetEdgeB() && nullptr != GetTool();

	l_bOk = ( S_OK == GetEdgeA()->m_svLinearEdges.GetValues( l_svAEdges ) ) && l_bOk;
	l_bOk = ( S_OK == GetEdgeB()->m_svLinearEdges.GetValues( l_svBEdges ) ) && l_bOk;

	if( l_bOk )
	{
		int l_iFResultAIndex = -1;
		int l_iFResultBIndex = -1;
		int l_iBResultAIndex = -1;
		int l_iBResultBIndex = -1;
		int offsetA = 0;
		int offsetB = 0;

		double l_dMaxForegroundDistance = 0.0;
		double l_dMaxBackgroundDistance = 0.0;

		int l_iACount = static_cast<int>(l_svAEdges.size());
		int l_iBCount = static_cast<int>(l_svBEdges.size());

		// Calc maximum foreground object...
		for( int indexA = 0; indexA < l_iACount && indexA + offsetB < l_iBCount; ++ indexA )
		{
			// Throw lower negative edges away...
			while( l_svAEdges.at( indexA ) >= l_svBEdges.at( indexA + offsetB ) &&
					 indexA + ( ++offsetB ) < l_iBCount );

			// Check for indexB bounds...
			if( indexA + offsetB >= l_iBCount )
				break;

			if( l_svBEdges.at( indexA + offsetB ) - l_svAEdges.at( indexA ) > l_dMaxForegroundDistance )
			{
				l_dMaxForegroundDistance = l_svBEdges.at( indexA + offsetB ) - l_svAEdges.at( indexA );

				l_iFResultAIndex = indexA;
				l_iFResultBIndex = indexA + offsetB;
			}
		}

		// Calc maximum background object...
		for( int indexB = 0; indexB < l_iBCount && indexB + offsetA < l_iACount; ++ indexB )
		{
			// Throw lower positive edges away...
			while( l_svBEdges.at( indexB ) >= l_svAEdges.at( indexB + offsetA ) &&
					 indexB + ( ++offsetA ) < l_iACount );

			// Check for indexA bounds...
			if( indexB + offsetA >= l_iACount )
				break;

			if( l_svAEdges.at( indexB + offsetA ) - l_svBEdges.at( indexB ) > l_dMaxBackgroundDistance )
			{
				l_dMaxBackgroundDistance = l_svAEdges.at( indexB + offsetA ) - l_svBEdges.at( indexB );
				
				l_iBResultAIndex = indexB + offsetA;
				l_iBResultBIndex = indexB;
			}
		}

		if( l_dMaxBackgroundDistance > l_dMaxForegroundDistance )
		{
			if( 0 <= l_iBResultAIndex )
			{
				l_dDistanceA = l_svAEdges.at( l_iBResultAIndex );
			}
			else
			{
				RRunStatus.SetFailed();
			}

			if(	0 <= l_iBResultBIndex )
			{
				l_dDistanceB = l_svBEdges.at( l_iBResultBIndex );
			}
			else
			{
				RRunStatus.SetFailed();
			}
		}
		else
		{
			if( 0 <= l_iFResultAIndex )
			{
				l_dDistanceA = l_svAEdges.at( l_iFResultAIndex );
			}
			else
			{
				RRunStatus.SetFailed();
			}

			if(	0 <= l_iFResultBIndex )
			{
				l_dDistanceB = l_svBEdges.at( l_iFResultBIndex );
			}
			else
			{
				RRunStatus.SetFailed();
			}
		}

		l_bOk &= S_OK == GetEdgeA()->GetPointFromDistance( l_dDistanceA, l_svEdgePointA );
		l_bOk &= S_OK == GetEdgeB()->GetPointFromDistance( l_dDistanceB, l_svEdgePointB );
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

	l_dWidth = l_dDistanceB - l_dDistanceA;

	l_bOk = ( S_OK == m_svLinearDistanceA.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceA) ) && l_bOk;
	l_bOk = ( S_OK == m_svLinearDistanceB.SetValue(RRunStatus.m_lResultDataIndex, l_dDistanceB) ) && l_bOk;
	
	l_bOk = ( S_OK == mdpEdgeA.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointA) ) && l_bOk;
	l_bOk = ( S_OK == mdpEdgeB.SetValue(RRunStatus.m_lResultDataIndex, l_svDPointB) ) && l_bOk;
	l_bOk = ( S_OK == mdpCenter.SetValue(RRunStatus.m_lResultDataIndex, l_svCenterPoint) ) && l_bOk;
	l_bOk = ( S_OK == mdWidth.SetValue(RRunStatus.m_lResultDataIndex, fabs(l_dWidth) ) ) && l_bOk;

	if ( !l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	return l_bOk;
}

BOOL SVLinearMaximumObjectLineAnalyzerClass::OnValidate()
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
