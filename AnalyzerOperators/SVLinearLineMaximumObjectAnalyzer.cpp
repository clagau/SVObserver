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
#include "Definitions/LinearEdgeEnums.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "Operators/SVLinearEdgeBProcessingClass.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS( SVLinearMaximumObjectLineAnalyzerClass, SvPb::LinearMaximumObjectLineAnalyzerClassId);

SVLinearMaximumObjectLineAnalyzerClass::SVLinearMaximumObjectLineAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( POwner, StringResourceID ) 
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
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLinearMaximumObjectAnalyzerObjectType;

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

	// Set Edge defaults
	SvOp::SVLinearEdgeProcessingClass *pEdgeA = new SvOp::SVLinearEdgeAProcessingClass(this);
	SvOp::SVLinearEdgeProcessingClass *pEdgeB = new SvOp::SVLinearEdgeBProcessingClass(this);

	if( nullptr != pEdgeA )
	{
		AddFriend(pEdgeA->getObjectId());

		pEdgeA->m_svPolarisation.SetDefaultValue(SvDef::SV_POSITIVE_POLARISATION, true);

		pEdgeA->m_svEdgeSelect.SetDefaultValue(SvDef::SV_THIS_EDGE, true);
		pEdgeA->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true );
	}

	if( nullptr != pEdgeB )
	{
		AddFriend(pEdgeB->getObjectId());

		pEdgeB->m_svPolarisation.SetDefaultValue(SvDef::SV_NEGATIVE_POLARISATION, true);

		pEdgeB->m_svEdgeSelect.SetDefaultValue(SvDef::SV_THIS_EDGE, true);
		pEdgeB->m_svEdgeSelectThisValue.SetDefaultValue(1.0, true);
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

	// Set default inputs and outputs
	addDefaultInputObjects();
	
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

SVLinearMaximumObjectLineAnalyzerClass::~SVLinearMaximumObjectLineAnalyzerClass()
{
	SVLinearMaximumObjectLineAnalyzerClass::CloseObject();
}

bool SVLinearMaximumObjectLineAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	mdpEdgeA.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_svLinearDistanceA.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVLinearMaximumObjectLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

HRESULT SVLinearMaximumObjectLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	double dDistance = 0.0;

	if(nullptr != GetEdgeA() && S_OK == m_svLinearDistanceA.GetValue(dDistance))
	{
		SVExtentLineStruct line;

		if( S_OK == GetEdgeA()->GetEdgeOverlayFromDistance(dDistance, line) )
		{
			GetImageExtent().TranslateFromOutputSpace(line, line);

			p_MultiLine.m_svLineArray.emplace_back( line );
		}
	}

	if(nullptr != GetEdgeB() && S_OK == m_svLinearDistanceB.GetValue(dDistance))
	{
		SVExtentLineStruct line;

		if( S_OK == GetEdgeB()->GetEdgeOverlayFromDistance(dDistance, line) )
		{
			GetImageExtent().TranslateFromOutputSpace(line, line);

			p_MultiLine.m_svLineArray.push_back(line);
		}
	}
	return S_OK;
}

