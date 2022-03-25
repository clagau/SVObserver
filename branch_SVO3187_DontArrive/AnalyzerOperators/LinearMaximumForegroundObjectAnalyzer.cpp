// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : LinearMaximumForegroundObjectAnalyzer
// * .File Name       : $Workfile:   LinearMaximumForegroundObjectAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 14:57:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinearMaximumForegroundObjectAnalyzer.h"
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

SV_IMPLEMENT_CLASS( LinearMaximumForegroundObjectAnalyzer, SvPb::LinearMaximumForegroundObjectLineAnalyzerClassId);

LinearMaximumForegroundObjectAnalyzer::LinearMaximumForegroundObjectAnalyzer( SVObjectClass* POwner, int StringResourceID )
										   :LinearAnalyzer( POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void LinearMaximumForegroundObjectAnalyzer::init()
{
	// Identify our type
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearMaximumForegroundObjectAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SvPb::DPEdgeAEId, IDS_OBJECTNAME_DPEDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SvPb::DPEdgeBEId, IDS_OBJECTNAME_DPEDGE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SvPb::DPCenterEId, IDS_OBJECTNAME_DPCENTER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SvPb::DWidthEId, IDS_OBJECTNAME_DWIDTH, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svLinearDistanceA, SvPb::LinearDistanceEdgeAEId, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	m_svLinearDistanceA.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svLinearDistanceB, SvPb::LinearDistanceEdgeBEId, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SvOi::SVResetItemNone );
	m_svLinearDistanceB.setSaveValueFlag(false);

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SvPb::ShowAllEdgeAOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SvPb::ShowAllEdgeBOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SvOi::SVResetItemNone );

    //
	// Set Edge defaults

	SvOp::SVLinearEdgeProcessingClass *pEdgeA = new SvOp::SVLinearEdgeAProcessingClass(this);
	SvOp::SVLinearEdgeProcessingClass *pEdgeB = new SvOp::SVLinearEdgeBProcessingClass(this);

	if( nullptr != pEdgeA )
	{
		AddFriend(pEdgeA);
		pEdgeA->setDefaultValues(SvDef::SV_POSITIVE_POLARISATION, SvDef::SV_THIS_EDGE, 1.0);
	}

	if( nullptr != pEdgeB )
	{
		AddFriend(pEdgeB);
		pEdgeA->setDefaultValues(SvDef::SV_NEGATIVE_POLARISATION, SvDef::SV_THIS_EDGE, 1.0);
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

LinearMaximumForegroundObjectAnalyzer::~LinearMaximumForegroundObjectAnalyzer()
{
	LinearMaximumForegroundObjectAnalyzer::CloseObject();
}

bool LinearMaximumForegroundObjectAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = LinearAnalyzer::CreateObject(rCreateStructure);
	mdpEdgeA.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceA.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool LinearMaximumForegroundObjectAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

HRESULT LinearMaximumForegroundObjectAnalyzer::GetSelectedEdgeOverlays( SVExtentMultiLineStruct& rMultiLine )
{
	double dDistance = 0.0;

	if(nullptr != GetEdgeA() && S_OK == m_svLinearDistanceA.GetValue( dDistance ))
	{
		SVExtentLineStruct line;

		if( S_OK == GetEdgeA()->GetEdgeOverlayFromDistance( dDistance, line ) )
		{
			GetImageExtent().TranslateFromOutputSpace( line, line );

			rMultiLine.m_svLineArray.push_back( line );
		}
	}

	if(nullptr != GetEdgeB() && S_OK == m_svLinearDistanceB.GetValue( dDistance ))
	{
		SVExtentLineStruct line;

		if( S_OK == GetEdgeB()->GetEdgeOverlayFromDistance( dDistance, line ) )
		{
			GetImageExtent().TranslateFromOutputSpace( line, line );

			rMultiLine.m_svLineArray.push_back( line );
		}
	}

	return S_OK;
}

void LinearMaximumForegroundObjectAnalyzer::addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical) const
{
	auto* pGroup = rOverlay.add_shapegroups();
	pGroup->set_detaillevel(SvPb::Level1);
	pGroup->set_name("LAnalyzer-Result");
	RECT rect{};
	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if (nullptr != pAnalyzer)
	{
		pAnalyzer->GetImageExtent().GetOutputRectangle(rect);
	}

	//edgeA
	auto* pResultAShape = pGroup->add_shapes();
	pResultAShape->mutable_color()->set_trpos(m_statusColor.getTrPos() + 1);
	auto* pResultMarker = pResultAShape->mutable_marker();
	pResultMarker->set_minvalue(rect.left);
	pResultMarker->set_maxvalue(rect.right);
	pResultMarker->mutable_value()->set_trpos(m_svLinearDistanceA.getTrPos() + 1);
	pResultMarker->set_sizetype(SvPb::Size::MidShort);
	if (isVertical)
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::x);
	}
	else
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::y);
	}
	//edgeB
	auto* pResultBShape = pGroup->add_shapes();
	pResultBShape->mutable_color()->set_trpos(m_statusColor.getTrPos() + 1);
	pResultMarker = pResultBShape->mutable_marker();
	pResultMarker->set_minvalue(rect.left);
	pResultMarker->set_maxvalue(rect.right);
	pResultMarker->mutable_value()->set_trpos(m_svLinearDistanceB.getTrPos() + 1);
	pResultMarker->set_sizetype(SvPb::Size::MidShort);
	if (isVertical)
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::x);
	}
	else
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::y);
	}
}

bool LinearMaximumForegroundObjectAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVPoint<double> edgePointA;
	SVPoint<double> edgePointB;
	double DistanceA( 0.0 );
	double DistanceB( 0.0 );

	bool l_bOk = __super::onRun( rRunStatus, pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
	
	if (nullptr == GetTool() )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( l_bOk )
	{
		std::vector<double> AEdges = GetEdgeA()->getLinearEdges();
		std::vector<double> BEdges = GetEdgeB()->getLinearEdges();
		int resultAIndex = -1;
		int resultBIndex = -1;
		int offsetB = 0;

		double l_dMaxForegroundDistance = 0.0;

		int l_iACount = static_cast<int>(AEdges.size());
		int l_iBCount = static_cast<int>(BEdges.size());

		// Calc maximum foregoundobject...
		for( int indexA = 0; indexA < l_iACount && indexA + offsetB < l_iBCount; ++ indexA )
		{
			// Throw lower negative edges away...
			while( AEdges.at( indexA ) >= BEdges.at( indexA + offsetB ) && indexA + ( ++offsetB ) < static_cast<int>(BEdges.size()) );

			// Check for indexB bounds...
			if( indexA + offsetB >= static_cast<int>(BEdges.size()) )
				break;

			if( BEdges.at( indexA + offsetB ) - AEdges.at( indexA ) > l_dMaxForegroundDistance )
			{
				l_dMaxForegroundDistance = BEdges.at( indexA + offsetB ) - AEdges.at( indexA );
				resultAIndex = indexA;
				resultBIndex = indexA + offsetB;
			}
		}

		if( 0 <= resultAIndex )
		{
			DistanceA = AEdges.at( resultAIndex );
		}
		else
		{
			rRunStatus.SetFailed();
		}

		if(	0 <= resultBIndex )
		{
			DistanceB = BEdges.at( resultBIndex );
		}
		else
		{
			rRunStatus.SetFailed();
		}

		l_bOk &= S_OK == GetEdgeA()->GetPointFromDistance( DistanceA, edgePointA );
		l_bOk &= S_OK == GetEdgeB()->GetPointFromDistance( DistanceB, edgePointB );
	}

	l_bOk &= S_OK == GetImageExtent().TranslateFromOutputSpace(edgePointA, edgePointA) &&
			S_OK == GetImageExtent().TranslateFromOutputSpace(edgePointB, edgePointB);

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	l_bOk &= pTool && S_OK == pTool->GetImageExtent().TranslateFromOutputSpace(edgePointA, edgePointA) &&
			S_OK == pTool->GetImageExtent().TranslateFromOutputSpace(edgePointB, edgePointB);

	SVPoint<double> centerPoint
	{
		edgePointA.m_x + ((edgePointB.m_x - edgePointA.m_x) / 2.0),
		edgePointA.m_y + ((edgePointB.m_y - edgePointA.m_y) / 2.0)
	};

	double Width = fabs( DistanceB - DistanceA );

	l_bOk = ( S_OK == m_svLinearDistanceA.SetValue(DistanceA) ) && l_bOk;
	l_bOk = ( S_OK == m_svLinearDistanceB.SetValue(DistanceB) ) && l_bOk;
	
	l_bOk = ( S_OK == mdpEdgeA.SetValue(edgePointA) ) && l_bOk;
	l_bOk = ( S_OK == mdpEdgeB.SetValue(edgePointB) ) && l_bOk;
	l_bOk = ( S_OK == mdpCenter.SetValue(centerPoint) ) && l_bOk;
	l_bOk = ( S_OK == mdWidth.SetValue(Width) ) && l_bOk;

	if ( !l_bOk )
	{
		rRunStatus.SetInvalid();

		if (nullptr != pErrorMessages && pErrorMessages->empty())
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return l_bOk;
}

} //namespace SvAo