// ******************************************************************************
// * COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : LinearMeasurementAnalyzer
// * .File Name       : $Workfile:   LinearMeasurementAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:04:02  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinearMeasurementAnalyzer.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/LinearEdgeEnums.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "Operators/SVLinearEdgeBProcessingClass.h"
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

SV_IMPLEMENT_CLASS( LinearMeasurementAnalyzer, SvPb::LinearMeasurementAnalyzerClassId);

LinearMeasurementAnalyzer::LinearMeasurementAnalyzer( SVObjectClass* POwner, int StringResourceID )
									 :LinearAnalyzer( POwner, StringResourceID ) 
{
	m_bEnableDirection = true;
	m_bEnableEdgeSelect = true;
	m_bEnablePolarisation = true;
	m_bEnablePosition = true;
	m_bEnableThreshold = true;

	init();
}

void LinearMeasurementAnalyzer::init()
{
	// Identify our type
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearMeasurementAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SvPb::DPEdgeAEId, IDS_OBJECTNAME_DPEDGE_A, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject( &mdpEdgeB, SvPb::DPEdgeBEId, IDS_OBJECTNAME_DPEDGE_B, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject( &mdpCenter, SvPb::DPCenterEId, IDS_OBJECTNAME_DPCENTER, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject( &mdWidth, SvPb::DWidthEId, IDS_OBJECTNAME_DWIDTH, false, SvOi::SVResetItemNone, false);

	RegisterEmbeddedObject( &m_svLinearDistanceA, SvPb::LinearDistanceEdgeAEId, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone, false);
	m_svLinearDistanceA.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svLinearDistanceB, SvPb::LinearDistanceEdgeBEId, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SvOi::SVResetItemNone, false);
	m_svLinearDistanceB.setSaveValueFlag(false);

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SvPb::ShowAllEdgeAOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SvPb::ShowAllEdgeBOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SvOi::SVResetItemNone, true);

	// Set Edge defaults
	SvOp::SVLinearEdgeProcessingClass *pEdgeA = new SvOp::SVLinearEdgeAProcessingClass(this);
	SvOp::SVLinearEdgeProcessingClass *pEdgeB = new SvOp::SVLinearEdgeBProcessingClass(this);

	if( nullptr != pEdgeA )
	{
		AddFriend(pEdgeA);
	}

	if( nullptr != pEdgeB )
	{
		AddFriend(pEdgeB);

		pEdgeB->setEdgeSelectDefaultValue(SvDef::SV_LAST_EDGE);
	}

	// Set Embedded defaults
	SVPoint<double> defaultPoint(0,0);
	mdpEdgeA.SetDefaultValue( defaultPoint, true );
	mdpEdgeA.setSaveValueFlag(false);
	mdpEdgeB.SetDefaultValue( defaultPoint, true );
	mdpEdgeB.setSaveValueFlag(false);
	mdpCenter.SetDefaultValue( defaultPoint, true );
	mdpCenter.setSaveValueFlag(false);
	mdWidth.SetDefaultValue( 0.0, true );
	mdWidth.setSaveValueFlag(false);

	// Add the Sub-pixel EdgeA X and Y Results...
	addDPointResultToAvailableChildren(SvPb::DPEdgeAEId, IDS_OBJECTNAME_DPEDGE_A);

	// Add the Sub-pixel EdgeB X and Y Results...
	addDPointResultToAvailableChildren(SvPb::DPEdgeBEId, IDS_OBJECTNAME_DPEDGE_B);

	// Add the Sub-pixel Center X and Y Results...
	addDPointResultToAvailableChildren(SvPb::DPCenterEId, IDS_OBJECTNAME_DPCENTER);

	// Add the Sub-pixel Width Result...
	addScalarResultToAvailableChildren(SvPb::DWidthEId, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_DWIDTH, IDS_RESULT_STRING);

	// Add the Linear Distance (EdgeA) Result...
	addScalarResultToAvailableChildren(SvPb::LinearDistanceEdgeAEId, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT);

	// Add the Linear Distance (EdgeB) Result...
	addScalarResultToAvailableChildren(SvPb::LinearDistanceEdgeBEId, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT);
}


