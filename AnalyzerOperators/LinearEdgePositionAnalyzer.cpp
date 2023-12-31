// ******************************************************************************
// * COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/RunStatus.h"
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
								:LinearAnalyzer( POwner, StringResourceID ) 
{
	init();
}

void LinearEdgePositionAnalyzer::init()
{
	// Identify our type
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearEdgePositionAnalyzerObjectType;

	SvOp::SVLinearEdgeProcessingClass *pEdge = new SvOp::SVLinearEdgeAProcessingClass( this );

	AddFriend( pEdge );

	RegisterEmbeddedObject( &dpEdge, SvPb::DPEdgeAEId, IDS_OBJECTNAME_DPEDGE, false, SvOi::SVResetItemNone, false );
	RegisterEmbeddedObject( &m_svLinearDistance, SvPb::LinearDistanceEdgeAEId, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone, false );
	m_svLinearDistance.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SvPb::ShowAllEdgeAOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone, true);

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
}

LinearEdgePositionAnalyzer::~LinearEdgePositionAnalyzer()
{
	LinearEdgePositionAnalyzer::CloseObject();
}

bool LinearEdgePositionAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = LinearAnalyzer::CreateObject(rCreateStructure);

	dpEdge.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistance.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

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
	if (nullptr != GetEdgeA() && nullptr != pGroup)
	{
		pGroup->set_detaillevel(SvPb::Level1);
		pGroup->set_name("LAnalyzer-Result");
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

bool LinearEdgePositionAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
		const SVImageExtentClass& rAnalyzerExtents = GetImageExtent();

		Result &= S_OK == rAnalyzerExtents.TranslateFromOutputSpace( edgePoint, edgePoint );

		
		if (auto pToolExtent = getToolExtentPtr(); Result &&  nullptr != pToolExtent)
		{
			Result = S_OK == pToolExtent->TranslateFromOutputSpace(edgePoint, edgePoint);;
		}
		else 
		{ 
			Result = false;
		}

		if (!Result && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( S_OK != m_svLinearDistance.SetValue(Distance) || S_OK != dpEdge.SetValue(edgePoint) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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