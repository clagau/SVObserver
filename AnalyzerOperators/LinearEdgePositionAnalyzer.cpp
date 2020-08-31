// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : LinearEdgePositionAnalyzer
// * .File Name       : $Workfile:   LinearEdgePositionAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:59:10  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinearEdgePositionAnalyzer.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( LinearEdgePositionAnalyzer, SvPb::LinearEdgePositionLineAnalyzerClassId);

LinearEdgePositionAnalyzer::LinearEdgePositionAnalyzer( SVObjectClass* POwner, int StringResourceID )
								:SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	init();
}

void LinearEdgePositionAnalyzer::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLinearEdgePositionAnalyzerObjectType;

	SvOp::SVLinearEdgeProcessingClass *pEdge = new SvOp::SVLinearEdgeAProcessingClass( this );

	AddFriend( pEdge->getObjectId() );

	RegisterEmbeddedObject( &dpEdge, SvPb::DPEdgeAEId, IDS_OBJECTNAME_DPEDGE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistance, SvPb::LinearDistanceEdgeAEId, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	m_svLinearDistance.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SvPb::ShowAllEdgeAOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );

	m_bEnableDirection = true;
	m_bEnableEdgeSelect = true;
	m_bEnablePolarisation = true;
	m_bEnablePosition = true;
	m_bEnableThreshold = true;

	// Set Embedded defaults
	SVPoint<double> defaultPoint(0,0);
	dpEdge.SetDefaultValue( defaultPoint, true);
	dpEdge.setSaveValueFlag(false);

	// Add the Sub-pixel Edge X and Y Results...
	addDPointResultToAvailableChildren(SvPb::DPEdgeAEId, IDS_OBJECTNAME_DPEDGE);

	// Add the Linear Distance (EdgeA) Result...
	addScalarResultToAvailableChildren(SvPb::LinearDistanceEdgeAEId, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

LinearEdgePositionAnalyzer::~LinearEdgePositionAnalyzer()
{
	LinearEdgePositionAnalyzer::CloseObject();
}

bool LinearEdgePositionAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	dpEdge.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistance.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool LinearEdgePositionAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages);
}

void LinearEdgePositionAnalyzer::addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical) const
{
	auto* pGroup = rOverlay.add_shapegroups();
	pGroup->set_detaillevel(SvPb::Level1);
	pGroup->set_name("LAnalyzer-Result");

	if (nullptr != GetEdgeA() && nullptr != pGroup)
	{
		GetEdgeA()->addOverlayResultMarker(*pGroup, isVertical);
	}
}

std::vector<std::string> LinearEdgePositionAnalyzer::getParameterNamesForML() const
{
	return {
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY),
	};
}

bool LinearEdgePositionAnalyzer::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVPoint<double> edgePoint;
	double Distance( 0.0 );

	// All inputs and outputs must be validated first
	bool Result = __super::onRun( rRunStatus, pErrorMessages ) && ValidateEdgeA(pErrorMessages);
	if (nullptr == GetTool())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( Result )
	{
		if( S_OK != GetEdgeA()->GetOutputEdgePoint( edgePoint ) )
		{
			rRunStatus.SetFailed();	
		}

		if( S_OK != GetEdgeA()->GetOutputEdgeDistance( Distance ) )
		{
			rRunStatus.SetFailed();	
		}

		Result &= S_OK == GetImageExtent().TranslateFromOutputSpace( edgePoint, edgePoint );

		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
		Result &= pTool && S_OK == pTool->GetImageExtent().TranslateFromOutputSpace( edgePoint, edgePoint );

		if (!Result && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( S_OK != m_svLinearDistance.SetValue(Distance) || S_OK != dpEdge.SetValue(edgePoint) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}
	
	if( !Result )
	{
		rRunStatus.SetInvalid();	
	}

	return Result;
}

} //namespace SvAo