LinearMeasurementAnalyzer::~LinearMeasurementAnalyzer()
{
	LinearMeasurementAnalyzer::CloseObject();
}

bool LinearMeasurementAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = LinearAnalyzer::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	mdpEdgeA.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_svLinearDistanceA.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool LinearMeasurementAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

void LinearMeasurementAnalyzer::addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical) const
{
	auto* pGroup = rOverlay.add_shapegroups();
	if (nullptr != pGroup)
	{
		pGroup->set_detaillevel(SvPb::Level1);
		pGroup->set_name("LAnalyzer-Result");

		if (nullptr != GetEdgeA())
		{
			GetEdgeA()->addOverlayResultMarker(*pGroup, isVertical);
		}

		if (nullptr != GetEdgeB())
		{
			GetEdgeB()->addOverlayResultMarker(*pGroup, isVertical);
		}
	}
}

bool LinearMeasurementAnalyzer::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = __super::onRun( rRunStatus, pErrorMessages ) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);

	if ( nullptr == GetTool())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if ( Result )
	{
	
		const SVImageExtentClass& rAnalyzerExtents = GetImageExtent();

		SVPoint<double> edgePointA;
		if ( S_OK != GetEdgeA()->GetOutputEdgePoint(edgePointA) )
		{
			rRunStatus.SetFailed();
		}

		double DistanceA(0.0);
		if ( S_OK != GetEdgeA()->GetOutputEdgeDistance(DistanceA) )
		{
			rRunStatus.SetFailed();
		}

		SVPoint<double> edgePointB;
		if ( S_OK != GetEdgeB()->GetOutputEdgePoint(edgePointB) )
		{
			rRunStatus.SetFailed();
		}

		double DistanceB(0.0);
		if ( S_OK != GetEdgeB()->GetOutputEdgeDistance(DistanceB) )
		{
			rRunStatus.SetFailed();
		}

		Result &= S_OK == rAnalyzerExtents.TranslateFromOutputSpace(edgePointA, edgePointA) &&
				S_OK == rAnalyzerExtents.TranslateFromOutputSpace(edgePointB, edgePointB);

	
		const SvTo::SVToolExtentClass* pToolExtent = getToolExtentPtr();
		Result &= pToolExtent && S_OK == pToolExtent->TranslateFromOutputSpace(edgePointA, edgePointA) &&
				S_OK == pToolExtent->TranslateFromOutputSpace(edgePointB, edgePointB);

		SVPoint<double> CenterPoint
		{
			edgePointA.m_x + ((edgePointB.m_x - edgePointA.m_x) / 2.0),
			edgePointA.m_y + ((edgePointB.m_y - edgePointA.m_y) / 2.0)
		};

		double Width = fabs( DistanceB - DistanceA + 1.0 );

		Result = ( S_OK == m_svLinearDistanceA.SetValue(DistanceA) ) && Result;
		Result = ( S_OK == m_svLinearDistanceB.SetValue(DistanceB) ) && Result;
		
		Result = ( S_OK == mdpEdgeA.SetValue(edgePointA) ) && Result;
		Result = ( S_OK == mdpEdgeB.SetValue(edgePointB) ) && Result;
		Result = ( S_OK == mdpCenter.SetValue(CenterPoint) ) && Result;
		Result = ( S_OK == mdWidth.SetValue(Width) ) && Result;
	}

	if ( !Result )
	{
		rRunStatus.SetInvalid();

		if (nullptr != pErrorMessages && pErrorMessages->empty())
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

std::vector<std::string> LinearMeasurementAnalyzer::getParameterNamesForML() const
{
	return {
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_A) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_A) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_B) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_B) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY)
	};
}

} //namespace SvAo
