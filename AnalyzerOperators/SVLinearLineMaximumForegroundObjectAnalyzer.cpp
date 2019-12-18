// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearMaximumForegroundObjectLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLineMaximumForegroundObjectAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 14:57:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineMaximumForegroundObjectAnalyzer.h"
#include "Definitions/LinearEdgeEnums.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "Operators/SVLinearEdgeBProcessingClass.h"
#include "SVObjectLibrary/SVClsIds.h"
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

SV_IMPLEMENT_CLASS( SVLinearMaximumForegroundObjectLineAnalyzerClass, SVLinearMaximumForegroundObjectLineAnalyzerClassGuid );

SVLinearMaximumForegroundObjectLineAnalyzerClass::SVLinearMaximumForegroundObjectLineAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
										   :SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void SVLinearMaximumForegroundObjectLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVLinearMaximumForegroundObjectAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	m_svLinearDistanceA.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SvOi::SVResetItemNone );
	m_svLinearDistanceB.setSaveValueFlag(false);

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SvOi::SVResetItemNone );

    //
	// Set Edge defaults

	SvOp::SVLinearEdgeProcessingClass *pEdgeA = new SvOp::SVLinearEdgeAProcessingClass(this);
	SvOp::SVLinearEdgeProcessingClass *pEdgeB = new SvOp::SVLinearEdgeBProcessingClass(this);

	if( nullptr != pEdgeA )
	{
		AddFriend(pEdgeA->GetUniqueObjectID());

		pEdgeA->m_svPolarisation.SetDefaultValue(SvDef::SV_POSITIVE_POLARISATION, true);

		pEdgeA->m_svEdgeSelect.SetDefaultValue(SvDef::SV_THIS_EDGE, true);
		pEdgeA->m_svEdgeSelectThisValue.SetDefaultValue(1.0, true);
	}

	if( nullptr != pEdgeB )
	{
		AddFriend(pEdgeB->GetUniqueObjectID());

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
	addDPointResultToAvailableChildren(SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A);

	// Add the Sub-pixel EdgeB X and Y Results...
	addDPointResultToAvailableChildren(SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B);

	// Add the Sub-pixel Center X and Y Results...
	addDPointResultToAvailableChildren(SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER);

	// Add the Sub-pixel Width Result...
	addScalarResultToAvailableChildren(SVDWidthObjectGuid, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_DWIDTH, IDS_RESULT_STRING);

	// Add the Linear Distance (EdgeA) Result...
	addScalarResultToAvailableChildren(SVLinearDistanceEdgeAObjectGuid, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT);

	// Add the Linear Distance (EdgeB) Result...
	addScalarResultToAvailableChildren(SVLinearDistanceEdgeBObjectGuid, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT);
}

SVLinearMaximumForegroundObjectLineAnalyzerClass::~SVLinearMaximumForegroundObjectLineAnalyzerClass()
{
	CloseObject();
}

bool SVLinearMaximumForegroundObjectLineAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
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

bool SVLinearMaximumForegroundObjectLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

HRESULT SVLinearMaximumForegroundObjectLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct& rMultiLine )
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

bool SVLinearMaximumForegroundObjectLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	std::vector<double> AEdges;
	std::vector<double> BEdges;
	SVPoint<double> edgePointA;
	SVPoint<double> edgePointB;
	double DistanceA( 0.0 );
	double DistanceB( 0.0 );

	bool l_bOk = __super::onRun( rRunStatus, pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
	
	if (nullptr == GetTool() || S_OK != GetEdgeA()->m_svLinearEdges.GetArrayValues( AEdges ) || S_OK != GetEdgeB()->m_svLinearEdges.GetArrayValues( BEdges ) )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( l_bOk )
	{
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return l_bOk;
}

} //namespace SvAo