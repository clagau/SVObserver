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
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS( SVLinearEdgePositionLineAnalyzerClass, SvPb::LinearEdgePositionLineAnalyzerClassId);

SVLinearEdgePositionLineAnalyzerClass::SVLinearEdgePositionLineAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
								:SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	init();
}

void SVLinearEdgePositionLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVLinearEdgePositionAnalyzerObjectType;

	SvOp::SVLinearEdgeProcessingClass *pEdge = new SvOp::SVLinearEdgeAProcessingClass( this );

	AddFriend( pEdge->GetUniqueObjectID() );

	RegisterEmbeddedObject( &dpEdge, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistance, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	m_svLinearDistance.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );

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
	addDPointResultToAvailableChildren(SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE);

	// Add the Linear Distance (EdgeA) Result...
	addScalarResultToAvailableChildren(SVLinearDistanceEdgeAObjectGuid, SvPb::SVResultDoubleObjectType, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLinearEdgePositionLineAnalyzerClass::~SVLinearEdgePositionLineAnalyzerClass()
{
	SVLinearEdgePositionLineAnalyzerClass::CloseObject();
}

bool SVLinearEdgePositionLineAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	dpEdge.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistance.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVLinearEdgePositionLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages);
}

std::vector<std::string> SVLinearEdgePositionLineAnalyzerClass::getParameterNamesForML() const
{
	return {
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY),
	};
}

bool SVLinearEdgePositionLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( S_OK != m_svLinearDistance.SetValue(Distance) || S_OK != dpEdge.SetValue(edgePoint) )
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
		rRunStatus.SetInvalid();	
	}

	return Result;
}

} //namespace SvAo