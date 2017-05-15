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
#include "SVOCore/SVTool.h"
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
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVLinearEdgePositionAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdge = new SVLinearEdgeAProcessingClass( this );

	AddFriend( l_pEdge->GetUniqueObjectID() );

	RegisterEmbeddedObject( &dpEdge, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistance, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );

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

	dpEdge.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistance.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

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

bool SVLinearEdgePositionLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVDPointClass DPoint;
	SVExtentPointStruct EdgePoint;
	double Distance( 0.0 );

	// All inputs and outputs must be validated first
	bool Result = __super::onRun( rRunStatus, pErrorMessages ) && ValidateEdgeA(pErrorMessages);
	if (nullptr == GetTool())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( Result )
	{
		SVImageExtentClass Extents;

		if( S_OK != GetEdgeA()->GetOutputEdgePoint( EdgePoint ) )
		{
			rRunStatus.SetFailed();	
		}

		if( S_OK != GetEdgeA()->GetOutputEdgeDistance( Distance ) )
		{
			rRunStatus.SetFailed();	
		}

		Result &= S_OK == GetImageExtent( Extents ) && S_OK == Extents.TranslateFromOutputSpace( EdgePoint, EdgePoint );

		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		Result &= pTool && S_OK == pTool->GetImageExtent( Extents ) && S_OK == Extents.TranslateFromOutputSpace( EdgePoint, EdgePoint );

		if (!Result && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	DPoint.x = EdgePoint.m_dPositionX;
	DPoint.y = EdgePoint.m_dPositionY;

	if( S_OK != m_svLinearDistance.SetValue( Distance, rRunStatus.m_lResultDataIndex ) ||
		 S_OK != dpEdge.SetValue( DPoint, rRunStatus.m_lResultDataIndex ) ) 
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	
	if( !Result )
	{
		SetInvalid();
		rRunStatus.SetInvalid();	
	}

	return Result;
}
