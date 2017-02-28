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
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge Result...
	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
	
	// Add the Sub-pixel Edge X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE );
	SVString Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	// Add the Sub-pixel Edge Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE );
	Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A );
	Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

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

bool SVLinearEdgePositionLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages);
}

bool SVLinearEdgePositionLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVDPointClass l_svDPoint;
	SVExtentPointStruct l_svEdgePoint;
	double l_dDistance = 0.0;

	// All inputs and outputs must be validated first
	bool l_bOk = __super::onRun( RRunStatus, pErrorMessages ) && ValidateEdgeA(pErrorMessages);
	if (nullptr == GetTool())
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

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

		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		l_bOk &= pTool && S_OK == pTool->GetImageExtent( l_svExtents ) &&
		         S_OK == l_svExtents.TranslateFromOutputSpace( l_svEdgePoint, l_svEdgePoint );

		if (!l_bOk && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	l_svDPoint.x = l_svEdgePoint.m_dPositionX;
	l_svDPoint.y = l_svEdgePoint.m_dPositionY;

	if( S_OK != m_svLinearDistance.SetValue( RRunStatus.m_lResultDataIndex, l_dDistance ) ||
		 S_OK != dpEdge.SetValue( RRunStatus.m_lResultDataIndex, l_svDPoint ) ) 
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	
	if( ! l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();	
	}

	return l_bOk;
}