void SVLinearMaximumObjectLineAnalyzerClass::addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical) const
{
	auto* pGroup = rOverlay.add_shapegroups();
	pGroup->set_detaillevel(SvPb::Level1);
	pGroup->set_name("LAnalyzer-Result");
	RECT rect{};
	SvAo::SVAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass*>(GetAnalyzer());
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

bool SVLinearMaximumObjectLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVImageExtentClass Extents;
	std::vector<double> AEdges;
	std::vector<double> BEdges;
	SVPoint<double> edgePointA;
	SVPoint<double> edgePointB;
	double DistanceA( 0.0 );
	double DistanceB( 0.0 );

	bool Result = __super::onRun(rRunStatus, pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
	
	if ( nullptr == GetTool() || S_OK != GetEdgeA()->m_svLinearEdges.GetArrayValues( AEdges ) || S_OK != GetEdgeB()->m_svLinearEdges.GetArrayValues( BEdges ) )
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
		int l_iFResultAIndex = -1;
		int l_iFResultBIndex = -1;
		int l_iBResultAIndex = -1;
		int l_iBResultBIndex = -1;
		int offsetA = 0;
		int offsetB = 0;

		double l_dMaxForegroundDistance = 0.0;
		double l_dMaxBackgroundDistance = 0.0;

		int l_iACount = static_cast<int>(AEdges.size());
		int l_iBCount = static_cast<int>(BEdges.size());

		// Calc maximum foreground object...
		for( int indexA = 0; indexA < l_iACount && indexA + offsetB < l_iBCount; ++ indexA )
		{
			// Throw lower negative edges away...
			while( AEdges.at( indexA ) >= BEdges.at( indexA + offsetB ) && indexA + ( ++offsetB ) < l_iBCount );

			// Check for indexB bounds...
			if( indexA + offsetB >= l_iBCount )
				break;

			if( BEdges.at( indexA + offsetB ) - AEdges.at( indexA ) > l_dMaxForegroundDistance )
			{
				l_dMaxForegroundDistance = BEdges.at( indexA + offsetB ) - AEdges.at( indexA );

				l_iFResultAIndex = indexA;
				l_iFResultBIndex = indexA + offsetB;
			}
		}

		// Calc maximum background object...
		for( int indexB = 0; indexB < l_iBCount && indexB + offsetA < l_iACount; ++ indexB )
		{
			// Throw lower positive edges away...
			while( BEdges.at( indexB ) >= AEdges.at( indexB + offsetA ) && indexB + ( ++offsetA ) < l_iACount );

			// Check for indexA bounds...
			if( indexB + offsetA >= l_iACount )
			{
				break;
			}

			if( AEdges.at( indexB + offsetA ) - BEdges.at( indexB ) > l_dMaxBackgroundDistance )
			{
				l_dMaxBackgroundDistance = AEdges.at( indexB + offsetA ) - BEdges.at( indexB );
				
				l_iBResultAIndex = indexB + offsetA;
				l_iBResultBIndex = indexB;
			}
		}

		if( l_dMaxBackgroundDistance > l_dMaxForegroundDistance )
		{
			if( 0 <= l_iBResultAIndex )
			{
				DistanceA = AEdges.at( l_iBResultAIndex );
			}
			else
			{
				rRunStatus.SetFailed();
			}

			if(	0 <= l_iBResultBIndex )
			{
				DistanceB = BEdges.at( l_iBResultBIndex );
			}
			else
			{
				rRunStatus.SetFailed();
			}
		}
		else
		{
			if( 0 <= l_iFResultAIndex )
			{
				DistanceA = AEdges.at( l_iFResultAIndex );
			}
			else
			{
				rRunStatus.SetFailed();
			}

			if(	0 <= l_iFResultBIndex )
			{
				DistanceB = BEdges.at( l_iFResultBIndex );
			}
			else
			{
				rRunStatus.SetFailed();
			}
		}

		Result &= S_OK == GetEdgeA()->GetPointFromDistance( DistanceA, edgePointA );
		Result &= S_OK == GetEdgeB()->GetPointFromDistance( DistanceB, edgePointB );
	}

	Result &= S_OK == GetImageExtent().TranslateFromOutputSpace(edgePointA, edgePointA) &&
			S_OK == GetImageExtent().TranslateFromOutputSpace(edgePointB, edgePointB);

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	Result &= pTool && S_OK == pTool->GetImageExtent().TranslateFromOutputSpace(edgePointA, edgePointA) &&
			S_OK == pTool->GetImageExtent().TranslateFromOutputSpace(edgePointB, edgePointB);

	SVPoint<double> centerPoint
	{
		edgePointA.m_x + ((edgePointB.m_x - edgePointA.m_x) / 2.0),
		edgePointA.m_y + ((edgePointB.m_y - edgePointA.m_y) / 2.0)
	};

	double Width = fabs( DistanceB - DistanceA );

	Result = ( S_OK == m_svLinearDistanceA.SetValue(DistanceA) ) && Result;
	Result = ( S_OK == m_svLinearDistanceB.SetValue(DistanceB) ) && Result;
	
	Result = ( S_OK == mdpEdgeA.SetValue(edgePointA) ) && Result;
	Result = ( S_OK == mdpEdgeB.SetValue(edgePointB) ) && Result;
	Result = ( S_OK == mdpCenter.SetValue(centerPoint) ) && Result;
	Result = ( S_OK == mdWidth.SetValue(Width) ) && Result;

	if ( !Result )
	{
		rRunStatus.SetInvalid();

		if (nullptr != pErrorMessages && pErrorMessages->empty())
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

} //namespace SvAo